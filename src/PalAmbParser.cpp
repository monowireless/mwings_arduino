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

    parsedPalAmbPacket->u32SourceSerialId = barePacket.u32At(7);
    parsedPalAmbPacket->u8SourceLogicalId = barePacket.u8At(11);
    parsedPalAmbPacket->u16SequenceNumber = barePacket.u16At(5);
    parsedPalAmbPacket->u8Lqi = barePacket.u8At(4);
    parsedPalAmbPacket->u16SupplyVoltage = barePacket.u16At(19);

    parsedPalAmbPacket->i16Temp100x = barePacket.i16At(31);
    parsedPalAmbPacket->u16Humid100x = barePacket.u16At(37);
    parsedPalAmbPacket->u32Luminance = barePacket.u32At(43);

    return true;
}
