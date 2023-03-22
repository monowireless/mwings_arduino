/**
 * @file   CueParser.h
 * @brief  App_CUE (CUE mode) parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef CUEPARSER_H
#define CUEPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedCuePacket
 * @brief  Packet content for App_CUE
 */
struct ParsedCuePacket final : public mwings_common::ParsedPacketBase {
    int16_t i16SamplesX[10];
    int16_t i16SamplesY[10];
    int16_t i16SamplesZ[10];
    uint8_t u8SampleCount;
    bool bHasAccelEvent;
    uint8_t u8AccelEvent;
    uint8_t u8MagnetState;
    bool bMagnetStateChanged;
};

/**
 * @class cue::Parser
 * @brief  Packet parser for App_CUE (CUE mode)
 */
namespace cue {
class Parser final : public mwings_common::ParserBase {
public:
    // Check if the packet is TWELITE CUE
    inline bool isValid(const mwings_common::BarePacket& barePacket) const override {
        if (((barePacket.u8At(0) & 0x80) == 0x80)
            and ((barePacket.u8At(7) & 0x80) == 0x80)
            and (barePacket.u8At(12) == 0x80)
            and (barePacket.u8At(13) == 0x05)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const mwings_common::BarePacket& barePacket, mwings_common::ParsedPacketBase* const parsedPacket) const override;
};
}

extern cue::Parser CueParser;

#endif  // CUEPARSER_H
