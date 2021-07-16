//Copyright 2021 わんころメソッド();

#include "RCS620S.h"
#include "RCS620SCommand.h"

#include <string.h>

//Wait time
const unsigned int IO_wait_time_ms = 250;

//RC-S620S Device class
RCS620S rc_s620s;
RCS620SCommand rc_s620s_cmd;

void ShowIDm();
void ShowPMm();
void ShowData();

//レスポンスデータキャッチ用
uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];
uint8_t responseLen;

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
  for(int i = 0; i < 8; i++){
    beforeFrameIDm[i] = rc_s620s.idm[i];
  }

  if (!rc_s620s.polling(SystemCode::code_wild_card)) {
    rc_s620s.rfOff();
    digitalWrite(2, LOW);
    delay(IO_wait_time_ms);
    return; 
  }

  if(memcmp(beforeFrameIDm, rc_s620s.idm, 8) == 0){
    digitalWrite(2, HIGH);
    rc_s620s.rfOff();
    delay(IO_wait_time_ms);
    return; 
  }

  ShowData();
  rc_s620s_cmd.CreateDataReadCommand(RWCommand::ReadWithoutEncryption, rc_s620s.idm, 0x01, ServiceCode::ROAccess, 0x01, RWBlock::S_PAD13);
  uint8_t data[] = { 0xE3, 0x81, 0x93, 0xE3, 0x82, 0x93, 0xE3, 0x81, 0xAB, 0xE3, 0x81, 0xA1, 0xE3, 0x82, 0x8F, 0x00 };
  //rc_s620s_cmd.CreateDataSendCommand(RWCommand::WriteWithoutEncryption, rc_s620s.idm, 1, ServiceCode::RWAccess, 1, RWBlock::S_PAD13, data, 16);

  if(rc_s620s.cardCommand(rc_s620s_cmd.cmdList, 31, response, &responseLen) == 1){
    for (int i = 0; i < responseLen; i++) {
      Serial.print(response[i], HEX);
      Serial.print(":");
    }
    Serial.println("");
  }
  else{
    Serial.println("DataErr0r");
  }

  //切断処理
  rc_s620s.rfOff();
  delay(IO_wait_time_ms);
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