// Monitor example for TWELITE with Arduino UNO R4: Receive data from act

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = D11;
const int PRG_PIN = D12;
const int LED_PIN = D13;        // Use on-board LED as indicator

const uint8_t TWE_CHANNEL = 13;
const uint32_t TWE_APP_ID = 0x1234ABCD;

void printAsChar(const uint8_t data);
void printByte(const uint8_t data);

void setup()
{
    // Initialize serial ports
    while (!Serial && millis() < 5000); // Wait for internal USB-UART
    Serial.begin(115200);
    Serial.println("Monitor example for TWELITE with Arduino UNO R4: App_Twelite");
    Serial1.begin(115200);

    // Initialize TWELITE
    if (Twelite.begin(Serial1,
                      LED_PIN, RST_PIN, PRG_PIN,
                      TWE_CHANNEL, TWE_APP_ID)) {
        Serial.println("Successfully initialized TWELITE");
    } else {
        Serial.println("Failed to initialize TWELITE");
    }

    // Attach an event handler to process packets from any TWELITE
    Twelite.on([](const ParsedActPacket& packet) {
        // Print packet
        Serial.print("[LID:0x");
        Serial.print(packet.u8SourceLogicalId, HEX);
        Serial.print(",LQI:");
        Serial.print(packet.u8Lqi, DEC);
        Serial.print(",COM:0x");
        Serial.print(packet.u8CommandId, HEX);
        Serial.print(",4CH:");
        for (int i = 0; i < 4; i++) {
            printAsChar(packet.u8Data[i]); // First 4 bytes as ASCII
        }
        Serial.print("] ");
        for (int i = 0; i < packet.u16DataSize; i++) {
            printByte(packet.u8Data[i]);
        }
        Serial.println("");
    });
}

void loop()
{
    // Update TWELITE
    Twelite.update();
}


void printAsChar(const uint8_t data)
{
    Serial.write((0x20 <= data and data < 0x7F) ? data : '.');
}

void printByte(const uint8_t data)
{
    const uint8_t bit74 = ((data >> 4) & 0xF);
    const uint8_t bit30 = ((data >> 0) & 0xF);
    Serial.write((bit74 < 0xA) ? ('0' + bit74) : ('A' + bit74 - 0xA));
    Serial.write((bit30 < 0xA) ? ('0' + bit30) : ('A' + bit30 - 0xA));
}

/*
 * Copyright (C) 2024 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */
