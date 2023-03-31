// Commander example for TWELITE SPOT: Send 0x80 command to App_Twelite

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = 5;
const int PRG_PIN = 4;
const int LED_PIN = 18;

const uint8_t TWE_CHANNEL = 18;
const uint32_t TWE_APP_ID = 0x67720102;

const uint8_t TWE_TARGET_LID = 0x78;

AppTweliteCommand command;

void printHelp();

void setup()
{
    Serial.begin(115200);
    Serial.println("Commander example for TWELITE SPOT: App_Twelite");
    Serial2.begin(115200, SERIAL_8N1);
    Twelite.setup(Serial2, LED_PIN, RST_PIN, PRG_PIN);
    Twelite.begin(TWE_CHANNEL, TWE_APP_ID);
    command.u8DestinationLogicalId = TWE_TARGET_LID;
    Twelite.send(command);
    printHelp();
}

void loop()
{
    static uint8_t lidState = 0;
    static uint8_t pwmState[4] = {0, 0, 0, 0};
    Twelite.update();
    if (Serial.available()) {
        int c = Serial.read();
        switch (c) {
        case '1': { command.bDiState[0] = !(command.bDiState[0]);
            Serial.print("Set DI1 to "); Serial.println(command.bDiState[0] ? "Lo" : "Hi"); break;
        }
        case '2': {
            command.bDiState[1] = !(command.bDiState[1]);
            Serial.print("Set DI2 to "); Serial.println(command.bDiState[1] ? "Lo" : "Hi"); break;
        }
        case '3': {
            command.bDiState[2] = !(command.bDiState[2]);
            Serial.print("Set DI3 to "); Serial.println(command.bDiState[2] ? "Lo" : "Hi"); break;
        }
        case '4': {
            command.bDiState[3] = !(command.bDiState[3]);
            Serial.print("Set DI4 to "); Serial.println(command.bDiState[3] ? "Lo" : "Hi"); break;
        }
        case 'q': {
            command.bDiToChange[0] = !(command.bDiToChange[0]);
            Serial.print(command.bDiToChange[0] ? "Enabled" : "Disabled"); Serial.println(" DI1"); break;
        }
        case 'w': {
            command.bDiToChange[1] = !(command.bDiToChange[1]);
            Serial.print(command.bDiToChange[1] ? "Enabled" : "Disabled"); Serial.println(" DI2"); break;
        }
        case 'r': {
            command.bDiToChange[2] = !(command.bDiToChange[2]);
            Serial.print(command.bDiToChange[2] ? "Enabled" : "Disabled"); Serial.println(" DI3"); break;
        }
        case 't': {
            command.bDiToChange[3] = !(command.bDiToChange[3]);
            Serial.print(command.bDiToChange[3] ? "Enabled" : "Disabled"); Serial.println(" DI4"); break;
        }
        case 'a': {
            switch (pwmState[0]) {
            case 0: {
                command.u16PwmDuty[0] = 0; pwmState[0]++;
                Serial.println("Set PWM1 to 0%"); break;
            }
            case 1: {
                command.u16PwmDuty[0] = 1024 * 0.2; pwmState[0]++;
                Serial.println("Set PWM1 to 20%"); break;
            }
            case 2: {
                command.u16PwmDuty[0] = 1024 * 0.4; pwmState[0]++;
                Serial.println("Set PWM1 to 40%"); break;
            }
            case 3: {
                command.u16PwmDuty[0] = 1024 * 0.6; pwmState[0]++;
                Serial.println("Set PWM1 to 60%"); break;
            }
            case 4: {
                command.u16PwmDuty[0] = 1024 * 0.8; pwmState[0]++;
                Serial.println("Set PWM1 to 80%"); break;
            }
            case 5: {
                command.u16PwmDuty[0] = 1024; pwmState[0]++;
                Serial.println("Set PWM1 to 100%"); break;
            }
            case 6: {
                command.u16PwmDuty[0] = 0xFFFF; pwmState[0] = 0;
                Serial.println("Set PWM1 to N/A"); break;
            }
            default: break;
            } break;
        }
        case 's': {
            switch (pwmState[1]) {
            case 0: {
                command.u16PwmDuty[1] = 0; pwmState[1]++;
                Serial.println("Set PWM2 to 0%"); break;
            }
            case 1: {
                command.u16PwmDuty[1] = 1024 * 0.2; pwmState[1]++;
                Serial.println("Set PWM2 to 20%"); break;
            }
            case 2: {
                command.u16PwmDuty[1] = 1024 * 0.4; pwmState[1]++;
                Serial.println("Set PWM2 to 40%"); break;
            }
            case 3: {
                command.u16PwmDuty[1] = 1024 * 0.6; pwmState[1]++;
                Serial.println("Set PWM2 to 60%"); break;
            }
            case 4: {
                command.u16PwmDuty[1] = 1024 * 0.8; pwmState[1]++;
                Serial.println("Set PWM2 to 80%"); break;
            }
            case 5: {
                command.u16PwmDuty[1] = 1024 * 1.0; pwmState[1]++;
                Serial.println("Set PWM2 to 100%"); break;
            }
            case 6: {
                command.u16PwmDuty[1] = 0xFFFF; pwmState[1] = 0;
                Serial.println("Set PWM2 to N/A"); break;
            }
            default: break;
            } break;
        }
        case 'd': {
            switch (pwmState[2]) {
            case 0: {
                command.u16PwmDuty[2] = 0; pwmState[2]++;
                Serial.println("Set PWM3 to 0%"); break;
            }
            case 1: {
                command.u16PwmDuty[2] = 1024 * 0.2; pwmState[2]++;
                Serial.println("Set PWM3 to 20%"); break;
            }
            case 2: {
                command.u16PwmDuty[2] = 1024 * 0.4; pwmState[2]++;
                Serial.println("Set PWM3 to 40%"); break;
            }
            case 3: {
                command.u16PwmDuty[2] = 1024 * 0.6; pwmState[2]++;
                Serial.println("Set PWM3 to 60%"); break;
            }
            case 4: {
                command.u16PwmDuty[2] = 1024 * 0.8; pwmState[2]++;
                Serial.println("Set PWM3 to 80%"); break;
            }
            case 5: {
                command.u16PwmDuty[2] = 1024 * 1.0; pwmState[2]++;
                Serial.println("Set PWM3 to 100%"); break;
            }
            case 6: {
                command.u16PwmDuty[2] = 0xFFFF; pwmState[2] = 0;
                Serial.println("Set PWM3 to N/A"); break;
            }
            default: break;
            } break;
        }
        case 'f': {
            switch (pwmState[3]) {
            case 0: {
                command.u16PwmDuty[3] = 0; pwmState[3]++;
                Serial.println("Set PWM4 to 0%"); break;
            }
            case 1: {
                command.u16PwmDuty[3] = 1024 * 0.2; pwmState[3]++;
                Serial.println("Set PWM4 to 20%"); break;
            }
            case 2: {
                command.u16PwmDuty[3] = 1024 * 0.4; pwmState[3]++;
                Serial.println("Set PWM4 to 40%"); break;
            }
            case 3: {
                command.u16PwmDuty[3] = 1024 * 0.6; pwmState[3]++;
                Serial.println("Set PWM4 to 60%"); break;
            }
            case 4: {
                command.u16PwmDuty[3] = 1024 * 0.8; pwmState[3]++;
                Serial.println("Set PWM4 to 80%"); break;
            }
            case 5: {
                command.u16PwmDuty[3] = 1024 * 1.0; pwmState[3]++;
                Serial.println("Set PWM4 to 100%"); break;
            }
            case 6: {
                command.u16PwmDuty[3] = 0xFFFF; pwmState[3] = 0;
                Serial.println("Set PWM4 to N/A"); break;
            }
            default: break;
            } break;
        }
        case ' ': {
            Twelite.send(command);
            Serial.println("Sent command"); break;
        }
        case 'h': { printHelp(); break;}
        default: break;
        }
    }
}

void printHelp()
{
    Serial.println("");
    Serial.println("Keys:");
    Serial.println("DI1:  '1' DI2:  '2' DI3:  '3' DI4:  '4'");
    Serial.println("SEL1: 'q' SEL2: 'w' SEL3: 'r' SEL4: 't'");
    Serial.println("PWM1: 'a' PWM2: 's' PWM3: 'd' PWM4: 'f'");
    Serial.println("Send: SPACE");
    Serial.println("Help: 'h'");
    Serial.println("");
}

/*
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */
