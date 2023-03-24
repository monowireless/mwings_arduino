// Sample: Receive data from App_Twelite with TWELITE SPOT

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = 5;
const int PRG_PIN = 4;
const int LED_PIN = 18;

void setup()
{
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1);
    Twelite.setup(Serial2, LED_PIN, RST_PIN, PRG_PIN);
    Twelite.on([](const ParsedAppTwelitePacket& packet){
        if (not (packet.bPeriodic)) {
            Serial.print("Packet Timestamp:  ");
            Serial.print(packet.u16SequenceNumber / 64.0f, 1); Serial.println("sec");
            Serial.print("Source Logical ID: 0x");
            Serial.println(packet.u8SourceLogicalId, HEX);
            Serial.print("LQI:               ");
            Serial.println(packet.u8Lqi, DEC);
            Serial.print("Supply Voltage:    ");
            Serial.print(packet.u16SupplyVoltage, DEC); Serial.println("mV");
            Serial.print("Digital Input:     ");
            Serial.print(packet.bDIState[0] ? "DI1:Hi/" : "DI1:Lo/");
            Serial.print(packet.bDIState[1] ? "DI2:Hi/" : "DI2:Lo/");
            Serial.print(packet.bDIState[2] ? "DI3:Hi/" : "DI3:Lo/");
            Serial.println(packet.bDIState[3] ? "DI4:Hi" : "DI4:Lo");
            Serial.print("Analog Input:      ");
            Serial.print("AI1:"); Serial.print(packet.u16AIVoltage[0]); Serial.print("mV/");
            Serial.print("AI2:"); Serial.print(packet.u16AIVoltage[1]); Serial.print("mV/");
            Serial.print("AI3:"); Serial.print(packet.u16AIVoltage[2]); Serial.print("mV/");
            Serial.print("AI4:"); Serial.print(packet.u16AIVoltage[3]); Serial.println("mV");
            Serial.println("");
        }
    });
    Twelite.begin(18, 0x67720102);
}

void loop()
{
    Twelite.update();
}

/*
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */
