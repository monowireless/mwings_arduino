/**
 * @file   AppTagAdcPacketParser.cpp
 * @brief  App_Tag (ADC) packet parser for MWings.
 *
 * Copyright (C) 2024 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppTagAdcPacketParser.h"

apptagadc::PacketParser AppTagAdcPacketParser;

bool apptagadc::PacketParser::parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedAppTagAdcPacket* const parsedAppTagAdcPacket = static_cast<ParsedAppTagAdcPacket*>(parsedPacket);

    parsedAppTagAdcPacket->u32SourceSerialId = barePacket.u32At(7);
    parsedAppTagAdcPacket->u8SourceLogicalId = barePacket.u8At(11);
    parsedAppTagAdcPacket->u16SequenceNumber = barePacket.u16At(5);
    parsedAppTagAdcPacket->u8Lqi = barePacket.u8At(4);

    const uint16_t ecc = barePacket.u8At(13);
    if (ecc <= 170) {
        parsedAppTagAdcPacket->u16SupplyVoltage = 5 * ecc + 1950;
    } else {
        parsedAppTagAdcPacket->u16SupplyVoltage = 10 * (ecc - 170) + 2800;
    }

    parsedAppTagAdcPacket->u32RouterSerialId = barePacket.u32At(0);
    parsedAppTagAdcPacket->u8SensorType = barePacket.u8At(12);

    for (int i = 0; i < 2; i++) {
        parsedAppTagAdcPacket->u16AiVoltage[i] = barePacket.u16At(2*i+14);
    }

    return true;
}
