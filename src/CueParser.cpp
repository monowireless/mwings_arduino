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

    parsedCuePacket->u32SourceSerialId = barePacket.u32At(7);
    parsedCuePacket->u8SourceLogicalId = barePacket.u8At(11);
    parsedCuePacket->u16SequenceNumber = barePacket.u16At(5);
    parsedCuePacket->u8Lqi = barePacket.u8At(4);
    parsedCuePacket->u16SupplyVoltage = barePacket.u16At(34);

    const uint8_t accelEventSource = barePacket.u8At(24);
    const uint8_t accelEventId = barePacket.u8At(26);
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
        parsedCuePacket->i16SamplesX[index] = barePacket.i16At(addr + 0);
        parsedCuePacket->i16SamplesY[index] = barePacket.i16At(addr + 2);
        parsedCuePacket->i16SamplesZ[index] = barePacket.i16At(addr + 4);
    }
    parsedCuePacket->u8SampleCount = index + 1;

    const uint8_t rawMagnetState = barePacket.u8At(46);
    parsedCuePacket->u8MagnetState = rawMagnetState & 0x0F;
    parsedCuePacket->bMagnetStateChanged = (rawMagnetState & 0x80) ? false : true;

    return true;
}
