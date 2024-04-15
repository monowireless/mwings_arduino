/**
 * @file   ActPacketParser.h
 * @brief  Act packet parser for MWings.
 *
 * Copyright (C) 2024 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef ACTPACKETPARSER_H
#define ACTPACKETPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedActPacket
 * @brief  Packet content for Act
 */
struct ParsedActPacket final : public mwings::ParsedPacketBase {
    uint8_t u8CommandId;
    uint8_t* u8Data;
    uint16_t u16DataSize;
};

/**
 * @class act::PacketParser
 * @brief  Packet parser for Act
 */
namespace act {
class PacketParser final : public mwings::PacketParserBase {
public:
    // Check if the packet is from Act
    inline bool isValid(const BarePacket& barePacket) const override {
        if ((barePacket.u8At(1) == 0xAA)
            and ((0x00 <= barePacket.u8At(2)) and (barePacket.u8At(2) <= 0x7F))
            and ((barePacket.u8At(3) & 0x80) == 0x80)
            and (((barePacket.u8At(7) & 0x80) == 0x80) or barePacket.u32At(7) == 0)
            and (barePacket.u16At(12) == barePacket.u16PayloadSize - 14)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const override;
};
}

extern act::PacketParser ActPacketParser;

#endif  // ACTPACKETPARSER_H
