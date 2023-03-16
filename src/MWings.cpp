/**
 * @file   MWings.cpp
 * @brief  Mono Wireless TWELITE Wings API for Arduino on ESP32.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "MWings.h"

MWings::~MWings()
{
    if (_buffer) {
        delete[] _buffer;
    }
}

void MWings::setup(HardwareSerial& serial,
                   const int resetPin, const int programPin, const int indicatorPin,
                   const int bufferSize)
{
    _serial = &serial;
    _resetPin = resetPin;
    _programPin = programPin;
    _indicatorPin = indicatorPin;
    _isIndicatorOn = false;
    _indicatorTimestamp = UINT32_MAX;
    _indicatorDuration = 0;
    _buffer = new uint8_t[bufferSize];
    _bufferSize = bufferSize;
    _characterCount = 0;
    _checksum = 0;
    _timeout = 0;
    _latestTimestamp = UINT32_MAX;

    if (not (_resetPin < 0 or _programPin < 0)) {
        pinMode(_resetPin, OUTPUT);
        pinMode(_programPin, OUTPUT);

        digitalWrite(_resetPin, LOW);
        delay(1);
        digitalWrite(_programPin, HIGH);
        delay(1);
        digitalWrite(_resetPin, HIGH);
    }

    if (not (_indicatorPin < 0)) {
        pinMode(_indicatorPin, OUTPUT);
        digitalWrite(_indicatorPin, HIGH);
    }

    turnOnIndicator();
    ensureStarted(100);
    turnOffIndicator();
}

bool MWings::begin(const uint8_t channel, const uint32_t appId)
{
    turnOnIndicator();

    // Set application id
    beginCommand();
    writeInAscii(static_cast<uint8_t>(MWings::Command::SET_PARAMETER));
    writeInAscii(static_cast<uint8_t>(MWings::Parameter::APP_ID));
    writeInAscii(appId);
    endCommand();

    if (not ensureSetParameter(100)) { return false; }
    debugPrint("Successfully set the AppID.");

    // Set channel
    const uint32_t channelMask = 0x00000000 | (1 << channel);

    beginCommand();
    writeInAscii(static_cast<uint8_t>(MWings::Command::SET_PARAMETER));
    writeInAscii(static_cast<uint8_t>(MWings::Parameter::CH_MASK));
    writeInAscii(channelMask);
    endCommand();

    if (not ensureSetParameter(100)) { return false; }
    debugPrint("Successfully set the Channel.");

    // Save and reset
    beginCommand();
    writeInAscii(static_cast<uint8_t>(MWings::Command::SAVE_RESET));
    endCommand();

    if (not ensureStarted(100)) { return false; }
    debugPrint("Successfully saved parameters.");

    // Disable silent mode
    beginCommand();
    writeInAscii(static_cast<uint8_t>(MWings::Command::CONTROL));
    writeInAscii(static_cast<uint8_t>(0x10));
    endCommand();

    if (not ensureControlled(100)) { return false; }
    debugPrint("Successfully started receiving packets.");

    turnOffIndicator();
    return true;
}

void MWings::update()
{
    // Abort if the serial is not initialized
    if (not _serial) { return; }

    // Reference to the serial
    HardwareSerial& serial = *_serial;

    // Process a byte if available
    if (serial.available()) {
        // Read a character
        const int character = serial.read();

        // Abort if the read byte is invalid
        if (not (character >= 0)) { return; }

        debugWrite(character);

        // Bare packet storage
        mwings_common::BarePacket barePacket;

        // Process packet contents upon parsing completion
        if (processAscii(character, barePacket) == MWings::State::COMPLETED) {
            turnOnIndicatorFor(50);
            debugPrint("Successfully received the packet (above).");

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
    static MWings::State state = MWings::State::WAITING_FOR_COLON;

    // Reset on timeout
    if (_timeout > 0 and state not_eq MWings::State::WAITING_FOR_COLON) {
        if (millis() - _latestTimestamp > _timeout) {
            state = MWings::State::WAITING_FOR_COLON;
        }
    }

    // Reset if the state is error or completed
    if (state == MWings::State::COMPLETED
        or state == MWings::State::UNKNOWN_ERROR
        or state == MWings::State::CHECKSUM_ERROR) {
        state = MWings::State::WAITING_FOR_COLON;
    }

    // Run state machine
    switch (state) {
    case MWings::State::WAITING_FOR_COLON: {
        // If the character is colon, start to read
        if (character == ':') {
            state = MWings::State::READING_PAYLOAD;
            _latestTimestamp = millis();
            _characterCount = 0;
        }
        break;
    }
    case MWings::State::READING_PAYLOAD: {
        if ((character >= '0' and character <= '9')
            or (character >= 'A' and character <= 'F')) {
            // Hex character

            // Abort if the buffer is overflowing
            if (byteCountFrom(_characterCount) >= _bufferSize) {
                state = MWings::State::UNKNOWN_ERROR;
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
        } else if (character == '\r' or character == '\n') {
            // CR or LF

            // Abort if received data are not valid
            if (not (_characterCount >= 4 and (_characterCount & 1) == 0)) {
                state = MWings::State::UNKNOWN_ERROR;
                break;
            }

            // Mask checksum
            _checksum = _checksum & 0xFF;

            // Abort if the checksum is not valid
            if (not (_checksum == 0)) {
                state = MWings::State::CHECKSUM_ERROR;
                break;
            }

            // Completed
            state = MWings::State::COMPLETED;
        } else if (character == 'X') {
            state = MWings::State::WAITING_FOR_COLON;
        }
        break;
    }
    default:
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
