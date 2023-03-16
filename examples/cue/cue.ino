#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = 5;
const int PRG_PIN = 4;
const int LED_PIN = 18;

MWings twelite;

void printlnAccelEvent(const uint8_t event);

void setup()
{
    Serial.begin(115200);
    twelite.debugUsing(Serial);
    Serial2.begin(115200, SERIAL_8N1);
    twelite.setup(Serial2, RST_PIN, PRG_PIN, LED_PIN);
    twelite.on([](const ParsedCuePacket& packet){
            Serial.println("");
            Serial.print("Packet Number:     #");
            Serial.println(packet.u16SequenceNumber, DEC);
            Serial.print("Source Logical ID: 0x");
            Serial.println(packet.u8SourceLogicalId, HEX);
            Serial.print("LQI:               ");
            Serial.println(packet.u8Lqi, DEC);
            Serial.print("Supply Voltage:    ");
            Serial.print(packet.u16SupplyVoltage, DEC); Serial.println(" [mV]");
            Serial.print("Accel Event:       ");
            printlnAccelEvent(packet.u8AccelEvent);
            Serial.print("Accel X Axis [0]:  ");
            Serial.print(packet.i16SamplesX[0], DEC); Serial.println(" [mG]");
            Serial.print("Accel Y Axis [0]:  ");
            Serial.print(packet.i16SamplesY[0], DEC); Serial.println(" [mG]");
            Serial.print("Accel Z Axis [0]:  ");
            Serial.print(packet.i16SamplesZ[0], DEC); Serial.println(" [mG]");
            Serial.print("Magnet State:      0x");
            Serial.println(packet.u8MagnetState, HEX);
        });
    twelite.begin(18, 0x67720102);
}

void loop()
{
    twelite.update();
}

void printlnAccelEvent(const uint8_t event)
{
    switch (event) {
    case 0x01: {
        Serial.print("Dice (1)"); break;
    }
    case 0x02: {
        Serial.print("Dice (2)"); break;
    }
    case 0x03: {
        Serial.print("Dice (3)"); break;
    }
    case 0x04: {
        Serial.print("Dice (4)"); break;
    }
    case 0x05: {
        Serial.print("Dice (5)"); break;
    }
    case 0x06: {
        Serial.print("Dice (6)"); break;
    }
    case 0x08: {
        Serial.print("Shake"); break;
    }
    case 0x10: {
        Serial.print("Move"); break;
    }
    default:
        break;
    }
    Serial.println("");
}
