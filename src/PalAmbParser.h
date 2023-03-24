/**
 * @file   PalAmbParser.h
 * @brief  App_PAL (AMB) parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef PALAMBPARSER_H
#define PALAMBPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedPalAmbPacket
 * @brief  Packet content for App_PAL
 */
struct ParsedPalAmbPacket final : public mwings_common::ParsedPacketBase {
    int16_t i16Temp100x;
    uint16_t u16Humid100x;
    uint32_t u32Luminance;
};

/**
 * @class palamb::Parser
 * @brief  Packet parser for App_PAL (AMB)
 */
namespace palamb {
class Parser final : public mwings_common::ParserBase {
public:
    // Check if the packet is App_PAL (AMB)
    inline bool isValid(const mwings_common::BarePacket& barePacket) const override {
        if (((barePacket.u8At(0) & 0x80) == 0x80)
            and ((barePacket.u8At(7) & 0x80) == 0x80)
            and (barePacket.u8At(12) == 0x80)
            and (barePacket.u8At(13) == 0x82)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const mwings_common::BarePacket& barePacket, mwings_common::ParsedPacketBase* const parsedPacket) const override;
};
}

extern palamb::Parser PalAmbParser;

#endif  // PALAMBPARSER_H
