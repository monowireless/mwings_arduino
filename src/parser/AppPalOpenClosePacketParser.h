/**
 * @file   AppPalOpenClosePacketParser.h
 * @brief  App_PAL (OPENCLOSE) packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPPALOPENCLOSEPACKETPARSER_H
#define APPPALOPENCLOSEPACKETPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedAppPalOpenClosePacket
 * @brief  Packet content for App_PAL
 */
struct ParsedAppPalOpenClosePacket final : public mwings_common::ParsedPacketBase {
    uint16_t u16Ai1Voltage;
    uint8_t u8MagnetState;
    bool bMagnetStateChanged;
};

/**
 * @class palopenclose::PacketParser
 * @brief  Packet parser for App_PAL (OPENCLOSE)
 */
namespace palopenclose {
class PacketParser final : public mwings_common::PacketParserBase {
public:
    // Check if the packet is from App_PAL (OPENCLOSE)
    inline bool isValid(const mwings_common::BarePacket& barePacket) const override {
        if (((barePacket.u8At(0) & 0x80) == 0x80)
            and ((barePacket.u8At(7) & 0x80) == 0x80)
            and (barePacket.u8At(12) == 0x80)
            and (barePacket.u8At(13) == 0x81)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const mwings_common::BarePacket& barePacket, mwings_common::ParsedPacketBase* const parsedPacket) const override;
};
}

extern palopenclose::PacketParser AppPalOpenClosePacketParser;

#endif  // APPPALOPENCLOSEPACKETPARSER_H
