#include "CryptoProcess.h"

void CryptoProcess::InitVariable()
{
    for(int i = 0; i < 8; i++){
        RC1[i] = 0; RC2[i] = 0;
        CK1[i] = 0; CK2[i] = 0;
        SK1[i] = 0; SK2[i] = 0;
    }
}

void CryptoProcess::swapByteOrder(uint8_t * data)
{
  uint8_t swap[8];
  for (int i = 0; i < 8; i++) {
    swap[7 - i] = data[i];
  }

  for (int i = 0; i < 8; i++) {
    data[i] = swap[i];
  }
}

void CryptoProcess::TwoKeyTripleDES(const uint8_t * plainData, uint8_t * encData, const uint8_t * key1, const uint8_t * key2)
{
    uint8_t encDataCache_1[8] = { 0 }, encDataCache_2[8] = { 0 };
    des.encrypt( encDataCache_1,       plainData,  key1);
    des.decrypt( encDataCache_2,  encDataCache_1,  key2);
    des.encrypt(        encData,  encDataCache_2,  key1);
}

void CryptoProcess::CalcMAC(const uint8_t *RCdata, const uint8_t *CKdata, const uint8_t *ResponceData, uint8_t *MACdata)
{
    //メンバ変数初期化
    InitVariable();

    //CKコピー
    for(int i = 0; i < 8; i++){
        CK1[i] = CKdata[i];
        CK2[i] = CKdata[i + 8];
    }

    //RCコピー
    for(int i = 0; i < 8; i++) {
        RC1[i] = RCdata[i];
        RC2[i] = RCdata[i + 8];
    }

    //SK1生成
    swapByteOrder(CK1);
    swapByteOrder(CK2);
    swapByteOrder(RC1);
    TwoKeyTripleDES(RC1, SK1, CK1, CK2);

    //SK2生成
    swapByteOrder(RC2);
    for(int i = 0; i < 8; i++){
        RC2[i] ^= SK1[i];
    }
    TwoKeyTripleDES(RC2, SK2, CK1, CK2);

    //暗号化用データのコピー
    uint8_t ID1[8], ID2[8], CKV1[8], CKV2[8];
    for(int i = 0; i < 8; i++){
        ID1[i]  = ResponceData[i + 12];
        ID2[i]  = ResponceData[i + 20];
        CKV1[i] = ResponceData[i + 28];
        CKV2[i] = ResponceData[i + 36];
    }

    //エンコードデータ格納用変数
    uint8_t enc1[8], enc2[8], enc3[8];

    //暗号化1段目
    swapByteOrder(ID1);
    for(int i = 0; i < 8; i++){
      ID1[i] ^= RC1[i];
    }
    TwoKeyTripleDES(ID1, enc1, SK1, SK2);
    
    //暗号化2段目
    swapByteOrder(ID2);
    for(int i = 0; i < 8; i++){
      ID2[i] ^= enc1[i];;
    }
    TwoKeyTripleDES(ID2, enc2, SK1, SK2);
    
    //暗号化3段目
    swapByteOrder(CKV1);
    for(int i = 0; i < 8; i++){
      CKV1[i] ^= enc2[i];
    }
    TwoKeyTripleDES(CKV1, enc3, SK1, SK2);
    
    //MAC生成
    swapByteOrder(CKV2);
    for(int i = 0; i < 8; i++){
      CKV2[i] ^= enc3[i];
    }
    TwoKeyTripleDES(enc3, MACdata, SK1, SK2);
    swapByteOrder(MACdata);
}

void CryptoProcess::CalcMAC_A(const uint8_t *RCdata, const uint8_t *CKdata, const uint8_t *ResponceData, uint8_t *MAC_Adata)
{
    InitVariable();
}