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

    parsedAriaPacket->u32SourceSerialId = barePacket.at_u32(7);
    parsedAriaPacket->u8SourceLogicalId = barePacket.at_u8(11);
   	parsedAriaPacket->u16SequenceNumber = barePacket.at_u16(5);
    parsedAriaPacket->u8Lqi = barePacket.at_u8(4);
    parsedAriaPacket->u16SupplyVoltage = barePacket.at_u16(34);

    parsedAriaPacket->i16Temp100x = static_cast<int16_t>(barePacket.at_u16(51));
    parsedAriaPacket->u16Humid100x = barePacket.at_u16(57);

    const uint8_t rawMagnetState = barePacket.at_u8(46);
    parsedAriaPacket->u8MagnetState = rawMagnetState & 0x0F;
    parsedAriaPacket->bMagnetStateChanged = (rawMagnetState & 0x80) ? false : true;

    return true;
}
