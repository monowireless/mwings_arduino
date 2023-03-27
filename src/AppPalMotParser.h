/**
 * @file   AppPalMotParser.h
 * @brief  App_PAL (MOT) parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPPALMOTPARSER_H
#define APPPALMOTPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedAppPalMotPacket
 * @brief  Packet content for App_PAL
 */
struct ParsedAppPalMotPacket final : public mwings_common::ParsedPacketBase {
    int16_t i16SamplesX[16];
    int16_t i16SamplesY[16];
    int16_t i16SamplesZ[16];
    uint8_t u8SampleCount;
};

/**
 * @class palmot::Parser
 * @brief  Packet parser for App_PAL (MOT)
 */
namespace palmot {
class Parser final : public mwings_common::ParserBase {
public:
    // Check if the packet is from App_PAL (MOT)
    inline bool isValid(const mwings_common::BarePacket& barePacket) const override {
        if (((barePacket.u8At(0) & 0x80) == 0x80)
            and ((barePacket.u8At(7) & 0x80) == 0x80)
            and (barePacket.u8At(12) == 0x80)
            and (barePacket.u8At(13) == 0x83)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const mwings_common::BarePacket& barePacket, mwings_common::ParsedPacketBase* const parsedPacket) const override;
};
}

extern palmot::Parser AppPalMotParser;

#endif  // APPPALMOTPARSER_H
