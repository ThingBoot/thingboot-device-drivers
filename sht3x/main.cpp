//温湿度传感器
#include <Arduino.h>
#include "ThingBootSDK.h"

ThingBootDevice device;

//SHT3x 系列温湿度传感器驱动
#include "sht3x.h"

//调试信息打印回调
void onDebug(const char* category, const char* message)
{
    Serial.print("[");
    Serial.print(millis());
    Serial.print("] ");
    Serial.print(category);
    Serial.print(" | ");
    Serial.println(message);
}

//命令回调
void onOrder(const char* mid, JSONVar data)
{
    JSONVar ret;

    if (data.hasOwnProperty("sht_enable"))
    {
        sht_enable(CN(data["sht_enable"]) > 0);
        device.Config.saveConfig("sht_enable", SHT.enable ? "1" : "0");
        ret["sht_enable"] = SHT.enable ? "1" : "0";
    }

    if (data.hasOwnProperty("sht_correction"))
    {
        sht_correction(CS(data["sht_correction"]));
        device.Config.saveConfig("sht_correction", String(SHT.correction, 2).c_str());
        ret["sht_correction"] = String(SHT.correction, 2).c_str();
    }

    if (data.hasOwnProperty("sht_sensitivity"))
    {
        sht_sensitivity(CS(data["sht_sensitivity"]));
        device.Config.saveConfig("sht_sensitivity", String(SHT.sensitivity, 2).c_str());
        ret["sht_sensitivity"] = String(SHT.sensitivity, 2).c_str();
    }

    device.Message.publishMessage(TOPIC_ORDER, mid, C(ret));
}

void setup()
{
    Serial.begin(115200);

    //调试信息打印
    device.onDebug(onDebug);

    //产品信息
    device.setProduct(
        "product_key",     //产品代号[Key]
        "product_secret",  //产品密码[Secret]
        "board",           //适配PCB板，自由定义
        "mcu",             //适配MCU,esp8266
        "version.sub"      //当前固件版本号，先在控制台登记新版本，末段xxx为子版本号（自由定义）
    );

    //设备配置
    device.Config.addConfig("sht_enable", 10, 1);
    device.Config.addConfig("sht_correction", 11, 4);
    device.Config.addConfig("sht_sensitivity", 12, 4);

    //设置命令回调函数
    device.Order.onOrder(onOrder);

    //外设初始化
    device.Peripheral.onInit([]() {
        sht_setup();
    });

    device.setup();
}

void loop()
{
    device.loop();
}
