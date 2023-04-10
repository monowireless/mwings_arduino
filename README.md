# MWings

## About
MWings is a library that communicate with TWELITE wireless modules.

This library is suitable for TWELITE SPOT series.

You can receive packets from and send commands to TWELITE child devices through the connected TWELITE parent device.

## Requirements

Built for 32-bit Arduino boards (esp32, esp8266, stm32, sam, rp2040). Tested on ESP32 / ESP8266 series.

**8-bit AVR Arduino boards (like UNO R3) are NOT supported.**

## Features

### Receive and Parse Packets from

- App_Twelite
  - Get digital input states and analog input voltage values.
  - Pre-installed on TWELITE (SMD) and TWELITE DIP series.
- App_ARIA (TWELITE ARIA mode)
  - Get air temperature, relative humidity and magnet states.
  - Pre-installed on TWELITE ARIA series.
- App_CUE (TWELITE CUE mode)
  - Get acceleration and magnet states.
  - Pre-installed on TWELITE CUE series.
- App_PAL (AMBIENT)
  - Get air temperature, relative humidity and illuminance.
  - Pre-installed on TWELITE PAL series (App_PAL)
- App_PAL (MOTION) / App_CUE (MOTION PAL mode)
  - Get acceleration.
  - Pre-installed on TWELITE PAL series (App_PAL) and TWELITE CUE series (App_CUE).
- App_PAL (OPENCLOSE) / App_CUE (OPENCLOSE PAL mode) / App_ARIA (OPENCLOSE PAL mode)
  - Get magnet states.
  - Pre-installed on TWELITE PAL series (App_PAL), TWELITE ARIA series (App_ARIA) and TWELITE CUE series (App_CUE).
- App_Uart (Mode A, Simplified or Extended format)
  - Get serial data received on TWELITE child devices.
  - Pre-installed on TWELITE UART series.
- App_IO
  - Get digital input states

### Receive Packets from

- Any TWELITE
  - Get payload from ASCII-formatted serial input (starts with ':' and ends with CRLF).

### Serialize and Send Commands to

- App_Twelite
  - Set digital output states and PWM duty ratios.
  - Pre-installed on TWELITE (SMD) and TWELITE DIP series.
- App_PAL (NOTICE)
  - Set full-color LED states.
  - Pre-installed on TWELITE PAL series (App_PAL)

### Send Commands to

- Any TWELITE
  - Set some data with ASCII-formatted serial output (starts with ':' and ends with CRLF).

## License

```
Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
```
