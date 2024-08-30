/**
 * @file   AppTagAdcPacketParser.h
 * @brief  App_Tag (ADC) packet parser for MWings.
 *
 * Copyright (C) 2024 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPTAGADCPACKETPARSER_H
#define APPTAGADCPACKETPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedAppTagAdcPacket
 * @brief  Packet content for App_Twelite
 */
struct ParsedAppTagAdcPacket final : public mwings::ParsedPacketBase {
    uint32_t u32RouterSerialId;
    uint8_t u8SensorType;
    uint16_t u16AiVoltage[2];
};

/**
 * @class apptagadc::PacketParser
 * @brief  Packet parser for App_Twelite
 */
namespace apptagadc {
class PacketParser final : public mwings::PacketParserBase {
public:
    // Check if the packet is from App_Twelite
    inline bool isValid(const BarePacket& barePacket) const override {
        if ((barePacket.u8At(12) == 0x10)
            and (barePacket.u32At(18) == 0)
            and (barePacket.u16PayloadSize == 22)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const override;
};
}

extern apptagadc::PacketParser AppTagAdcPacketParser;

#endif  // APPTAGADCPACKETPARSER_H
