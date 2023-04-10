# MWings

**Mono Wireless TWELITE Wings API for 32-bit Arduinos**

## About

MWings is a library that communicate with TWELITE wireless modules.

This library is suitable for TWELITE SPOT series.

You can receive packets from and send commands to TWELITE child devices through the connected TWELITE parent device.

## Requirements

Built for 32-bit Arduino boards (esp32, esp8266, stm32, sam, rp2040). Tested on ESP32 series.

**8-bit Arduino boards (like UNO R3) are NOT supported.**

## Features

### Receive and Parse Packets from

- **App_Twelite**
  - Get digital and analog input states.
  - Pre-installed on TWELITE (SMD) and TWELITE DIP series.
  - Example:
    - `monitor_spot_app_twelite.ino`
- **App_ARIA (TWELITE ARIA mode)**
  - Get air temperature, relative humidity and magnet state.
  - Pre-installed on TWELITE ARIA series.
  - Examples:
    - `monitor_spot_app_aria.ino`
    - `plotter_spot_app_aria.ino`
- **App_CUE (TWELITE CUE mode)**
  - Get acceleration and magnet state.
  - Pre-installed on TWELITE CUE series.
  - Example:
    - `monitor_spot_app_cue.ino`
- **App_PAL (AMBIENT)**
  - Get air temperature, relative humidity and illuminance.
  - Pre-installed on TWELITE PAL series (App_PAL)
  - Examples:
    - `monitor_spot_app_pal_amb.ino`
    - `plotter_spot_app_pal_amb.ino`
- **App_PAL (MOTION)** / **App_CUE (MOTION PAL mode)**
  - Get acceleration.
  - Pre-installed on TWELITE PAL series (App_PAL) and TWELITE CUE series (App_CUE).
  - Examples:
    - `monitor_spot_app_pal_mot.ino`
    - `plotter_spot_app_pal_mot.ino`
- **App_PAL (OPENCLOSE)** / **App_CUE (OPENCLOSE PAL mode)** / **App_ARIA (OPENCLOSE PAL mode)**
  - Get magnet state.
  - Pre-installed on TWELITE PAL series (App_PAL), TWELITE ARIA series (App_ARIA) and TWELITE CUE series (App_CUE).
  - Example:
    - `monitor_spot_app_pal_openclose.ino`
- **App_Uart (Mode A, Simplified or Extended ASCII format)**
  - Get serial data received on TWELITE child devices.
  - Pre-installed on TWELITE UART series.
  - Example:
    - `monitor_spot_app_uart_ascii.ino`
- **App_IO**
  - Get digital input states.
  - Example:
    - `monitor_spot_app_io.ino`

### Receive Packets from

- Any TWELITE
  - Get binary payload from ASCII-formatted serial input (starts with `:` and ends with `CRLF`).
  - Example:
    - `monitor_spot_any.ino`

### Serialize and Send Commands to

- **App_Twelite**
  - Set digital output states and PWM duty ratios.
  - Pre-installed on TWELITE (SMD) and TWELITE DIP series.
  - Example:
    - `commander_spot_app_twelite.ino`
- **App_PAL (NOTICE)**
  - Set full-color LED states.
  - Pre-installed on TWELITE PAL series (App_PAL)
  - Examples:
    - `commander_spot_app_pal_notice.ino`
    - `commander_spot_app_pal_notice_detailed.ino`
    - `commander_spot_app_pal_notice_event.ino`

### Send Commands to

- Any TWELITE
  - Set some binary data with ASCII-formatted serial output (starts with `:` and ends with `CRLF`).
  - Example: `commander_spot_app_twelite_manually.ino`

## Getting Started

Here's an example using TWELITE SPOT with [TWELITE ARIA](https://mono-wireless.com/jp/products/twelite-aria/index.html).

