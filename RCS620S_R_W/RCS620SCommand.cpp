#include <inttypes.h>
#include "RCS620SCommand.h"

void RCS620SCommand::CreateDataReadCommand(RWCommand cmd, const uint8_t *IDm, uint8_t serviceSize, ServiceCode sCode, uint8_t blockSize, RWBlock block)
{
    uint8_t c[] = {
        (uint8_t)cmd,
        IDm[0], IDm[1], IDm[2], IDm[3], IDm[4], IDm[5], IDm[6], IDm[7],
        (uint8_t)serviceSize,
        (uint8_t)sCode, 0x00,
        (uint8_t)blockSize,
        (uint8_t)0x80, (uint8_t)block
    };

    cmdList = new uint8_t[sizeof(c)];
    for (int i = 0; i < sizeof(c); i++) {
        cmdList[i] = c[i];
    }
}