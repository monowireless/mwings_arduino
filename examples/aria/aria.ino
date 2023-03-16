#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = 5;
const int PRG_PIN = 4;
const int LED_PIN = 18;

MWings twelite;

void setup()
{
    Serial.begin(115200);
    twelite.debugUsing(Serial);
    Serial2.begin(115200, SERIAL_8N1);
    twelite.setup(Serial2, RST_PIN, PRG_PIN, LED_PIN);
    twelite.on([](const ParsedAriaPacket& packet){
            Serial.println("");
            Serial.print("Packet Number:     #");
            Serial.println(packet.u16SequenceNumber, DEC);
            Serial.print("Source Logical ID: 0x");
            Serial.println(packet.u8SourceLogicalId, HEX);
            Serial.print("LQI:               ");
            Serial.println(packet.u8Lqi, DEC);
            Serial.print("Supply Voltage:    ");
            Serial.print(packet.u16SupplyVoltage, DEC); Serial.println(" [mV]");
            Serial.print("Air Temperature:   ");
            Serial.print(packet.i16Temp100x / 100.0f, 2); Serial.println(" [C]");
            Serial.print("Relative Humidity: ");
            Serial.print(packet.u16Humid100x / 100.0f, 2); Serial.println(" [%]");
            Serial.print("Magnet State:      0x");
            Serial.println(packet.u8MagnetState, HEX);
        });
    twelite.begin(18, 0x67720102);
}

void loop()
{
    twelite.update();
}
