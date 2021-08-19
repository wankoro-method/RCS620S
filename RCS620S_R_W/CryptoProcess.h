#include "DES.h"

class CryptoProcess
{
private:
    DES des;
    uint8_t RC1[8], RC2[8];
    uint8_t CK1[8], CK2[8];
    uint8_t SK1[8], SK2[8];

public:
    void CalcMAC(const uint8_t *RCdata, const uint8_t *CKdata, const uint8_t *ResponceData, uint8_t *MACdata);
    void CalcMAC_A(const uint8_t *RCdata, const uint8_t *CKdata, const uint8_t *ResponceData, uint8_t *MAC_Adata);

private:
    void InitVariable();
    void swapByteOrder(uint8_t *data);
    void TwoKeyTripleDES(const uint8_t *plainData, uint8_t *encData, const uint8_t *key1, const uint8_t *key2);
};