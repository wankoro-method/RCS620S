//Copyright 2021 わんころメソッド();

#include "RCS620S.h"
#include "RCS620SCommand.h"

#include <string.h>

#include "DES.h"

//Wait time
const unsigned int IO_wait_time_ms = 250;

//RC-S620S Device class
RCS620S rc_s620s;
RCS620SCommand rc_s620s_cmd;

//DES class
DES des;

//Function
bool Pooling();
void ShowIDm();
void ShowPMm();
void ShowData();
void GetMAC_A();
void MAC_AValueCalc();

//レスポンスデータキャッチ用
uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];
uint8_t responseLen;

//MAC_Aデータレスポンスキャッチ用
uint8_t RCValue[16];
uint8_t ResMAC_A[8];

//前フレームIDmデータ比較用
bool executeFlag = false;
uint8_t beforeFrameIDm[8] = { 0 };

//Initialize
void setup() {
  pinMode(2, OUTPUT);
  Serial1.begin(115200);
  Serial.begin(115200);
  rc_s620s.initDevice();
}

void loop() {

  if(Pooling() != true) { return; }
  ShowData();

  
  uint8_t data[] = { 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2 };
  rc_s620s_cmd.CreateDataWriteCommand(rc_s620s.idm, 1, ServiceCode::RWAccess, 1, RWBlock::CK, data, sizeof(data));

  if(rc_s620s.cardCommand(rc_s620s_cmd.cmdList, rc_s620s_cmd.cmdListLen, response, &responseLen) == 1){
    for (int i = 0; i < responseLen; i++) {
      Serial.print(response[i], HEX);
      Serial.print(":");
    }
    Serial.println("");
  }
  else{
    Serial.println("DataErr0r");
  }

  /*
  rc_s620s_cmd.CreateDataReadCommand(rc_s620s.idm, 0x01, ServiceCode::ROAccess, 0x01, RWBlock::S_PAD13);
  
  if(rc_s620s.cardCommand(rc_s620s_cmd.cmdList, rc_s620s_cmd.cmdListLen, response, &responseLen) == 1){
    for (int i = 0; i < responseLen; i++) {
      Serial.print(response[i], HEX);
      Serial.print(":");
    }
    Serial.println("");
  }
  else{
    Serial.println("DataErr0r");
  }
  */

  GetMAC_A();
  MAC_AValueCalc();

  //切断処理
  Serial.println("");
  rc_s620s.rfOff();
  delay(IO_wait_time_ms);
}

bool Pooling()
{
  for(int i = 0; i < 8; i++){
    beforeFrameIDm[i] = rc_s620s.idm[i];
  }

  if (!rc_s620s.polling(SystemCode::code_wild_card)) {
    rc_s620s.rfOff();
    digitalWrite(2, LOW);
    delay(IO_wait_time_ms);
    return false; 
  }

  if(memcmp(beforeFrameIDm, rc_s620s.idm, 8) == 0){
    digitalWrite(2, HIGH);
    rc_s620s.rfOff();
    delay(IO_wait_time_ms);
    return false; 
  }

  return true;
}

void ShowIDm()
{
  Serial.print("IDm : ");
  for (int i = 0; i < sizeof(rc_s620s.idm); i++) {
    char buf[2];
    sprintf(buf, "%02X", rc_s620s.idm[i]);
    Serial.print(buf);

    if(i != sizeof(rc_s620s.idm) - 1){ Serial.print(":"); }
  }
  
  Serial.println("");
}

void ShowPMm()
{
  Serial.print("PMm : ");
  for (int i = 0; i < sizeof(rc_s620s.pmm); i++) {
    char buf[2];
    sprintf(buf, "%02X", rc_s620s.pmm[i]);
    Serial.print(buf);
  }
    
  Serial.println("");
}

void ShowData()
{
  ShowIDm();
  ShowPMm();
  digitalWrite(2, HIGH);
}

void GetMAC_A()
{
  //1st step authentication
  {
    //ランダムチャレンジ用乱数生成
    randomSeed(analogRead(A1));

    Serial.print("RCData : ");
    for(int i = 0; i < 16; i++){
      RCValue[i] = random(0, 256);
      //RCValue[i] = 0;
      Serial.print(RCValue[i], HEX);
      Serial.print(":");
    }
    Serial.println("");
    //ランダムチャレンジブロック書き込み
    rc_s620s_cmd.CreateDataWriteCommand(rc_s620s.idm, 1, ServiceCode::RWAccess, 1, RWBlock::RC, RCValue, sizeof(RCValue));

    //レスポンス確認
    if(rc_s620s.cardCommand(rc_s620s_cmd.cmdList, rc_s620s_cmd.cmdListLen, response, &responseLen) == 1){
      Serial.print("RC OK : ");
      for (int i = 0; i < responseLen; i++) {
        Serial.print(response[i], HEX);
        Serial.print(":");
      }
      Serial.println("");
    }
    else{
      Serial.println("DataError");
    }
  }

  //2nd step authentication
  {
    //Read ID, CKV, MAC_A
    uint8_t RWEcmd[] = {
        RWCommand::ReadWithoutEncryption,
        rc_s620s.idm[0], rc_s620s.idm[1], rc_s620s.idm[2], rc_s620s.idm[3], rc_s620s.idm[4], rc_s620s.idm[5], rc_s620s.idm[6], rc_s620s.idm[7],
        0x01,
        ServiceCode::ROAccess, 0x00,
        0x03,
        0x80, RWBlock::ID,
        0x80, RWBlock::CKV,
        0x80, RWBlock::MAC_A,
    };

    if(rc_s620s.cardCommand(RWEcmd, sizeof(RWEcmd), response, &responseLen) == 1){
      Serial.print("MAC_A Respoce Success : ");
      for (int i = 0; i < responseLen; i++) {
        Serial.print(response[i], HEX);
        Serial.print(":");
      }
      Serial.println("");
    }
    else{
      Serial.println("DataErr0r");
    }

    Serial.print("MAC_A : ");
    for(int i = 0; i < 8; i++){
      ResMAC_A[i] = response[44 + i];
      Serial.print(ResMAC_A[i], HEX);
      Serial.print(":");
    }
    Serial.println("");
  }
}

