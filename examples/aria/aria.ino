// Sample: Receive data from TWELITE ARIA (TWELITE ARIA Mode) with TWELITE SPOT

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
    Twelite.on([](const ParsedAriaPacket& packet){
            Serial.print("Packet Number:     #");
            Serial.println(packet.u16SequenceNumber, DEC);
            Serial.print("Source Logical ID: 0x");
            Serial.println(packet.u8SourceLogicalId, HEX);
            Serial.print("LQI:               ");
            Serial.println(packet.u8Lqi, DEC);
            Serial.print("Supply Voltage:    ");
            Serial.print(packet.u16SupplyVoltage, DEC); Serial.println("mV");
            Serial.print("Air Temperature:   ");
            Serial.print(packet.i16Temp100x / 100.0f, 2); Serial.println("C");
            Serial.print("Relative Humidity: ");
            Serial.print(packet.u16Humid100x / 100.0f, 2); Serial.println("%");
            Serial.print("Magnet State:      0x");
            Serial.println(packet.u8MagnetState, HEX);
            Serial.println("");
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
