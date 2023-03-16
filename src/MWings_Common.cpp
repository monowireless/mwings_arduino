/**
 * @file   MWings_Common.cpp
 * @brief  Mono Wireless TWELITE Wings API Common Definitions
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "MWings_Common.h"

#if 0
using namespace mwings_common;

template<>
uint8_t mwings_common::BarePacket::at_u8(int index) const {
    return payload[index];
}

template<>
uint16_t mwings_common::BarePacket::at_u16(int index) const {
    return (payload[index] << 8) | payload[index+1];
}

template<>
uint32_t mwings_common::BarePacket::at_u32(int index) const {
    return
        (payload[index+0] << 24) |
        (payload[index+1] << 16) |
        (payload[index+2] << 8) |
        (payload[index+3] << 0);
}
#endif