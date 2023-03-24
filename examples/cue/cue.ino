// Receive data from TWELITE CUE (TWELITE CUE Mode) with TWELITE SPOT

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = 5;
const int PRG_PIN = 4;
const int LED_PIN = 18;

void printlnAccelEvent(const uint8_t event);

void setup()
{
    Serial.begin(115200);
    Serial2.begin(115200);
    Twelite.setup(Serial2, LED_PIN, RST_PIN, PRG_PIN);
    Twelite.on([](const ParsedCuePacket& packet){
            Serial.print("Packet Number:     #");
            Serial.println(packet.u16SequenceNumber, DEC);
            Serial.print("Source Logical ID: 0x");
            Serial.println(packet.u8SourceLogicalId, HEX);
            Serial.print("LQI:               ");
            Serial.println(packet.u8Lqi, DEC);
            Serial.print("Supply Voltage:    ");
            Serial.print(packet.u16SupplyVoltage, DEC); Serial.println("mV");
            Serial.print("Accel Event:       ");
            printlnAccelEvent(packet.u8AccelEvent);
            Serial.print("Accel X Axis [0]:  ");
            Serial.print(packet.i16SamplesX[0], DEC); Serial.println("mG");
            Serial.print("Accel Y Axis [0]:  ");
            Serial.print(packet.i16SamplesY[0], DEC); Serial.println("mG");
            Serial.print("Accel Z Axis [0]:  ");
            Serial.print(packet.i16SamplesZ[0], DEC); Serial.println("mG");
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

void printlnAccelEvent(const uint8_t event)
{
    switch (event) {
    case 0x01: { Serial.print("Dice (1)"); break; }
    case 0x02: { Serial.print("Dice (2)"); break; }
    case 0x03: { Serial.print("Dice (3)"); break; }
    case 0x04: { Serial.print("Dice (4)"); break; }
    case 0x05: { Serial.print("Dice (5)"); break; }
    case 0x06: { Serial.print("Dice (6)"); break; }
    case 0x08: { Serial.print("Shake"); break; }
    case 0x10: { Serial.print("Move"); break; }
    default: break;
    }
    Serial.println("");
}

/*
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */
