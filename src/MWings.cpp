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

    //// AriaParser for App_ARIA (ARIA mode)
    _onAriaPacket = nullptr;
    //// CueParser for App_CUE (CUE mode)
    _onCuePacket = nullptr;
    //// PalAmbParser for App_PAL (AMB)
    _onPalAmbPacket = nullptr;
    //// PalMotParser for App_PAL (MOT)
    _onPalMotPacket = nullptr;

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

            //// Start: AriaParser for App_ARIA (ARIA mode)
            if (AriaParser.isValid(barePacket) and _onAriaPacket) {
                ParsedAriaPacket parsedAriaPacket;
                if (AriaParser.parse(barePacket, &parsedAriaPacket)) {
                    _onAriaPacket(parsedAriaPacket);
                }
            }
            //// End: AriaParser for App_ARIA (ARIA mode)

            //// Start: CueParser for App_CUE (CUE mode)
            if (CueParser.isValid(barePacket) and _onCuePacket) {
                ParsedCuePacket parsedCuePacket;
                if (CueParser.parse(barePacket, &parsedCuePacket)) {
                    _onCuePacket(parsedCuePacket);
                }
            }
            //// End: CueParser for App_CUE (CUE mode)

            //// Start: PalAmbParser for App_PAL (AMB)
            if (PalAmbParser.isValid(barePacket) and _onPalAmbPacket) {
                ParsedPalAmbPacket parsedPalAmbPacket;
                if (PalAmbParser.parse(barePacket, &parsedPalAmbPacket)) {
                    _onPalAmbPacket(parsedPalAmbPacket);
                }
            }
            //// End: PalAmbParser for App_PAL (AMB)

            //// Start: PalMotParser for App_PAL (MOT)
            if (PalMotParser.isValid(barePacket) and _onPalMotPacket) {
                ParsedPalMotPacket parsedPalMotPacket;
                if (PalMotParser.parse(barePacket, &parsedPalMotPacket)) {
                    _onPalMotPacket(parsedPalMotPacket);
                }
            }
            //// End: PalMotParser for App_PAL (MOT)
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
        barePacket.size = byteCountFrom(_characterCount) - 1;
        barePacket.payload = _buffer;
        const uint8_t lrc = _buffer[byteCountFrom(_characterCount) - 1];
        barePacket.checksum = (~(_checksum - lrc) + 1) & 0xFF;
    }

    return state;
}
