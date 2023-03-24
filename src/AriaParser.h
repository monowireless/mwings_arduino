/**
 * @file   AriaParser.h
 * @brief  App_ARIA (ARIA mode) parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef ARIAPARSER_H
#define ARIAPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedAriaPacket
 * @brief  Packet content for App_ARIA
 */
struct ParsedAriaPacket final : public mwings_common::ParsedPacketBase {
    int16_t i16Temp100x;
    uint16_t u16Humid100x;
    uint8_t u8MagnetState;
    bool bMagnetStateChanged;
};

/**
 * @class aria::Parser
 * @brief  Packet parser for App_ARIA (ARIA mode)
 */
namespace aria {
class Parser final : public mwings_common::ParserBase {
public:
    // Check if the packet is from App_ARIA (ARIA mode)
    inline bool isValid(const mwings_common::BarePacket& barePacket) const override {
        if (((barePacket.u8At(0) & 0x80) == 0x80)
            and ((barePacket.u8At(7) & 0x80) == 0x80)
            and (barePacket.u8At(12) == 0x80)
            and (barePacket.u8At(13) == 0x06)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const mwings_common::BarePacket& barePacket, mwings_common::ParsedPacketBase* const parsedPacket) const override;
};
}

extern aria::Parser AriaParser;

#endif  // ARIAPARSER_H
