//继电器结构体
struct relay
{
    uint8_t pin1;        //定义
    uint8_t pin2;        //定义
	bool state;      //状态
    void (*callback)(uint8_t, bool, const char*);  //回调函数
    uint32_t timer;     //计时
    uint32_t interval;   //时间
    char mid[9] = "";              //附加消息ID
	SYS_TOPIC topic;
    bool keep;       //状态保持
    uint16_t revert;     //恢复时间，单位秒
    uint32_t change;    //状态变化的时间
    bool trigger;            //是否因为状态保持触发事件
    bool init;            //是否初始化
};

//继电器结构体数组
struct relay RELAY[] =
{
    #ifdef HAS_RELAY_MAGNETIC
    {4,    5},
    #else
    {4,    0},
    #endif
};

bool relay_state(uint8_t num)
{
    return RELAY[num - 1].state;
}

void relay_state(uint8_t num, bool state)
{
    if (RELAY[num - 1].state != state)
    {
        RELAY[num - 1].change = millis();

        #ifdef HAS_POWER
        if (BL.enable)
        {
            //刷新值
            bl_refresh(true);

            //重置预警
            if (state) 
            {
                bl_reset_alert();
            }
        }
        #endif
    }
    
    RELAY[num - 1].state = state;

    if (RELAY[num - 1].pin2 > 0)
    {
        digitalWrite(RELAY[num - 1].pin1, state);
        digitalWrite(RELAY[num - 1].pin2, !state);

        device.delay(100);

        digitalWrite(RELAY[num - 1].pin1, 0);
        digitalWrite(RELAY[num - 1].pin2, 0);

        //device.Config.saveConfig("relay", num, "_state", state ? "1" : "0");
    }
    else
    {
        digitalWrite(RELAY[num - 1].pin1, state);
    }

    //重置计量脉冲
    if (state == 0)
    {
        //bl_reset_energy();
    }
}

void relay_state(uint8_t num, String oper)
{
    if (oper == "reverse")
    {
        bool state = relay_state(num);
        relay_state(num, !state);
    }
}

//清除relay上的任务和保持
void relay_clear(uint8_t num)
{
	RELAY[num - 1].timer = 0;
	RELAY[num - 1].interval = 0;
	RELAY[num - 1].revert = 0;
}

bool relay_trigger(uint8_t num)
{
    if (!RELAY[num - 1].init) 
    {
        return false;
    }
    
    return RELAY[num - 1].trigger;
}

void relay_trigger(uint8_t num, bool reset)
{
    RELAY[num - 1].trigger = reset;
}

void relay_keep(uint8_t num, const char* mid, int revert)
{
	relay_clear(num);

    RELAY[num - 1].keep = relay_state(num);
    RELAY[num - 1].revert = revert;
    strcpy(RELAY[num - 1].mid, mid);
}

//检查是否有动作
bool relay_task(uint8_t num)
{
	return RELAY[num - 1].interval > 0;
}

void relay_task(uint8_t num, uint32_t interval, const char* mid, void (*callback)(uint8_t, bool, const char*))
{
	relay_clear(num);

    //开始计时
    RELAY[num - 1].timer = millis();
    RELAY[num - 1].interval = interval;

    strcpy(RELAY[num - 1].mid, mid);
    RELAY[num - 1].callback = callback;
}

//继电器任务处理线程
//每10ms执行一次
void relay_handle()
{
    for (uint8_t i = 0; i < sizeof(RELAY) / sizeof(RELAY[0]); i++)
	{
        if (!RELAY[i].init) continue;

        //有动作
        if (RELAY[i].interval > 0 && RELAY[i].timer > 0 && device.millElapsed(RELAY[i].timer) >= RELAY[i].interval)
        {
            Serial.println(String(i));

            RELAY[i].interval = 0;
            RELAY[i].timer = 0;
            
            //结束动作
            if (RELAY[i].callback) RELAY[i].callback(i + 1, RELAY[i].state, RELAY[i].mid);
            RELAY[i].callback = NULL;
        }

        //led灯绑定
        //继电器关，蓝灯亮
        device.Peripheral.ledToggle(i + 1, RELAY[i].state);
        
        #ifdef HAS_RELAY_MAGNETIC
        device.Peripheral.ledToggle(2, RELAY[i].state);
        #endif

        //状态保持
        if (RELAY[i].keep != RELAY[i].state && RELAY[i].revert > 0 && device.millElapsed(RELAY[i].change) >= RELAY[i].revert * 1000)
        {
            relay_state(i + 1, RELAY[i].keep);

            RELAY[i].trigger = true;
        }
	}
}

void relay_setup()
{
    for (uint8_t i = 0; i < sizeof(RELAY) / sizeof(RELAY[0]); i++)
	{
        pinMode(RELAY[i].pin1, OUTPUT);
        if (RELAY[i].pin2) pinMode(RELAY[i].pin2, OUTPUT);

        relay_state(i + 1, device.Config.readConfig("relay", "0") == "1");

        RELAY[i].init = true;
	}

	device.Timer.addTimer(100, []() {
		relay_handle();
	});
}