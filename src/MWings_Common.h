/**
 * @file   MWings_Common.h
 * @brief  Mono Wireless TWELITE Wings API Common Definitions
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef MWINGS_COMMON_H
#define MWINGS_COMMON_H

#include <Arduino.h>

namespace mwings_common {

/**
 * @struct BarePacket
 * @brief  Bare packet contents
 */
struct BarePacket {
    uint8_t* payload;
    uint16_t size;

    inline uint8_t u8At(const int index) const { return payload[index]; }
    inline int8_t i8At(const int index) const { return static_cast<int8_t>(u8At(index)); }
    inline uint16_t u16At(const int index) const { return (payload[index] << 8) | payload[index+1]; }
    inline int16_t i16At(const int index) const { return static_cast<int16_t>(u16At(index)); }
    inline uint32_t u32At(const int index) const {
        return (payload[index+0] << 24) | (payload[index+1] << 16) | (payload[index+2] << 8) | (payload[index+3] << 0);
    }
    inline int32_t i32At(const int index) const { return static_cast<int16_t>(u16At(index)); }
};

/**
 * @struct ParsedPacketBase
 * @brief  Contains essential packet contents
 */
struct ParsedPacketBase {
    uint32_t u32SourceSerialId;
    uint8_t u8SourceLogicalId;
    uint16_t u16SequenceNumber;
    uint8_t u8Lqi;
    uint16_t u16SupplyVoltage;
protected:
    ParsedPacketBase() {}
    virtual ~ParsedPacketBase() {}
};

/**
 * @class  ParserBase
 * @brief  Packet parser template
 */
class ParserBase {
public:
    // Check if the packet is valid or not
    virtual bool isValid(const BarePacket& barePacket) const = 0;
    // Get parsed packet from bare packet
    virtual bool parse(const BarePacket& barePacket, ParsedPacketBase* const parsedPacket) const = 0;
protected:
    ParserBase() {}
    virtual ~ParserBase() {}
};
}

#endif  // MWINGS_COMMON_H
