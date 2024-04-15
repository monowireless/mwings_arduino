/**
 * @file   ActPacketParser.cpp
 * @brief  Act packet parser for MWings.
 *
 * Copyright (C) 2024 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "ActPacketParser.h"

act::PacketParser ActPacketParser;

bool act::PacketParser::parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedActPacket* const parsedActPacket = static_cast<ParsedActPacket*>(parsedPacket);

    parsedActPacket->u32SourceSerialId = barePacket.u32At(3);
    parsedActPacket->u8SourceLogicalId = barePacket.u8At(0);
    parsedActPacket->u16SequenceNumber = 0x0000;
    parsedActPacket->u8Lqi = barePacket.u8At(11);
    parsedActPacket->u16SupplyVoltage = 0x0000;

    parsedActPacket->u8CommandId = barePacket.u8At(2);
    parsedActPacket->u8Data = barePacket.u8From(14);
    parsedActPacket->u16DataSize = barePacket.u16PayloadSize - 14;

    return true;
}
