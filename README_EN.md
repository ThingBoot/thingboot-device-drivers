# ThingBoot Device Drivers

This repository contains ready-to-use device driver examples for the [ThingBoot Device SDK](https://github.com/ThingBoot/thingboot-device-sdk), aimed at developers who want to integrate ThingBoot into their own projects.

> Note: This repository only contains published driver code. If you want to write or contribute new drivers, please visit the driver development repository.

## Repository Contents

Each subdirectory is a standalone driver example:

- `relay/`: Relay / switch / socket / circuit-breaker driver example
- `sht3x/`: SHT3x series temperature & humidity sensor driver example

## Quick Start

1. Make sure your project already uses [thingboot-device-sdk](https://github.com/ThingBoot/thingboot-device-sdk).
2. Copy the whole driver directory (e.g. `relay/`) into your project's `src/` directory.
3. Modify the pin assignments, channel counts and other configurations in the driver code (usually in header files such as `include/relay.h`) according to your hardware.
4. Build and flash the firmware.

For example, to use the relay driver:

```cpp
#include "relay.h"

// call this in your setup()
relay_setup();
```

## Notes

- All driver code in this repository is free and open-source; you can reference or integrate it into your own firmware.
- If a driver depends on a third-party library, please comply with that library's license. This project assumes no liability for third-party dependencies.
- Product IDs, keys, pin numbers and other example values in the driver code are for reference only. Replace them with your own ThingBoot product information before production.

## License

Apache License 2.0
