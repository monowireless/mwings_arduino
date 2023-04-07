/**
 * @file   MWings.h
 * @brief  Mono Wireless TWELITE Wings API for 32-bit Arduinos.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#if defined(ARDUINO_ARCH_ESP32)
#elif defined(ARDUINO_ARCH_ESP8266)
#elif defined(ARDUINO_ARCH_STM32)
#elif defined(ARDUINO_ARCH_SAM)
#elif defined(ARDUINO_ARCH_RP2040)
#else
#error “This library is built for 32-bit MCUs like ESP32, ESP8266, STM32, SAM and RP2040.”
#endif

#ifndef MWINGS_H
#define MWINGS_H

#include <Arduino.h>
#include "MWings_Common.h"

//// AppTwelitePacketParser for App_Twelite
#include "parser/AppTwelitePacketParser.h"
//// AppIoPacketParser for App_IO
#include "parser/AppIoPacketParser.h"
//// AppAriaPacketParser for App_ARIA (ARIA mode)
#include "parser/AppAriaPacketParser.h"
//// AppCuePacketParser for App_CUE (CUE mode)
#include "parser/AppCuePacketParser.h"
//// AppPalOpenClosePacketParser for App_PAL (OPENCLOSE)
#include "parser/AppPalOpenClosePacketParser.h"
//// AppPalAmbPacketParser for App_PAL (AMB)
#include "parser/AppPalAmbPacketParser.h"
//// AppPalMotPacketParser for App_PAL (MOT)
#include "parser/AppPalMotPacketParser.h"
//// AppUartAsciiPacketParser for App_Uart (A mode)
#include "parser/AppUartAsciiPacketParser.h"
//// AppUartAsciiExtendedPacketParser for App_Uart (A mode, extended)
#include "parser/AppUartAsciiExtendedPacketParser.h"

//// AppTweliteCommandSerializer for App_Twelite
#include "serializer/AppTweliteCommandSerializer.h"
//// AppPalNoticeCommandSerializer for App_PAL (NOTICE)
#include "serializer/AppPalNoticeCommandSerializer.h"
//// AppPalNoticeDetailedCommandSerializer for App_PAL (NOTICE), detailed
#include "serializer/AppPalNoticeDetailedCommandSerializer.h"
//// AppPalNoticeEventCommandSerializer for App_PAL (NOTICE), event
#include "serializer/AppPalNoticeEventCommandSerializer.h"

class MWings {
public:
    MWings() : _serial(nullptr),
               _indicatorPin(-1), _resetPin(-1), _programPin(-1),
               _isIndicatorOn(false), _indicatorTimestamp(UINT32_MAX), _indicatorDuration(0),
               _buffer(nullptr), _bufferSize(0), _characterCount(0), _checksum(0),
               _timeout(0), _latestTimestamp(UINT32_MAX),
               _debugSerial(nullptr),
               //// AppTwelitePacketParser for App_Twelite
               _onAppTwelitePacket(nullptr),
               //// AppIoPacketParser for App_IO
               _onAppIoPacket(nullptr),
               //// AppAriaPacketParser for App_ARIA (ARIA mode)
               _onAppAriaPacket(nullptr),
               //// AppCuePacketParser for App_CUE (CUE mode)
               _onAppCuePacket(nullptr),
               //// AppPalOpenClosePacketParser for App_PAL (OPENCLOSE)
               _onAppPalOpenClosePacket(nullptr),
               //// AppPalAmbPacketParser for App_PAL (AMB)
               _onAppPalAmbPacket(nullptr),
               //// AppPalMotPacketParser for App_PAL (MOT)
               _onAppPalMotPacket(nullptr),
               //// AppUartAsciiPacketParser for App_Uart (A mode)
               _onAppUartAsciiPacket(nullptr),
               //// AppUartAsciiExtendedPacketParser for App_Uart (A mode, extended)
               _onAppUartAsciiExtendedPacket(nullptr)
        {}
    ~MWings();

    bool begin(HardwareSerial& serial,
               const uint8_t channel = 18, const uint32_t appId = 0x67720102,
               const int indicatorPin = -1, const int resetPin = -1, const int programPin = -1,
               const int bufferSize = 1024, const int timeout = 100,
               HardwareSerial* debugSerial = nullptr);

    inline void end() {
        _serial = nullptr;
        _indicatorPin = -1, _resetPin = -1; _programPin = -1;
        _isIndicatorOn = false; _indicatorTimestamp = UINT32_MAX; _indicatorDuration = 0;
        if (_buffer) { delete[] _buffer; }
        _bufferSize = 0; _characterCount = 0; _checksum = 0;
        _timeout = 0; _latestTimestamp = UINT32_MAX;
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
        //// AppUartAsciiPacketParser for App_Uart (A mode)
        _onAppUartAsciiPacket = nullptr;
        //// AppUartAsciiExtendedPacketParser for App_Uart (A mode, extended)
        _onAppUartAsciiExtendedPacket = nullptr;
    }

    void update();

private:
    enum class State {
        WAITING_FOR_HEADER,
        RETRIEVING_PAYLOAD,
        WAITING_FOR_FOOTER,
        COMPLETED,
        UNKNOWN_ERROR,
        CHECKSUM_ERROR,
        TIMEOUT_ERROR
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
        }
    }

    inline void debugPrint(const char* const str) const {
        if (_debugSerial) {
            _debugSerial->write('\n');
            _debugSerial->print(str);
            _debugSerial->write('\n');
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
        if (not _serial->availableForWrite()) { return; }
        _serial->write(characterFrom((data >> 4) & 0x0F));
        _serial->write(characterFrom((data >> 0) & 0x0F));
    }

    inline void writeInAscii(const uint16_t data) const {
        if (not _serial) { return; }
        if (not _serial->availableForWrite()) { return; }
        _serial->write(characterFrom((data >> 12) & 0x0F));
        _serial->write(characterFrom((data >> 8) & 0x0F));
        _serial->write(characterFrom((data >> 4) & 0x0F));
        _serial->write(characterFrom((data >> 0) & 0x0F));
    }

    inline void writeInAscii(const uint32_t data) const {
        if (not _serial) { return; }
        if (not _serial->availableForWrite()) { return; }
        _serial->write(characterFrom((data >> 28) & 0x0F));
        _serial->write(characterFrom((data >> 24) & 0x0F));
        _serial->write(characterFrom((data >> 20) & 0x0F));
        _serial->write(characterFrom((data >> 16) & 0x0F));
        _serial->write(characterFrom((data >> 12) & 0x0F));
        _serial->write(characterFrom((data >> 8) & 0x0F));
        _serial->write(characterFrom((data >> 4) & 0x0F));
        _serial->write(characterFrom((data >> 0) & 0x0F));
    }

    inline void writeInAscii(const uint8_t* const data, const int size) const {
        if (not _serial) { return; }
        if (not _serial->availableForWrite()) { return; }
        for (int i = 0; i < size; i++) { writeInAscii(data[i]); }
    }

    inline void debugWriteInAscii(const uint8_t data) const {
        if (not _debugSerial) { return; }
        if (not _debugSerial->availableForWrite()) { return; }
        _debugSerial->write(characterFrom((data >> 4) & 0x0F));
        _debugSerial->write(characterFrom((data >> 0) & 0x0F));
    }

    inline void debugWriteInAscii(const uint16_t data) const {
        if (not _debugSerial) { return; }
        if (not _debugSerial->availableForWrite()) { return; }
        _debugSerial->write(characterFrom((data >> 12) & 0x0F));
        _debugSerial->write(characterFrom((data >> 8) & 0x0F));
        _debugSerial->write(characterFrom((data >> 4) & 0x0F));
        _debugSerial->write(characterFrom((data >> 0) & 0x0F));
    }

    inline void debugWriteInAscii(const uint32_t data) const {
        if (not _debugSerial) { return; }
        if (not _debugSerial->availableForWrite()) { return; }
        _debugSerial->write(characterFrom((data >> 28) & 0x0F));
        _debugSerial->write(characterFrom((data >> 24) & 0x0F));
        _debugSerial->write(characterFrom((data >> 20) & 0x0F));
        _debugSerial->write(characterFrom((data >> 16) & 0x0F));
        _debugSerial->write(characterFrom((data >> 12) & 0x0F));
        _debugSerial->write(characterFrom((data >> 8) & 0x0F));
        _debugSerial->write(characterFrom((data >> 4) & 0x0F));
        _debugSerial->write(characterFrom((data >> 0) & 0x0F));
    }

    inline void debugWriteInAscii(const uint8_t* const data, const int size) const {
        if (not _debugSerial) { return; }
        if (not _debugSerial->availableForWrite()) { return; }
        for (int i = 0; i < size; i++) { debugWriteInAscii(data[i]); }
    }

    inline void beginCommand() const {
        if (not _serial) { return; }
        if (not _serial->availableForWrite()) { return; }
        _serial->write(':');
        writeInAscii(static_cast<uint8_t>(0xDB));
    }

    inline void endCommand() const {
        if (not _serial) { return; }
        if (not _serial->availableForWrite()) { return; }
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

    inline bool isThereAck(const uint32_t timeout) const {
        if (not _serial) { return false; }
        return find(":DBF0", timeout);
    }

    inline bool ensureSetParameter(const uint32_t timeout) const {
        if (not _serial) { return false; }
        return find(":DBF3", timeout) and (not (_serial->read() == static_cast<uint8_t>(MWings::Parameter::ERROR)));
    }

    inline bool ensureReset(const uint32_t timeout) const {
        if (not _serial) { return false; }
        return find("!INF MW APP_WINGS(Parent)", timeout);
    }

    inline bool ensureControlled(const uint32_t timeout) const {
        if (not _serial) { return false; }
        return find(":DBF8", timeout);
    }

    inline void flushSerialRxBuffer() const {
        if (not _serial) { return; }
        while (_serial->available()) { _serial->read(); }
    }

    HardwareSerial* _serial;

    int _indicatorPin;
    int _resetPin;
    int _programPin;

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
    //// AppTwelitePacketParser for App_Twelite
    inline void on(void (*callback)(const ParsedAppTwelitePacket& packet)) { _onAppTwelitePacket = callback; }
    //// AppIoPacketParser for App_IO
    inline void on(void (*callback)(const ParsedAppIoPacket& packet)) { _onAppIoPacket = callback; }
    //// AppAriaPacketParser for App_ARIA (ARIA mode)
    inline void on(void (*callback)(const ParsedAppAriaPacket& packet)) { _onAppAriaPacket = callback; }
    //// AppCuePacketParser for App_CUE (CUE mode)
    inline void on(void (*callback)(const ParsedAppCuePacket& packet)) { _onAppCuePacket = callback; }
    //// AppPalOpenClosePacketParser for App_PAL (OPENCLOSE)
    inline void on(void (*callback)(const ParsedAppPalOpenClosePacket& packet)) { _onAppPalOpenClosePacket = callback; }
    //// AppPalAmbPacketParser for App_PAL (AMB)
    inline void on(void (*callback)(const ParsedAppPalAmbPacket& packet)) { _onAppPalAmbPacket = callback; }
    //// AppPalMotPacketParser for App_PAL (MOT)
    inline void on(void (*callback)(const ParsedAppPalMotPacket& packet)) { _onAppPalMotPacket = callback; }
    //// AppUartAsciiPacketParser for App_Uart (A mode)
    inline void on(void (*callback)(const ParsedAppUartAsciiPacket& packet)) { _onAppUartAsciiPacket = callback; }
    //// AppUartAsciiExtendedPacketParser for App_Uart (A mode, extended)
    inline void on(void (*callback)(const ParsedAppUartAsciiExtendedPacket& packet)) { _onAppUartAsciiExtendedPacket = callback; }

    //// AppTweliteCommandSerializer for App_Twelite
    inline bool send(AppTweliteCommand& command) {
        if (not _serial) { return false; }
        if (not _serial->availableForWrite()) { return false; }
        turnOnIndicatorFor(10);
        constexpr int fixedPayloadSize = GetSerializedAppTweliteCommandPayloadSize();
        uint8_t payload[fixedPayloadSize];
        uint8_t checksum;
        if (not AppTweliteCommandSerializer.serialize(&command, payload, fixedPayloadSize, &checksum)) { return false; }
        _serial->write(':');
        writeInAscii(payload, fixedPayloadSize);
        writeInAscii(checksum);
        _serial->write('\r'); _serial->write('\n');
        if (_debugSerial) {
            _debugSerial->write(':');
            debugWriteInAscii(payload, fixedPayloadSize);
            debugWriteInAscii(checksum);
            _debugSerial->write('\r'); _debugSerial->write('\n');
        }
        return true;
    }
    //// AppPalNoticeCommandSerializer for App_PalNotice
    inline bool send(AppPalNoticeCommand& command) {
        if (not _serial) { return false; }
        if (not _serial->availableForWrite()) { return false; }
        turnOnIndicatorFor(10);
        constexpr int fixedPayloadSize = GetSerializedAppPalNoticeCommandPayloadSize();
        uint8_t payload[fixedPayloadSize];
        uint8_t checksum;
        if (not AppPalNoticeCommandSerializer.serialize(&command, payload, fixedPayloadSize, &checksum)) { return false; }
        _serial->write(':');
        writeInAscii(payload, fixedPayloadSize);
        writeInAscii(checksum);
        _serial->write('\r'); _serial->write('\n');
        if (_debugSerial) {
            _debugSerial->write(':');
            debugWriteInAscii(payload, fixedPayloadSize);
            debugWriteInAscii(checksum);
            _debugSerial->write('\r'); _debugSerial->write('\n');
        }
        return true;
    }
    //// AppPalNoticeDetailedCommandSerializer for App_PalNotice
    inline bool send(AppPalNoticeDetailedCommand& command) {
        if (not _serial) { return false; }
        if (not _serial->availableForWrite()) { return false; }
        constexpr int fixedPayloadSize = GetSerializedAppPalNoticeDetailedCommandPayloadSize();
        uint8_t payload[fixedPayloadSize];
        uint8_t checksum;
        if (not AppPalNoticeDetailedCommandSerializer.serialize(&command, payload, fixedPayloadSize, &checksum)) { return false; }
        _serial->write(':');
        writeInAscii(payload, fixedPayloadSize);
        writeInAscii(checksum);
        _serial->write('\r'); _serial->write('\n');
        if (_debugSerial) {
            _debugSerial->write(':');
            debugWriteInAscii(payload, fixedPayloadSize);
            debugWriteInAscii(checksum);
            _debugSerial->write('\r'); _debugSerial->write('\n');
        }
        return true;
    }
    //// AppPalNoticeEventCommandSerializer for App_PalNotice
    inline bool send(AppPalNoticeEventCommand& command) {
        if (not _serial) { return false; }
        if (not _serial->availableForWrite()) { return false; }
        constexpr int fixedPayloadSize = GetSerializedAppPalNoticeEventCommandPayloadSize();
        uint8_t payload[fixedPayloadSize];
        uint8_t checksum;
        if (not AppPalNoticeEventCommandSerializer.serialize(&command, payload, fixedPayloadSize, &checksum)) { return false; }
        _serial->write(':');
        writeInAscii(payload, fixedPayloadSize);
        writeInAscii(checksum);
        _serial->write('\r'); _serial->write('\n');
        if (_debugSerial) {
            _debugSerial->write(':');
            debugWriteInAscii(payload, fixedPayloadSize);
            debugWriteInAscii(checksum);
            _debugSerial->write('\r'); _debugSerial->write('\n');
        }
        return true;
    }

private:
    //// AppTwelitePacketParser for App_Twelite
    void (*_onAppTwelitePacket)(const ParsedAppTwelitePacket& packet);
    //// AppIoPacketParser for App_IO
    void (*_onAppIoPacket)(const ParsedAppIoPacket& packet);
    //// AppAriaPacketParser for App_ARIA (ARIA mode)
    void (*_onAppAriaPacket)(const ParsedAppAriaPacket& packet);
    //// AppCuePacketParser for App_CUE (CUE mode)
    void (*_onAppCuePacket)(const ParsedAppCuePacket& packet);
    //// AppPalOpenClosePacketParser for App_PAL (OPENCLOSE)
    void (*_onAppPalOpenClosePacket)(const ParsedAppPalOpenClosePacket& packet);
    //// AppPalAmbPacketParser for App_PAL (AMB)
    void (*_onAppPalAmbPacket)(const ParsedAppPalAmbPacket& packet);
    //// AppPalMotPacketParser for App_PAL (MOT)
    void (*_onAppPalMotPacket)(const ParsedAppPalMotPacket& packet);
    //// AppUartAsciiPacketParser for App_Uart (A mode)
    void (*_onAppUartAsciiPacket)(const ParsedAppUartAsciiPacket& packet);
    //// AppUartAsciiExtendedPacketParser for App_Uart (A mode, extended)
    void (*_onAppUartAsciiExtendedPacket)(const ParsedAppUartAsciiExtendedPacket& packet);
};

extern MWings Twelite;

#endif  // MWINGS_H