``` c++
// monitor_spot_app_aria.ino (Excerpt)

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = 5;
const int PRG_PIN = 4;
const int LED_PIN = 18;

const uint8_t TWE_CHANNEL = 18;
const uint32_t TWE_APP_ID = 0x67720102;

void printMagnetState(const uint8_t state, const bool changed);

void setup()
{
    // Initialize serial ports
    Serial.begin(115200);
    Serial.println("Monitor example for TWELITE SPOT: App_ARIA (ARIA Mode)");
    Serial2.begin(115200, SERIAL_8N1);

    // Initialize TWELITE
    Twelite.begin(Serial2,
                  TWE_CHANNEL, TWE_APP_ID,
                  LED_PIN, RST_PIN, PRG_PIN);

    // Attach an event handler to process packets from App_ARIA
    Twelite.on([](const ParsedAppAriaPacket& packet) {
        Serial.println("");
        Serial.print("Packet Number:     #");
        Serial.println(packet.u16SequenceNumber, DEC);
        Serial.print("Source Logical ID: 0x");
        Serial.println(packet.u8SourceLogicalId, HEX);
        Serial.print("LQI:               ");
        Serial.println(packet.u8Lqi, DEC);
        Serial.print("Supply Voltage:    ");
        Serial.print(packet.u16SupplyVoltage, DEC); Serial.println(" mV");
        Serial.print("Air Temperature:   ");
        Serial.print(packet.i16Temp100x / 100.0f, 2); Serial.println(" C");
        Serial.print("Relative Humidity: ");
        Serial.print(packet.u16Humid100x / 100.0f, 2); Serial.println(" %");
        Serial.print("Magnet State:      ");
        printMagnetState(packet.u8MagnetState, packet.bMagnetStateChanged);
    });
}

void loop()
{
    // Update TWELITE
    Twelite.update();
}
```

### Serial ports setup

You need to initialize a [HardwareSerial](https://www.arduino.cc/reference/en/language/functions/communication/serial/) instance to use before.

``` c++
    // Initialize serial ports
    Serial.begin(115200);
    Serial.println("Monitor example for TWELITE SPOT: App_ARIA (ARIA Mode)");
    Serial2.begin(115200, SERIAL_8N1);
```

In the above code, use `Serial` for monitoring and `Serial2` for communication with TWELITE.

### TWELITE setup

To initialize TWELITE, you need to pass a `HardwareSerial` instance for communication with the parent device over UART.

You can enable the status LED or set the channel and application ID of the connected TWELITE.

``` c++
    // Initialize TWELITE
    Twelite.begin(Serial2,
                  TWE_CHANNEL, TWE_APP_ID,
                  LED_PIN, RST_PIN, PRG_PIN);
```

But also the below code is valid (Run with the default channel and application id).

``` c++
    // Initialize TWELITE
    Twelite.begin(Serial2);
```

### TWELITE event handler

To receive data, you need to create an event handler.

You can easily create handlers using a non-capturing lambda-function like below.

``` c++
    // Attach an event handler to process packets from App_ARIA
    Twelite.on([](const ParsedAppAriaPacket& packet) {
        Serial.println("");
        Serial.print("Packet Number:     #");
        Serial.println(packet.u16SequenceNumber, DEC);
        Serial.print("Source Logical ID: 0x");
        Serial.println(packet.u8SourceLogicalId, HEX);
        Serial.print("LQI:               ");
        Serial.println(packet.u8Lqi, DEC);
        Serial.print("Supply Voltage:    ");
        Serial.print(packet.u16SupplyVoltage, DEC); Serial.println(" mV");
        Serial.print("Air Temperature:   ");
        Serial.print(packet.i16Temp100x / 100.0f, 2); Serial.println(" C");
        Serial.print("Relative Humidity: ");
        Serial.print(packet.u16Humid100x / 100.0f, 2); Serial.println(" %");
        Serial.print("Magnet State:      ");
        printMagnetState(packet.u8MagnetState, packet.bMagnetStateChanged);
    });
```

For App_ARIA, data type called `ParsedAppAriaPacket` is available for handlers.

### TWELTIE update

You need to update states in the `loop()`.

``` c++
    // Update TWELITE
    Twelite.update();
```

Do NOT block this function for a long time with `delay()` or something.

## License

``` plain
Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
```
