/**
 * @file   AppUartAsciiCommandSerializer.cpp
 * @brief  App_Uart (Mode A) command serializer for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppUartAsciiCommandSerializer.h"

appuartascii::CommandSerializer AppUartAsciiCommandSerializer;

bool appuartascii::CommandSerializer::serialize(const mwings::CommandBase* const command,
                                                uint8_t* const payload, const int maxPayloadSize,
                                                uint8_t* const checksum) const
{
    // WARNING: Note that there is NO RTTI
    const AppUartAsciiCommand* const appUartAsciiCommand = static_cast<const AppUartAsciiCommand*>(command);

    const int flexiblePayloadSize = appUartAsciiCommand->u16DataSize + 2;

    if (not (maxPayloadSize >= flexiblePayloadSize)) { return false; }
    if (not appUartAsciiCommand->isValid()) { return false; }

    payload[0] = (appUartAsciiCommand->u8DestinationLogicalId <= 0x64)
        ? appUartAsciiCommand->u8DestinationLogicalId : 0x78;
    payload[1] = appUartAsciiCommand->u8CommandId;
    for (int i = 0; i < appUartAsciiCommand->u16DataSize; i++) {
        payload[2+i] = appUartAsciiCommand->u8Data[i];
    }

    *(checksum) = 0;
    for (int i = 0; i < flexiblePayloadSize; i++) {
        *(checksum) += payload[i];
    }
    *(checksum) = ~(*(checksum)) + 1;

    return true;
}
