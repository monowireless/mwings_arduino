// Arduino plotter example for TWELITE SPOT: Receive data from App_PAL (AMBIENT)

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = 5;
const int PRG_PIN = 4;
const int LED_PIN = 18;

const int8_t RX1_PIN = 16;
const int8_t TX1_PIN = 17;

const uint8_t TWE_CHANNEL = 15;
const uint32_t TWE_APP_ID = 0x67726305;

void setup()
{
    // Initialize serial ports
    Serial.begin(115200);
    Serial.println("Plotter example for TWELITE SPOT: App_PAL (AMBIENT)");
    Serial2.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);

    // Initialize TWELITE
    Twelite.begin(Serial2,
                  LED_PIN, RST_PIN, PRG_PIN,
                  TWE_CHANNEL, TWE_APP_ID);

    // Attach an event handler to process packets from App_PAL (AMB)
    Twelite.on([](const ParsedAppPalAmbPacket& packet) {
        // Plot only the first device
        static uint32_t firstSourceSerialId = packet.u32SourceSerialId;
        if (not (packet.u32SourceSerialId == firstSourceSerialId)) { return; }
        // Print
        Serial.print("Temp(x100):");
        Serial.print(packet.i16Temp100x, DEC); Serial.print(",");
        Serial.print("Humid(x100):");
        Serial.print(packet.u16Humid100x, DEC); Serial.print(",");
        Serial.print("VCC:");
        Serial.print(packet.u16SupplyVoltage, DEC); Serial.print(",");
        Serial.print("LQI(x10):");
        Serial.print(packet.u8Lqi * 10, DEC); Serial.println("");
    });
}

void loop()
{
    // Update TWELITE
    Twelite.update();
}

/*
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */
