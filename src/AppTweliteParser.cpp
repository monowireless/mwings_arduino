/**
 * @file   AppTweliteParser.cpp
 * @brief  App_Twelite parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppTweliteParser.h"

apptwelite::Parser AppTweliteParser;

bool apptwelite::Parser::parse(const mwings_common::BarePacket& barePacket, mwings_common::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedAppTwelitePacket* const parsedAppTwelitePacket = static_cast<ParsedAppTwelitePacket*>(parsedPacket);

    parsedAppTwelitePacket->u32SourceSerialId = barePacket.u32At(5);
    parsedAppTwelitePacket->u8SourceLogicalId = barePacket.u8At(9);
    parsedAppTwelitePacket->u16SequenceNumber = barePacket.u16At(10); // Technically, it's timestamp
    parsedAppTwelitePacket->u8Lqi = barePacket.u8At(4);
    parsedAppTwelitePacket->u16SupplyVoltage = barePacket.u16At(13);

    parsedAppTwelitePacket->u8RelayCount = barePacket.u8At(12);
    const uint8_t u8DIState = barePacket.u8At(16);
    parsedAppTwelitePacket->bPeriodic = ((u8DIState & 0x80) == 0x80);
    parsedAppTwelitePacket->bDiState[0] = (u8DIState & (1 << 0));
    parsedAppTwelitePacket->bDiState[1] = (u8DIState & (1 << 1));
    parsedAppTwelitePacket->bDiState[2] = (u8DIState & (1 << 2));
    parsedAppTwelitePacket->bDiState[3] = (u8DIState & (1 << 3));
    const uint8_t u8DIChanged = barePacket.u8At(17);
    parsedAppTwelitePacket->bDiChanged[0] = (u8DIChanged & (1 << 0));
    parsedAppTwelitePacket->bDiChanged[1] = (u8DIChanged & (1 << 1));
    parsedAppTwelitePacket->bDiChanged[2] = (u8DIChanged & (1 << 2));
    parsedAppTwelitePacket->bDiChanged[3] = (u8DIChanged & (1 << 3));

    uint8_t u8AiValue[4];
    for (int i = 0; i < 4; i++) {
        u8AiValue[i] = barePacket.u8At(18+i);
    }
    const uint8_t u8AiFurtherValue = barePacket.u8At(22);
    for (int i = 0; i < 4; i++) {
        parsedAppTwelitePacket->u16AiVoltage[i] = u8AiValue[i] * 4 + ((u8AiFurtherValue >> (i*2)) & 0x03);
    }

    return true;
}
