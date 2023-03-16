/**
 * @file   MWings.h
 * @brief  Mono Wireless TWELITE Wings API for Arduino on ESP32.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef MWINGS_H
#define MWINGS_H

#include <Arduino.h>
#include "MWings_Common.h"

//// AriaParser for App_ARIA (ARIA mode)
#include "AriaParser.h"
//// CueParser for App_CUE (CUE mode)
#include "CueParser.h"
//// PalAmbParser for App_PAL (AMB)
#include "PalAmbParser.h"
//// PalMotParser for App_PAL (MOT)
#include "PalMotParser.h"

class MWings {
public:
    MWings() : _serial(nullptr),
               _resetPin(-1), _programPin(-1), _indicatorPin(-1),
               _buffer(nullptr), _bufferSize(0), _characterCount(0), _checksum(0),
               _latestTimestamp(UINT32_MAX), _timeout(0),
               _isIndicatorOn(false), _indicatorTimestamp(UINT32_MAX), _indicatorDuration(0),
               _debugSerial(nullptr),
               //// AriaParser for App_ARIA (ARIA mode)
               _onAriaPacket(nullptr),
               //// CueParser for App_CUE (CUE mode)
               _onCuePacket(nullptr),
               //// PalAmbParser for App_PAL (AMB)
               _onPalAmbPacket(nullptr),
               //// PalMotParser for App_PAL (MOT)
               _onPalMotPacket(nullptr)
        {}
    ~MWings();

    inline void debugUsing(HardwareSerial& debugSerial) { _debugSerial = &debugSerial; }

    void setup(HardwareSerial& serial,
               const int resetPin = -1, const int programPin = -1, const int indicatorPin = -1,
               const int bufferSize = 1024);

    bool begin(const uint8_t channel = 18, const uint32_t appId = 0x67720102);

    inline void end() const {}

    void update();

private:
    enum class State {
        WAITING_FOR_COLON,
        READING_PAYLOAD,
        COMPLETED,
        UNKNOWN_ERROR,
        CHECKSUM_ERROR
    };

    enum class Command : uint8_t {
        ACK = 0xF0,
        MODULE_ADDRESS = 0xF1,
        SET_PARAMETER = 0xF2,
        GET_PARAMETER = 0xF3,
        CONTROL = 0xF8,
        CLEAR_RESET = 0xFD,
        SAVE_RESET = 0xFE,
        RESET = 0xFF
    };

    enum class Parameter : uint8_t {
        APP_ID = 0x00,
        CH_MASK = 0x01,
        TX_RETRY = 0x02,
        LOGICAL_ID = 0x03,
        ROLE = 0x04,
        ROUTING_LAYER = 0x05,
        UART_MODE = 0x06,
        UART_BAUDRATE = 0x07,
        UART_PARITY = 0x08,
        ENCRYPTION = 0x09,
        KEY = 0x0A,
        TRIGGER = 0x0C,
        ERROR = 0xFF
    };

    inline void debugWrite(const uint8_t data) const {
        if (_debugSerial) {
            _debugSerial->write(data);
            _debugSerial->flush();
        }
    }

    inline void debugPrint(const char* const str) const {
        if (_debugSerial) {
            _debugSerial->write('\n');
            _debugSerial->print(str);
            _debugSerial->write('\n');
            _debugSerial->flush();
        }
    }

    inline void turnOnIndicator() {
        if (not (_indicatorPin < 0)) {
            digitalWrite(_indicatorPin, LOW);
            _isIndicatorOn = true;
        }
    }

    inline void turnOffIndicator() {
        if (not (_indicatorPin < 0)) {
            digitalWrite(_indicatorPin, HIGH);
            _isIndicatorOn = false;
        }
    }

    inline void turnOnIndicatorFor(const uint16_t duration)  {
        _indicatorTimestamp = millis();
        _indicatorDuration = duration;
        turnOnIndicator();
    }

    inline void updateIndicator() {
        if (_isIndicatorOn and (millis() - _indicatorTimestamp > _indicatorDuration)) {
            turnOffIndicator();
            _indicatorTimestamp = UINT32_MAX;
            _indicatorDuration = 0;
        }
    }

    State processAscii(const uint8_t character, mwings_common::BarePacket& barePacket);

    inline uint8_t hexFrom(const uint8_t character) const {
        const uint8_t hexTable[] = {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
            0, 0, 0, 0, 0, 0, 0,
            0xA, 0xB, 0xC, 0xD, 0xE, 0xF
        };
        return hexTable[character - '0'];
    }

    inline uint8_t characterFrom(const uint8_t hex) const {
        return (hex < 0xA) ? ('0' + hex) : ('A' + hex - 0xA);
    }

    inline int byteCountFrom(const int characterCount) const {
        return characterCount / 2;
    }

    inline void writeInAscii(const uint8_t data) const {
        if (not _serial) { return; }
        _serial->write(characterFrom((data >> 4) & 0x0F));
        _serial->write(characterFrom((data >> 0) & 0x0F));
    }

    inline void writeInAscii(const uint16_t data) const {
        if (not _serial) { return; }
        _serial->write(characterFrom((data >> 12) & 0x0F));
        _serial->write(characterFrom((data >> 8) & 0x0F));
        _serial->write(characterFrom((data >> 4) & 0x0F));
        _serial->write(characterFrom((data >> 0) & 0x0F));
    }

    inline void writeInAscii(const uint32_t data) const {
        if (not _serial) { return; }
        _serial->write(characterFrom((data >> 28) & 0x0F));
        _serial->write(characterFrom((data >> 24) & 0x0F));
        _serial->write(characterFrom((data >> 20) & 0x0F));
        _serial->write(characterFrom((data >> 16) & 0x0F));
        _serial->write(characterFrom((data >> 12) & 0x0F));
        _serial->write(characterFrom((data >> 8) & 0x0F));
        _serial->write(characterFrom((data >> 4) & 0x0F));
        _serial->write(characterFrom((data >> 0) & 0x0F));
    }

    inline void beginCommand() const {
        if (not _serial) { return; }
        _serial->write(':');
        writeInAscii(static_cast<uint8_t>(0xDB));
    }

    inline void endCommand() const {
        if (not _serial) { return; }
        _serial->write('X');
        _serial->flush();
    }

    inline bool find(const uint8_t* const data, const int size, const uint16_t timeout) const {
        if (not _serial) { return false; }
        if (not (size > 0)) { return false; }
        uint32_t timestamp = millis();
        while (true) {
            if (millis() - timestamp > timeout) { return false; }
            if (_serial->available()) {
                //debugWrite(_serial->peek());
                if (_serial->read() == data[0]) {
                    break;
                }
            }
        }
        for (int i = 1; i < size; i++) {
            if (data[i] == '\0') { break; }
            while (true) {
                if (millis() - timestamp > timeout) { return false; }
                if (_serial->available()) {
                    //debugWrite(_serial->peek());
                    if (_serial->read() == data[i]) {
                        break;
                    } else {
                        return find(data, size, timeout - millis() + timestamp); // Recursive
                    }
                }
            }
        }
        return true;
    }

    inline bool find(const char* const data, const uint16_t timeout) const {
        return find(reinterpret_cast<const uint8_t*>(data), 1024, timeout);
    }

    inline bool ensureSetParameter(const uint32_t timeout) const {
        if (not _serial) { return false; }
        return find(":DBF3", timeout) and (not (_serial->read() == static_cast<uint8_t>(MWings::Parameter::ERROR)));
    }

    inline bool ensureStarted(const uint32_t timeout) const {
        if (not _serial) { return false; }
        return find("!INF MW APP_WINGS(Parent)", timeout);
    }

    inline bool ensureControlled(const uint32_t timeout) const {
        if (not _serial) { return false; }
        return find(":DBF8", timeout);
    }

    HardwareSerial* _serial;

    int _resetPin;
    int _programPin;
    int _indicatorPin;

    bool _isIndicatorOn;
    uint32_t _indicatorTimestamp;
    uint16_t _indicatorDuration;

    uint8_t* _buffer;
    int _bufferSize;
    int _characterCount;
    uint16_t _checksum;
    uint16_t _timeout;
    uint32_t _latestTimestamp;

    HardwareSerial* _debugSerial;   // optional

public:
    //// AriaParser for App_ARIA (ARIA mode)
    inline void on(void (*callback)(const ParsedAriaPacket& packet)) { _onAriaPacket = callback; }
    //// CueParser for App_CUE (CUE mode)
    inline void on(void (*callback)(const ParsedCuePacket& packet)) { _onCuePacket = callback; }
    //// PalAmbParser for App_PAL (AMB)
    inline void on(void (*callback)(const ParsedPalAmbPacket& packet)) { _onPalAmbPacket = callback; }
    //// PalMotParser for App_PAL (MOT)
    inline void on(void (*callback)(const ParsedPalMotPacket& packet)) { _onPalMotPacket = callback; }

private:
    //// AriaParser for App_ARIA (ARIA mode)
    void (*_onAriaPacket)(const ParsedAriaPacket& packet);
    //// CueParser for App_CUE (CUE mode)
    void (*_onCuePacket)(const ParsedCuePacket& packet);
    //// PalAmbParser for App_PAL (AMB)
    void (*_onPalAmbPacket)(const ParsedPalAmbPacket& packet);
    //// PalMotParser for App_PAL (MOT)
    void (*_onPalMotPacket)(const ParsedPalMotPacket& packet);
};

#endif  // MWINGS_H
