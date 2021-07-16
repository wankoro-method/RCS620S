#include <inttypes.h>
#include <string.h>
#include "RCS620SCommand.h"

RCS620SCommand::RCS620SCommand()
{
    cmdList = nullptr;
    cmdListLen = 0;
}

RCS620SCommand::~RCS620SCommand()
{
    delete[] cmdList;
}

void RCS620SCommand::CreateDataWriteCommand(const uint8_t * IDm, uint8_t serviceSize, ServiceCode sCode, uint8_t blockSize, RWBlock block, const uint8_t * data, uint16_t dataLen)
{
    if (cmdList != nullptr) { delete[] cmdList; }

    uint8_t cache[] = {
        RWCommand::WriteWithoutEncryption,
        IDm[0], IDm[1], IDm[2], IDm[3], IDm[4], IDm[5], IDm[6], IDm[7],
        serviceSize,
        sCode, 0x00,
        blockSize,
        0x80, block,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    for (int i = 0; i < dataLen; i++) {
        cache[default_Cmd_Length + i] = data[i];
    }

    cmdListLen = sizeof(cache);
    cmdList = new uint8_t[cmdListLen];
    memcpy(cmdList, cache, cmdListLen);
}

void RCS620SCommand::CreateDataReadCommand(const uint8_t *IDm, uint8_t serviceSize, ServiceCode sCode, uint8_t blockSize, RWBlock block)
{
    if(cmdList != nullptr){ delete[] cmdList; }

    uint8_t cache[] = {
        RWCommand::ReadWithoutEncryption,
        IDm[0], IDm[1], IDm[2], IDm[3], IDm[4], IDm[5], IDm[6], IDm[7],
        serviceSize,
        sCode, 0x00,
        blockSize,
        0x80, block
    };

    cmdListLen = default_Cmd_Length;

    cmdList = new uint8_t[cmdListLen];
    memcpy(cmdList, cache, cmdListLen);
}