# ThingBoot Device Drivers

本仓库是 [ThingBoot Device SDK](https://github.com/ThingBoot/thingboot-device-sdk) 的驱动示例集合，面向需要在项目中使用 ThingBoot 的开发者。

> 注意：这里只包含已发布的驱动代码。如果你想参与编写或提交新的驱动，请访问驱动开发仓库。

## 仓库内容

每个子目录对应一个独立的驱动示例：

- `relay/`：继电器 / 开关 / 插座 / 通断器 / 断路器驱动示例
- `sht3x/`：SHT3x 系列温湿度传感器驱动示例

## 快速使用

1. 确保你的项目已接入 [thingboot-device-sdk](https://github.com/ThingBoot/thingboot-device-sdk)。
2. 将本仓库中对应驱动的整个目录（如 `relay/`）复制到你项目的 `src/` 目录下。
3. 根据你的硬件修改驱动代码中的引脚、数量等配置（通常集中在 `include/relay.h` 等头文件中）。
4. 编译并烧录到设备。

例如，使用继电器驱动：

```cpp
#include "relay.h"

// 在你的 setup() 中调用
relay_setup();
```

## 注意事项

- 本仓库所有驱动代码均免费且开源，可直接参考或集成到你的固件中。
- 如果某个驱动依赖了第三方库，请在使用时遵循该第三方库的许可协议，本项目不承担相关责任。
- 驱动代码中的产品 ID、密钥、引脚等示例信息仅供参考，正式上线前请替换为你自己的 ThingBoot 产品信息。

## License

Apache License 2.0
