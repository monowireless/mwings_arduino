/**
 * @file   MWings.cpp
 * @brief  Mono Wireless TWELITE Wings API for 32-bit Arduinos.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "MWings.h"

MWings Twelite;

MWings::~MWings()
{
    if (_buffer) {
        delete[] _buffer;
    }
}

bool MWings::setup(HardwareSerial& serial,
                   const int indicatorPin, const int resetPin, const int programPin,
                   const int bufferSize, const int timeout)
{
    _serial = &serial;
    _indicatorPin = indicatorPin;
    _resetPin = resetPin;
    _programPin = programPin;
    _isIndicatorOn = false;
    _indicatorTimestamp = UINT32_MAX;
    _indicatorDuration = 0;
    _buffer = new uint8_t[bufferSize];
    _bufferSize = bufferSize;
    _characterCount = 0;
    _checksum = 0;
    _timeout = timeout;
    _latestTimestamp = UINT32_MAX;

    _debugSerial = nullptr;

    //// AppTwelitePacketParser for App_Twelite
    _onAppTwelitePacket = nullptr;
    //// AppIoPacketParser for App_IO
    _onAppIoPacket = nullptr;
    //// AppAriaPacketParser for App_ARIA (ARIA mode)
    _onAppAriaPacket = nullptr;
    //// AppCuePacketParser for App_CUE (CUE mode)
    _onAppCuePacket = nullptr;
    //// AppPalOpenClosePacketParser for App_PAL (OPENCLOSE)
    _onAppPalOpenClosePacket = nullptr;
    //// AppPalAmbPacketParser for App_PAL (AMB)
    _onAppPalAmbPacket = nullptr;
    //// AppPalMotPacketParser for App_PAL (MOT)
    _onAppPalMotPacket = nullptr;

    if (not (_indicatorPin < 0)) {
        pinMode(_indicatorPin, OUTPUT);
        digitalWrite(_indicatorPin, HIGH);
    }

    bool resetOrNot = false;

    turnOnIndicator();
    if (_resetPin >= 0 and _programPin >= 0) {
        const int resetAttempts = 3;
        for(int i = 0; i < resetAttempts; i++) {
            pinMode(_resetPin, OUTPUT);
            pinMode(_programPin, OUTPUT);

            digitalWrite(_resetPin, LOW);
            delay(1);
            digitalWrite(_programPin, HIGH);
            delay(1);
            digitalWrite(_resetPin, HIGH);

            if (ensureReset(100)) {
                resetOrNot = true;
                break;
            }
        }
    }
    turnOffIndicator();

    return resetOrNot;
}

bool MWings::begin(const uint8_t channel, const uint32_t appId)
{
    bool commandsAvailable = false;

    for (int i = 0; i < 3; i++) {
        // Check if serial commands are available or not
        beginCommand();
        writeInAscii(static_cast<uint8_t>(MWings::Command::ACK));
        endCommand();
        if (isThereAck(100)) {
            commandsAvailable = true;
            break;
        }
    }

    if (commandsAvailable) {
        const int commandAttempts = 3;

        // Set application id
        for (int i = 0; i < commandAttempts; i++) {
            beginCommand();
            writeInAscii(static_cast<uint8_t>(MWings::Command::SET_PARAMETER));
            writeInAscii(static_cast<uint8_t>(MWings::Parameter::APP_ID));
            writeInAscii(appId);
            endCommand();
            if (ensureSetParameter(100)) {
                break;
            } else if (not ((i+1) < commandAttempts)) {
                return false;   // No remaining attempts
            }
        }
        debugPrint("Successfully set Application ID.");

        // Set channel
        const uint32_t channelMask = 0x00000000 | (1 << channel);
        for (int i = 0; i < commandAttempts; i++) {
            beginCommand();
            writeInAscii(static_cast<uint8_t>(MWings::Command::SET_PARAMETER));
            writeInAscii(static_cast<uint8_t>(MWings::Parameter::CH_MASK));
            writeInAscii(channelMask);
            endCommand();
            if (ensureSetParameter(100)) {
                break;
            } else if (not ((i+1) < commandAttempts)) {
                return false;   // No remaining attempts
            }
        }
        debugPrint("Successfully set Channel.");

        // Save and reset
        for (int i = 0; i < commandAttempts; i++) {
            beginCommand();
            writeInAscii(static_cast<uint8_t>(MWings::Command::SAVE_RESET));
            endCommand();
            if (ensureReset(100)) {
                break;
            } else if (not ((i+1) < commandAttempts)) {
                return false;   // No remaining attempts
            }
        }
        debugPrint("Successfully saved parameters.");

        // Disable silent mode
        for (int i = 0; i < commandAttempts; i++) {
            beginCommand();
            writeInAscii(static_cast<uint8_t>(MWings::Command::CONTROL));
            writeInAscii(static_cast<uint8_t>(0x10));
            endCommand();
            if (ensureControlled(100)) {
                break;
            } else if (not ((i+1) < commandAttempts)) {
                return false;   // No remaining attempts
            }
        }
        debugPrint("Successfully started receiving packets.");

        flushSerialRxBuffer();

        return true;
    }

    debugPrint("Serial commands are not available or an unknown error occurred.");
    return false;
}

void MWings::update()
{
    // Abort if the serial is not initialized
    if (not _serial) { return; }

    // Reference to the serial
    HardwareSerial& serial = *_serial;

    // Process all byte in the buffer
    while (serial.available()) {
        // Read a character
        const int character = serial.read();

        debugWrite(character);

        // Abort if the read byte is invalid
        if (not (character >= 0)) { return; }

        // Bare packet storage
        mwings_common::BarePacket barePacket;

        // Process packet contents upon parsing completion
        if (processAscii(character, barePacket) == MWings::State::COMPLETED) {
            turnOnIndicatorFor(10);

            //// Start: AppTwelitePacketParser for App_Twelite
            if (AppTwelitePacketParser.isValid(barePacket) and _onAppTwelitePacket) {
                ParsedAppTwelitePacket parsedAppTwelitePacket;
                if (AppTwelitePacketParser.parse(barePacket, &parsedAppTwelitePacket)) {
                    _onAppTwelitePacket(parsedAppTwelitePacket);
                }
            }
            //// End: AppTwelitePacketParser for App_Twelite

            //// Start: AppIoPacketParser for App_IO
            if (AppIoPacketParser.isValid(barePacket) and _onAppIoPacket) {
                ParsedAppIoPacket parsedAppIoPacket;
                if (AppIoPacketParser.parse(barePacket, &parsedAppIoPacket)) {
                    _onAppIoPacket(parsedAppIoPacket);
                }
            }
            //// End: AppIoPacketParser for App_IO

            //// Start: AppAriaPacketParser for App_ARIA (ARIA mode)
            if (AppAriaPacketParser.isValid(barePacket) and _onAppAriaPacket) {
                ParsedAppAriaPacket parsedAppAriaPacket;
                if (AppAriaPacketParser.parse(barePacket, &parsedAppAriaPacket)) {
                    _onAppAriaPacket(parsedAppAriaPacket);
                }
            }
            //// End: AppAriaPacketParser for App_ARIA (ARIA mode)

            //// Start: AppCuePacketParser for App_CUE (CUE mode)
            if (AppCuePacketParser.isValid(barePacket) and _onAppCuePacket) {
                ParsedAppCuePacket parsedAppCuePacket;
                if (AppCuePacketParser.parse(barePacket, &parsedAppCuePacket)) {
                    _onAppCuePacket(parsedAppCuePacket);
                }
            }
            //// End: AppCuePacketParser for App_CUE (CUE mode)

            //// Start: AppPalOpenClosePacketParser for App_PAL (OPENCLOSE)
            if (AppPalOpenClosePacketParser.isValid(barePacket) and _onAppPalOpenClosePacket) {
                ParsedAppPalOpenClosePacket parsedAppPalOpenClosePacket;
                if (AppPalOpenClosePacketParser.parse(barePacket, &parsedAppPalOpenClosePacket)) {
                    _onAppPalOpenClosePacket(parsedAppPalOpenClosePacket);
                }
            }
            //// End: AppPalAmbPacketParser for App_PAL (AMB)

            //// Start: AppPalAmbPacketParser for App_PAL (AMB)
            if (AppPalAmbPacketParser.isValid(barePacket) and _onAppPalAmbPacket) {
                ParsedAppPalAmbPacket parsedAppPalAmbPacket;
                if (AppPalAmbPacketParser.parse(barePacket, &parsedAppPalAmbPacket)) {
                    _onAppPalAmbPacket(parsedAppPalAmbPacket);
                }
            }
            //// End: AppPalAmbPacketParser for App_PAL (AMB)

            //// Start: AppPalMotPacketParser for App_PAL (MOT)
            if (AppPalMotPacketParser.isValid(barePacket) and _onAppPalMotPacket) {
                ParsedAppPalMotPacket parsedAppPalMotPacket;
                if (AppPalMotPacketParser.parse(barePacket, &parsedAppPalMotPacket)) {
                    _onAppPalMotPacket(parsedAppPalMotPacket);
                }
            }
            //// End: AppPalMotPacketParser for App_PAL (MOT)
        }
    }

    updateIndicator();
}


MWings::State MWings::processAscii(const uint8_t character, mwings_common::BarePacket& barePacket)
{
    static MWings::State state = MWings::State::WAITING_FOR_HEADER;

    // Reset on timeout
    if (_timeout > 0 and state not_eq MWings::State::WAITING_FOR_HEADER) {
        if (millis() - _latestTimestamp > _timeout) {
            state = MWings::State::TIMEOUT_ERROR;
        }
    }

    // Reset if the state is error or completed
    if (state == MWings::State::COMPLETED
        or state == MWings::State::UNKNOWN_ERROR
        or state == MWings::State::CHECKSUM_ERROR
        or state == MWings::State::TIMEOUT_ERROR) {
        state = MWings::State::WAITING_FOR_HEADER;
    }

    // Run state machine
    switch (state) {
    case MWings::State::WAITING_FOR_HEADER: {
        // If the character is colon, start to read
        if (character == ':') {
            state = MWings::State::RETRIEVING_PAYLOAD;
            _latestTimestamp = millis();
            _characterCount = 0;
            _checksum = 0;
        }
        break;
    }
    case MWings::State::RETRIEVING_PAYLOAD: {
        if ((character >= '0' and character <= '9')
            or (character >= 'A' and character <= 'F')) {
            // Valid hex character

            // Abort if the buffer is overflowing
            if (byteCountFrom(_characterCount) >= _bufferSize) {
                state = MWings::State::UNKNOWN_ERROR;
                debugPrint("OVERFLOW ERROR");
                break;
            }

            // Convert character to hex
            const uint8_t hexValue = hexFrom(character);

            // Get a pointer for the new byte
            uint8_t* const newByte = &_buffer[byteCountFrom(_characterCount)];

            // Add byte
            if (_characterCount++ & 1) {
                // Odd: set 0-3 bit of the new byte
                *newByte = (*newByte & 0xF0) | hexValue;
                _checksum += *newByte;
            } else {
                // Even: set 7-4 bit of the new byte
                *newByte = hexValue << 4;
            }
        } else if (character == '\r') {
            // CR

            // Abort if received data are not valid
            if (not (_characterCount >= 4 and (_characterCount & 1) == 0)) {
                state = MWings::State::UNKNOWN_ERROR;
                debugPrint("LENGTH ERROR");
                break;
            }

            // Mask checksum
            _checksum = _checksum & 0xFF;

            // Abort if the checksum is not valid
            if (not (_checksum == 0)) {
                state = MWings::State::CHECKSUM_ERROR;
                debugPrint("CHECKSUM ERROR");
                break;
            }

            state = MWings::State::WAITING_FOR_FOOTER;
        } else {
            // Unknown characters
            state = MWings::State::UNKNOWN_ERROR;
            debugPrint("UNKNOWN CHAR ERROR");
        }
        break;
    }
    case MWings::State::WAITING_FOR_FOOTER: {
        if (character == '\n') {
            // Completed
            state = MWings::State::COMPLETED;
            //debugPrint("COMPLETED!");
        } else {
            // CR only
            state = MWings::State::UNKNOWN_ERROR;
            debugPrint("NO LF ERROR");
        }
        break;
    }
    default:
        state = MWings::State::UNKNOWN_ERROR;
        debugPrint("UNKNOWN ERROR");
        break;
    }

    // Make bare packet available when parsing was completed
    if (state == MWings::State::COMPLETED) {
        barePacket.size = byteCountFrom(_characterCount);
        barePacket.payload = _buffer;
    }

    return state;
}
