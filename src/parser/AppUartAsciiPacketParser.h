/**
 * @file   AppUartAsciiPacketParser.h
 * @brief  App_Uart (A mode) packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPUARTASCIIPACKETPARSER_H
#define APPUARTASCIIPACKETPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedAppUartAsciiPacket
 * @brief  Packet content for App_Uart (A mode)
 */
struct ParsedAppUartAsciiPacket final : public mwings_common::ParsedPacketBase {
    uint8_t u8ResponseId;
    uint8_t* u8Data;
    uint16_t u16DataSize;
};

/**
 * @class uartascii::PacketParser
 * @brief  Packet parser for App_Uart (A mode)
 */
namespace uartascii {
class PacketParser final : public mwings_common::PacketParserBase {
public:
    // Check if the packet is from App_Uart (A mode)
    inline bool isValid(const BarePacket& barePacket) const override {
        if (((0x00 <= barePacket.u8At(0) and barePacket.u8At(0) <= 0x64) or barePacket.u8At(0) == 0x78)
            and (barePacket.u8At(1) < 0x80)
            and (4 <= barePacket.size and barePacket.size <= 643)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const BarePacket& barePacket, mwings_common::ParsedPacketBase* const parsedPacket) const override;
};
}

extern uartascii::PacketParser AppUartAsciiPacketParser;

#endif  // APPUARTASCIIPACKETPARSER_H
