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
    uint16_t checksum;

    uint8_t at_u8(int index) const { return payload[index]; }
    uint16_t at_u16(int index) const { return (payload[index] << 8) | payload[index+1]; }
    int16_t at_i16(int index) const { return static_cast<int16_t>(at_u16(index)); }
    uint32_t at_u32(int index) const { return (payload[index+0] << 24) | (payload[index+1] << 16) | (payload[index+2] << 8) | (payload[index+3] << 0); }
    int32_t at_i32(int index) const { return static_cast<int16_t>(at_u16(index)); }
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
