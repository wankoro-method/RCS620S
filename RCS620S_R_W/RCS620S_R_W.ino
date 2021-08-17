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
  GetMAC_A();
  MAC_AValueCalc();

  /*
  uint8_t data[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
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
      //RCValue[i] = random(0, 256);
      RCValue[i] = 0x00;
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

void MAC_AValueCalc()
{
  uint8_t RC1[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  uint8_t RC2[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  uint8_t CK1[] = { 1, 1, 1, 1, 1, 1, 1, 1 };
  uint8_t CK2[] = { 1, 1, 1, 1, 1, 1, 1, 1 };

  uint8_t SK1[8], SK2[8];

  //CK(key)とIVの初期化
  des.init(CK1, (unsigned long long int)0);

  //暗号化
  des.tdesCbcEncipher(RC1,SK1);

  Serial.print("Arduino SK1 = ");
  for(int i = 0; i < sizeof(SK1); i++){
    Serial.print(SK1[i], HEX);
  }
}
