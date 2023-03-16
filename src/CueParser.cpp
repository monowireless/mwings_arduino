/**
 * @file   CueParser.cpp
 * @brief  App_CUE (CUE mode) parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "CueParser.h"

cue::Parser CueParser;

bool cue::Parser::parse(const mwings_common::BarePacket& barePacket, mwings_common::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedCuePacket* const parsedCuePacket = static_cast<ParsedCuePacket*>(parsedPacket);

    parsedCuePacket->u32SourceSerialId = barePacket.at_u32(7);
    parsedCuePacket->u8SourceLogicalId = barePacket.at_u8(11);
   	parsedCuePacket->u16SequenceNumber = barePacket.at_u16(5);
    parsedCuePacket->u8Lqi = barePacket.at_u8(4);
    parsedCuePacket->u16SupplyVoltage = barePacket.at_u16(34);

    const uint8_t accelEventSource = barePacket.at_u8(24);
    const uint8_t accelEventId = barePacket.at_u8(26);
    if (accelEventSource == 0x04) {
        parsedCuePacket->bHasAccelEvent = true;
        parsedCuePacket->u8AccelEvent = accelEventId;
    } else {
        parsedCuePacket->bHasAccelEvent = false;
        parsedCuePacket->u8AccelEvent = 0xFF;
    }

    int index;
    uint16_t addr;
    for (index = 0, addr = 0x33; index < 10; index++, addr += 10) {
        parsedCuePacket->i16SamplesX[index] = static_cast<int16_t>(barePacket.at_u16(addr + 0));
        parsedCuePacket->i16SamplesY[index] = static_cast<int16_t>(barePacket.at_u16(addr + 2));
        parsedCuePacket->i16SamplesZ[index] = static_cast<int16_t>(barePacket.at_u16(addr + 4));
    }
    parsedCuePacket->u8SampleCount = index + 1;

    const uint8_t rawMagnetState = barePacket.at_u8(46);
    parsedCuePacket->u8MagnetState = rawMagnetState & 0x0F;
    parsedCuePacket->bMagnetStateChanged = (rawMagnetState & 0x80) ? false : true;

    return true;
}
