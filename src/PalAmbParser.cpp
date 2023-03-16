/**
 * @file   PalAmbParser.cpp
 * @brief  App_PAL (AMB) parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "PalAmbParser.h"

palamb::Parser PalAmbParser;

bool palamb::Parser::parse(const mwings_common::BarePacket& barePacket, mwings_common::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedPalAmbPacket* const parsedPalAmbPacket = static_cast<ParsedPalAmbPacket*>(parsedPacket);

    parsedPalAmbPacket->u32SourceSerialId = barePacket.at_u32(7);
    parsedPalAmbPacket->u8SourceLogicalId = barePacket.at_u8(11);
   	parsedPalAmbPacket->u16SequenceNumber = barePacket.at_u16(5);
    parsedPalAmbPacket->u8Lqi = barePacket.at_u8(4);
    parsedPalAmbPacket->u16SupplyVoltage = barePacket.at_u16(19);

    parsedPalAmbPacket->i16Temp100x = static_cast<int16_t>(barePacket.at_u16(31));
    parsedPalAmbPacket->u16Humid100x = barePacket.at_u16(37);
    parsedPalAmbPacket->u32Luminance = barePacket.at_u32(43);

    return true;
}
