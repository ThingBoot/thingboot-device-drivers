//SHT3x 温湿度传感器
#include <Wire.h>

struct sht
{
    uint8_t pin_sda = 2;
    uint8_t pin_scl = 14;

    bool enable = true;
    bool report = false;

    uint8_t address = 0x44;
    int MSB = 0;
    int LSB = 0;

    bool ready = false;
    uint32_t timer = 0;
    uint32_t readTimer = 0;

    float correction = 1.0;
    float sensitivity = 0.2;

    int temp_c = 0;
    int temp_c_last = 0;

    int humi = 0;
    int humi_last = 0;

    bool trigger = false;
} SHT;

void sht_trigger(bool reset)
{
    SHT.trigger = reset;
}

bool sht_trigger()
{
    return SHT.trigger;
}

void sht_mode(String mode)
{
    if (mode == "high_clock")
    {
        SHT.MSB = 0x2C;
        SHT.LSB = 0x06;
    }
    else if (mode == "medi_clock")
    {
        SHT.MSB = 0x2C;
        SHT.LSB = 0x0D;
    }
    else if (mode == "low_clock")
    {
        SHT.MSB = 0x2C;
        SHT.LSB = 0x10;
    }
    else if (mode == "high_no_clock")
    {
        SHT.MSB = 0x24;
        SHT.LSB = 0x00;
    }
    else if (mode == "medi_no_clock")
    {
        SHT.MSB = 0x24;
        SHT.LSB = 0x0B;
    }
    else if (mode == "low_no_clock")
    {
        SHT.MSB = 0x24;
        SHT.LSB = 0x16;
    }
    else
    {
        SHT.MSB = 0x2C;
        SHT.LSB = 0x06;
    }
}

void sht_read()
{
    Wire.beginTransmission(SHT.address);
    sht_mode("high_clock");
    Wire.write(SHT.MSB);
    Wire.write(SHT.LSB);

    if (Wire.endTransmission() != 0)
    {
        SHT.ready = false;
        return;
    }

    // 单次测量等待时间（高重复性时钟拉伸约 15ms）
    delay(15);

    Wire.requestFrom(SHT.address, (uint8_t)6);
    if (Wire.available() < 6)
    {
        SHT.ready = false;
        return;
    }

    uint8_t data[6];
    for (int i = 0; i < 6; i++)
    {
        data[i] = Wire.read();
    }

    // cTemp msb, cTemp lsb, cTemp crc, humidity msb, humidity lsb, humidity crc
    float temp_c = ((((data[0] * 256.0) + data[1]) * 175) / 65535.0) - 45;
    float humi = ((((data[3] * 256.0) + data[4]) * 100) / 65535.0);

    SHT.temp_c = int(temp_c * 10 * SHT.correction);
    SHT.humi = int(humi);

    SHT.ready = true;
}

void sht_enable(bool enable)
{
    SHT.enable = enable;
    SHT.report = true;
}

void sht_correction(String value)
{
    SHT.correction = value.toFloat();

    if (SHT.correction < 0.5) SHT.correction = 0.5;
    if (SHT.correction > 1.5) SHT.correction = 1.5;

    SHT.report = true;
}

void sht_sensitivity(String value)
{
    SHT.sensitivity = value.toFloat();

    if (SHT.sensitivity < 0.1) SHT.sensitivity = 0.1;
    if (SHT.sensitivity > 1) SHT.sensitivity = 1;

    SHT.report = true;
}

void sht_report()
{
    JSONVar state;
    state["temperature"] = SHT.temp_c / 10.0;
    state["humidity"] = SHT.humi;

    device.Event.reportEvent("sht", state);
}

void sht_handle()
{
    if (!SHT.enable) return;
    if (millis() < 2000) return;

    sht_read();

    // 温度变化超过灵敏度阈值，或湿度变化超过 1%，触发上报
    bool change = abs(SHT.temp_c - SHT.temp_c_last) > int(SHT.sensitivity * 10)
               || abs(SHT.humi - SHT.humi_last) >= 1;

    if (SHT.report || change)
    {
        SHT.report = false;

        SHT.temp_c_last = SHT.temp_c;
        SHT.humi_last = SHT.humi;

        SHT.trigger = true;

        sht_report();
    }
}

void sht_setup()
{
    Wire.begin(SHT.pin_sda, SHT.pin_scl);

    String enable = device.Config.readConfig("sht_enable", "1");
    sht_enable(enable.toInt());

    String correction = device.Config.readConfig("sht_correction", "1");
    sht_correction(correction);

    String sensitivity = device.Config.readConfig("sht_sensitivity", "0.2");
    sht_sensitivity(sensitivity);

    // 每 3 秒读取一次传感器
    device.Timer.addTimer(3000, []() {
        sht_handle();
    });
}
