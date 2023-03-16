/**
 * @file   PalMotParser.cpp
 * @brief  App_PAL (MOT) parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "PalMotParser.h"

palmot::Parser PalMotParser;

bool palmot::Parser::parse(const mwings_common::BarePacket& barePacket, mwings_common::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedPalMotPacket* const parsedPalMotPacket = static_cast<ParsedPalMotPacket*>(parsedPacket);

    parsedPalMotPacket->u32SourceSerialId = barePacket.at_u32(7);
    parsedPalMotPacket->u8SourceLogicalId = barePacket.at_u8(11);
   	parsedPalMotPacket->u16SequenceNumber = barePacket.at_u16(5);
    parsedPalMotPacket->u8Lqi = barePacket.at_u8(4);
    parsedPalMotPacket->u16SupplyVoltage = barePacket.at_u16(19);

    int index;
    uint16_t addr;
    for (index = 0, addr = 0x1F; index < 16; index++, addr += 10) {
        parsedPalMotPacket->i16SamplesX[index] = static_cast<int16_t>(barePacket.at_u16(addr + 0));
        parsedPalMotPacket->i16SamplesY[index] = static_cast<int16_t>(barePacket.at_u16(addr + 2));
        parsedPalMotPacket->i16SamplesZ[index] = static_cast<int16_t>(barePacket.at_u16(addr + 4));
    }
    parsedPalMotPacket->u8SampleCount = index + 1;

    return true;
}
