/**
 * @file   AppIoParser.h
 * @brief  App_IO parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPIOPARSER_H
#define APPIOPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedAppIoPacket
 * @brief  Packet content for App_IO
 */
struct ParsedAppIoPacket final : public mwings_common::ParsedPacketBase {
    uint8_t u8RelayCount;
    bool bDiState[12];
    bool bDiValid[12];
    bool bDiInterrupt[12];
};

/**
 * @class appio::Parser
 * @brief  Packet parser for App_IO
 */
namespace appio {
class Parser final : public mwings_common::ParserBase {
public:
    // Check if the packet is from App_IO
    inline bool isValid(const mwings_common::BarePacket& barePacket) const override {
        if ((barePacket.u8At(1) == 0x81)
            and (barePacket.u8At(3) == 0x02)
            and ((barePacket.u8At(5) & 0x80) == 0x80)
            and (barePacket.size == 21)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const mwings_common::BarePacket& barePacket, mwings_common::ParsedPacketBase* const parsedPacket) const override;
};
}

extern appio::Parser AppIoParser;

#endif  // APPIOPARSER_H
