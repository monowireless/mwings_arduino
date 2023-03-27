/**
 * @file   AppPalMotParser.cpp
 * @brief  App_PAL (MOT) parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppPalMotParser.h"

palmot::Parser AppPalMotParser;

bool palmot::Parser::parse(const mwings_common::BarePacket& barePacket, mwings_common::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedAppPalMotPacket* const parsedAppPalMotPacket = static_cast<ParsedAppPalMotPacket*>(parsedPacket);

    parsedAppPalMotPacket->u32SourceSerialId = barePacket.u32At(7);
    parsedAppPalMotPacket->u8SourceLogicalId = barePacket.u8At(11);
    parsedAppPalMotPacket->u16SequenceNumber = barePacket.u16At(5);
    parsedAppPalMotPacket->u8Lqi = barePacket.u8At(4);
    parsedAppPalMotPacket->u16SupplyVoltage = barePacket.u16At(19);

    int index;
    uint16_t addr;
    for (index = 0, addr = 0x1F; index < 16; index++, addr += 10) {
        parsedAppPalMotPacket->i16SamplesX[index] = barePacket.i16At(addr + 0);
        parsedAppPalMotPacket->i16SamplesY[index] = barePacket.i16At(addr + 2);
        parsedAppPalMotPacket->i16SamplesZ[index] = barePacket.i16At(addr + 4);
    }
    parsedAppPalMotPacket->u8SampleCount = index + 1;

    return true;
}
