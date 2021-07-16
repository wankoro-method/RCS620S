#include <inttypes.h>
#include "RCS620SCommand.h"

RCS620SCommand::~RCS620SCommand()
{
    delete[] cmdList;
}

void RCS620SCommand::CreateDataReadCommand(RWCommand cmd, const uint8_t *IDm, uint8_t serviceSize, ServiceCode sCode, uint8_t blockSize, RWBlock block)
{
    uint8_t cache[] = {
        (uint8_t)cmd,
        IDm[0], IDm[1], IDm[2], IDm[3], IDm[4], IDm[5], IDm[6], IDm[7],
        (uint8_t)serviceSize,
        (uint8_t)sCode, 0x00,
        (uint8_t)blockSize,
        (uint8_t)0x80, (uint8_t)block
    };

    cmdList = new uint8_t[sizeof(cache)];
    for (int i = 0; i < sizeof(cache); i++) {
        cmdList[i] = cache[i];
    }
}

void RCS620SCommand::CreateDataSendCommand(RWCommand cmd, const uint8_t * IDm, uint8_t serviceSize, ServiceCode sCode, uint8_t blockSize, RWBlock block, const uint8_t * data, uint16_t dataLen)
{
    uint8_t cache[] = {
        (uint8_t)cmd,
        IDm[0], IDm[1], IDm[2], IDm[3], IDm[4], IDm[5], IDm[6], IDm[7],
        (uint8_t)serviceSize,
        (uint8_t)sCode, 0x00,
        (uint8_t)blockSize,
        (uint8_t)0x80, (uint8_t)block,
        data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15], 
    };

    cmdList = new uint8_t[sizeof(cache)];
    for (int i = 0; i < sizeof(cache); i++) {
        cmdList[i] = cache[i];
    }
}