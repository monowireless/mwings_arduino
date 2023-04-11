/**
 * @file   AppUartAsciiExtendedPacketParser.h
 * @brief  App_Uart (A mode) extended packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPUARTASCIIEXTENDEDPACKETPARSER_H
#define APPUARTASCIIEXTENDEDPACKETPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedAppUartAsciiExtendedPacket
 * @brief  Extended packet content for App_Uart (A mode)
 */
struct ParsedAppUartAsciiExtendedPacket final : public mwings::ParsedPacketBase {
    uint32_t u32DestinationSerialId;
    uint8_t u8ResponseId;
    uint8_t* u8Data;
    uint16_t u16DataSize;
};

/**
 * @class uartascii::ExtendedPacketParser
 * @brief  Extended packet parser for App_Uart (A mode)
 */
namespace uartascii {
class ExtendedPacketParser final : public mwings::PacketParserBase {
public:
    // Check if the packet is from App_Uart (A mode)
    inline bool isValid(const BarePacket& barePacket) const override {
        if (((0x00 <= barePacket.u8At(0) and barePacket.u8At(0) <= 0x64) or barePacket.u8At(0) == 0x78)
            and (barePacket.u8At(1) == 0xA0)
            and (barePacket.u8At(2) < 0x80)
            and ((barePacket.u8At(3) & 0x80) == 0x80)
            and (barePacket.u16At(12) == barePacket.size - 15)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const override;
};
}

extern uartascii::ExtendedPacketParser AppUartAsciiExtendedPacketParser;

#endif  // APPUARTASCIIEXTENDEDPACKETPARSER_H
