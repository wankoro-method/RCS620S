# 個別化カード鍵書き込みサンプル
```cpp
//Copyright 2021 わんころメソッド();

#include "RCS620S.h"
#include "RCS620SCommand.h"
#include "CryptoProcess.h"

//Wait time
const unsigned int IO_wait_time_ms = 250;

//RC-S620S Device class
RCS620S rc_s620s;
RCS620SCommand rc_s620s_cmd;

CryptoProcess crypto;

//Function
bool Pooling();
void ShowIDm();
void ShowPMm();
void ShowData();
void GetMAC_A();
void MACValueCalc();
bool Authentication();

//レスポンスデータキャッチ用
uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];
uint8_t responseLen;

//MACデータレスポンスキャッチ用
uint8_t RCValue[16];
uint8_t ResMAC[8];

//マスターキー <※任意の鍵を入力してください※>
const uint8_t masterKey[] = { /*24ブロックのマスターキー*/ };

//MAC比較用
uint8_t GenMAC[8];

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
  MACValueCalc();

  if(Authentication()){
    Serial.println("Card Verified");
  }
  else{
    Serial.println("Unauthorized cards");
  }

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
    for(int i = 0; i < 16; i++){
      RCValue[i] = random(0, 256);
    }
    //ランダムチャレンジブロック書き込み
    rc_s620s_cmd.CreateDataWriteCommand(rc_s620s.idm, 1, ServiceCode::RWAccess, 1, RWBlock::RC, RCValue, sizeof(RCValue));

    //レスポンス確認
    if(rc_s620s.cardCommand(rc_s620s_cmd.cmdList, rc_s620s_cmd.cmdListLen, response, &responseLen) != 1){
      Serial.println("DataError");
      return;
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
        0x80, RWBlock::MAC
    };

    if(rc_s620s.cardCommand(RWEcmd, sizeof(RWEcmd), response, &responseLen) != 1){
      Serial.println("DataErr0r");
    }

    for(int i = 0; i < 8; i++){
      ResMAC[i] = response[44 + i];
    }
  }
}

void MACValueCalc()
{
  uint8_t cID[] = { rc_s620s.idm[0], rc_s620s.idm[1], rc_s620s.idm[2], rc_s620s.idm[3], rc_s620s.idm[4], rc_s620s.idm[5], rc_s620s.idm[6], rc_s620s.idm[7], 0, 0, 0, 0, 0, 0, 0, 0};
  //カードキー
  uint8_t CK1[16];
  crypto.GenerateCardKey(cID, masterKey, CK1);
  crypto.CalcMAC(RCValue, CK1, response, GenMAC);
}

bool Authentication()
{
  for(int i = 0; i < 8; i++){
    if(ResMAC[i] != GenMAC[i]){ return false; }
  }

  return true;
}
```

# 個別化カード鍵算出サンプル
```cpp
//Copyright 2021 わんころメソッド();

#include "RCS620S.h"
#include "RCS620SCommand.h"
#include "CryptoProcess.h"

//Wait time
const unsigned int IO_wait_time_ms = 250;

//RC-S620S Device class
RCS620S rc_s620s;
RCS620SCommand rc_s620s_cmd;

CryptoProcess crypto;

//Function
bool Pooling();
void ShowIDm();
void ShowPMm();
void ShowData();
void GetMAC_A();
void MACValueCalc();
bool Authentication();

//レスポンスデータキャッチ用
uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];
uint8_t responseLen;

//MACデータレスポンスキャッチ用
uint8_t RCValue[16];
uint8_t ResMAC[8];

//MAC比較用
const uint8_t masterKey[] = { /*24ブロックのマスターキー*/ };
uint8_t GenMAC[8];

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

  uint8_t writeIDdata[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  rc_s620s_cmd.CreateDataWriteCommand(rc_s620s.idm, 1, ServiceCode::RWAccess, 1, RWBlock::ID, writeIDdata, sizeof(writeIDdata));
  if(rc_s620s.cardCommand(rc_s620s_cmd.cmdList, rc_s620s_cmd.cmdListLen, response, &responseLen) != 1){
    Serial.println("DataErr0r");
  }

  rc_s620s_cmd.CreateDataWriteCommand(rc_s620s.idm, 1, ServiceCode::RWAccess, 1, RWBlock::CKV, writeIDdata, sizeof(writeIDdata));
  if(rc_s620s.cardCommand(rc_s620s_cmd.cmdList, rc_s620s_cmd.cmdListLen, response, &responseLen) != 1){
    Serial.println("DataErr0r");
  }

  uint8_t cID[] = { rc_s620s.idm[0], rc_s620s.idm[1], rc_s620s.idm[2], rc_s620s.idm[3], rc_s620s.idm[4], rc_s620s.idm[5], rc_s620s.idm[6], rc_s620s.idm[7], 0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t data[16];
  crypto.GenerateCardKey(cID, masterKey, data);

  rc_s620s_cmd.CreateDataWriteCommand(rc_s620s.idm, 1, ServiceCode::RWAccess, 1, RWBlock::CK, data, sizeof(data));
  if(rc_s620s.cardCommand(rc_s620s_cmd.cmdList, rc_s620s_cmd.cmdListLen, response, &responseLen) != 1){
    Serial.println("DataErr0r");
  }
  
  GetMAC_A();
  MACValueCalc();

  if(Authentication()){
    Serial.println("Card Verified");
  }
  else{
    Serial.println("Unauthorized cards");
  }

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
    for(int i = 0; i < 16; i++){
      RCValue[i] = random(0, 256);
    }
    //ランダムチャレンジブロック書き込み
    rc_s620s_cmd.CreateDataWriteCommand(rc_s620s.idm, 1, ServiceCode::RWAccess, 1, RWBlock::RC, RCValue, sizeof(RCValue));

    //レスポンス確認
    if(rc_s620s.cardCommand(rc_s620s_cmd.cmdList, rc_s620s_cmd.cmdListLen, response, &responseLen) != 1){
      Serial.println("DataError");
      return;
    }
  
  Serial.println("");
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
        0x80, RWBlock::MAC
    };

    if(rc_s620s.cardCommand(RWEcmd, sizeof(RWEcmd), response, &responseLen) != 1){
      Serial.println("DataErr0r");
    }

    for(int i = 0; i < 8; i++){
      ResMAC[i] = response[44 + i];
    }
  }
}

void MACValueCalc()
{
  uint8_t cID[] = { rc_s620s.idm[0], rc_s620s.idm[1], rc_s620s.idm[2], rc_s620s.idm[3], rc_s620s.idm[4], rc_s620s.idm[5], rc_s620s.idm[6], rc_s620s.idm[7], 0, 0, 0, 0, 0, 0, 0, 0};
  //カードキー
  uint8_t CK1[16];
  crypto.GenerateCardKey(cID, masterKey, CK1);
  crypto.CalcMAC(RCValue, CK1, response, GenMAC);
}

bool Authentication()
{
  for(int i = 0; i < 8; i++){
    if(ResMAC[i] != GenMAC[i]){ return false; }
  }

  return true;
}

```
