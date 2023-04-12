/**
 * @file   AppUartAsciiCommandSerializer.h
 * @brief  App_Uart (Mode A) command serializer for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPUARTASCIICOMMANDSERIALIZER_H
#define APPUARTASCIICOMMANDSERIALIZER_H

#include "MWings_Common.h"

/**
 * @struct AppUartAsciiCommand
 * @brief   command content for App_Uart (Mode A)
 */
struct AppUartAsciiCommand final : public mwings::CommandBase {
    uint8_t u8CommandId;
    uint8_t* u8Data;
    uint16_t u16DataSize;

    AppUartAsciiCommand() : u8CommandId(0x78), u8Data(nullptr), u16DataSize(0)
        {}
    // Check if the command is valid for App_Uart (Mode A)
    inline bool isValid() const override {
        return (((0x00 <= u8DestinationLogicalId and u8DestinationLogicalId <= 0x64)
                 or u8DestinationLogicalId == 0x78)
                and u8CommandId < 0x80
                and (not (u8Data == nullptr))
                and (1 <= u16DataSize and u16DataSize <= 640));
    }
};

/**
 * @class appuartascii::CommandSerializer
 * @brief   command serializer for App_Uart (Mode A)
 */
namespace appuartascii {
class CommandSerializer final : public mwings::CommandSerializerBase {
public:
    // Serialize command
    bool serialize(const mwings::CommandBase* const command,
                   uint8_t* const payload, const int payloadSize,
                   uint8_t* const checksum) const override;
};
}

extern appuartascii::CommandSerializer AppUartAsciiCommandSerializer;

#endif  // APPUARTASCIICOMMANDSERIALIZER_H
