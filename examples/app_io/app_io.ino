// Sample: Receive data from App_IO with TWELITE SPOT

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = 5;
const int PRG_PIN = 4;
const int LED_PIN = 18;

const int DIO_FROM_DI[] = {12, 13, 11, 16, 18, 19, 4, 9, 14, 15, 5, 8};

void setup()
{
    Serial.begin(115200);
    Serial.println("AppIoParser example");
    Serial2.begin(115200, SERIAL_8N1);
    Twelite.setup(Serial2, LED_PIN, RST_PIN, PRG_PIN);
    Twelite.on([](const ParsedAppIoPacket& packet) {
            Serial.println("");
            Serial.print("Packet Timestamp:  ");
            Serial.print(packet.u16SequenceNumber / 64.0f, 1); Serial.println("sec");
            Serial.print("Source Logical ID: 0x");
            Serial.println(packet.u8SourceLogicalId, HEX);
            Serial.print("LQI:               ");
            Serial.println(packet.u8Lqi, DEC);
            Serial.print("Digital Input:    ");
            for (int i = 0; i < 12; i++) {
                if (packet.bDiValid[i]) {
                    Serial.print(" DIO"); Serial.print(DIO_FROM_DI[i], DEC);
                    Serial.print(packet.bDiState[i] ? ":Lo" : ":Hi");
                }
            }
        });
    Twelite.begin(16, 0x67720107);
}

void loop()
{
    Twelite.update();
}

/*
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */