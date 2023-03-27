/**
 * @file   AppTweliteParser.h
 * @brief  App_Twelite parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPTWELITEPARSER_H
#define APPTWELITEPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedAppTwelitePacket
 * @brief  Packet content for App_Twelite
 */
struct ParsedAppTwelitePacket final : public mwings_common::ParsedPacketBase {
    uint8_t u8RelayCount;
    bool bPeriodic;
    bool bDiState[4];
    bool bDiChanged[4];
    uint16_t u16AiVoltage[4];
};

/**
 * @class apptwelite::Parser
 * @brief  Packet parser for App_Twelite
 */
namespace apptwelite {
class Parser final : public mwings_common::ParserBase {
public:
    // Check if the packet is from App_Twelite
    inline bool isValid(const mwings_common::BarePacket& barePacket) const override {
        if ((barePacket.u8At(1) == 0x81)
            and (barePacket.u8At(3) == 0x01)
            and ((barePacket.u8At(5) & 0x80) == 0x80)
            and (barePacket.size == 24)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const mwings_common::BarePacket& barePacket, mwings_common::ParsedPacketBase* const parsedPacket) const override;
};
}

extern apptwelite::Parser AppTweliteParser;

#endif  // APPTWELITEPARSER_H