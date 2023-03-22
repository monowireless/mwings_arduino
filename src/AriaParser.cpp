/**
 * @file   AriaParser.cpp
 * @brief  App_ARIA (ARIA mode) parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AriaParser.h"

aria::Parser AriaParser;

bool aria::Parser::parse(const mwings_common::BarePacket& barePacket, mwings_common::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedAriaPacket* const parsedAriaPacket = static_cast<ParsedAriaPacket*>(parsedPacket);

    parsedAriaPacket->u32SourceSerialId = barePacket.u32At(7);
    parsedAriaPacket->u8SourceLogicalId = barePacket.u8At(11);
    parsedAriaPacket->u16SequenceNumber = barePacket.u16At(5);
    parsedAriaPacket->u8Lqi = barePacket.u8At(4);
    parsedAriaPacket->u16SupplyVoltage = barePacket.u16At(34);

    parsedAriaPacket->i16Temp100x = barePacket.i16At(51);
    parsedAriaPacket->u16Humid100x = barePacket.u16At(57);

    const uint8_t rawMagnetState = barePacket.u8At(46);
    parsedAriaPacket->u8MagnetState = rawMagnetState & 0x0F;
    parsedAriaPacket->bMagnetStateChanged = (rawMagnetState & 0x80) ? false : true;

    return true;
}