//バイトオーダー反転
void swapByteOrder(uint8_t data[])
{
  uint8_t swap[8];
  for (int i = 0; i < 8; i++) {
    swap[7 - i] = data[i];
  }

  for (int i = 0; i < 8; i++) {
    data[i] = swap[i];
  }
}

void TwoKey_T_DES(const uint8_t *plainData, uint8_t *encData, const uint8_t *key1, const uint8_t *key2)
{
  uint8_t encDataCache_1[8], encDataCache_2[8];
  des.encrypt( encDataCache_1,       plainData,  key1);
  des.decrypt( encDataCache_2,  encDataCache_1,  key2);
  des.encrypt(        encData,  encDataCache_2,  key1);
}

void MAC_AValueCalc()
{
  //ランダムチャレンジ
  //uint8_t RC1[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  //uint8_t RC2[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  uint8_t RC1[8], RC2[8];
  for(int i = 0; i < 8; i++){
    RC1[i] = RCValue[i];
    RC2[i] = RCValue[i + 8];
  }

  //カードキー
  uint8_t CK1[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
  uint8_t CK2[8] = { 2, 2, 2, 2, 2, 2, 2, 2 };

  //セッションキー
  uint8_t SK1[8], SK2[8];

  /*******************************
       Generate session key 1     
  ********************************/
  swapByteOrder(RC1);
  TwoKey_T_DES(RC1, SK1, CK1, CK2);
  Serial.print("SK1 = ");
  for(int i = 7; i >= 0; i--){
    Serial.print(SK1[i], HEX);
    Serial.print(":");
  }
  Serial.println("");
  
  /*******************************
       Generate session key 2     
  ********************************/
 swapByteOrder(RC2);
  for(int i = 0; i < 8; i++){
    RC2[i] ^= SK1[i];
  }
  
  //TwoKey_T_DES(RC2, SK2, CK1, CK2);
  TwoKey_T_DES(RC2, SK2, CK1, CK2);
  Serial.print("SK2 = ");
  for(int i = 7; i >= 0; i--){
    Serial.print(SK2[i], HEX);
    Serial.print(":");
  }
  Serial.println("");
  
  /*******************************
           Generate MAC_A
  ********************************/
  
  uint8_t encData1[8], encData2[8];
  uint8_t ID1[]  = { response[12], response[13], response[14], response[15], response[16], response[17], response[18], response[19] }, ID2[]  = { response[20], response[21], response[22], response[23], response[24], response[25], response[26], response[27] };
  uint8_t CKV1[] = { response[28], response[29], response[30], response[31], response[32], response[33], response[34], response[35] }, CKV2[] = { response[36], response[37], response[38], response[39], response[40], response[41], response[42], response[43] };
  uint8_t firstData[] = { RWBlock::ID, 0x00, RWBlock::CKV, 0x00, 0xFF, 0xFF, 0xFF, 0xFF };

  uint8_t enc1[8], enc2[8], enc3[8], enc4[8], maca[8];
  
  //１段目
  swapByteOrder(firstData);
  for(int i = 0; i < 8; i++){
    firstData[i] ^= RC1[i];
  }
  TwoKey_T_DES(firstData, enc1, SK1, SK2);
  
  //２段目
  swapByteOrder(ID1);
  for(int i = 0; i < 8; i++){
    enc1[i] ^= ID1[i];
  }
  TwoKey_T_DES(encData1, enc2, SK1, SK2);
  
  //３段目
  swapByteOrder(ID2);
  for(int i = 0; i < 8; i++){
    enc2[i] ^= ID2[i];
  }
  TwoKey_T_DES(enc2, enc3, SK1, SK2);
  
  //４段目
  swapByteOrder(CKV1);
  for(int i = 0; i < 8; i++){
    enc3[i] ^= CKV1[i];
  }
  TwoKey_T_DES(enc3, enc4, SK1, SK2);
  
  //MAC_A生成
  swapByteOrder(CKV2);
  for(int i = 0; i < 8; i++){
    enc4[i] ^= CKV2[i];
  }
  TwoKey_T_DES(enc4, maca, SK1, SK2);
  
  Serial.print("Arduino MAC_A : ");
  for(int i = 7; i >= 0; i--){
    Serial.print(maca[i], HEX);
    Serial.print(":");
  }
  Serial.println("");
}
