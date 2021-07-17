//Copyright 2021 わんころメソッド();

#include "RCS620S.h"
#include "RCS620SCommand.h"

#include <string.h>

const int LED = 2;                              //LED
const unsigned int IO_wait_time_ms = 250;       //Wait time

RCS620S rc_s620s;                               //RC-S620S Device class
RCS620SCommand rc_s620s_cmd;                    //RC-S620S Command class

uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];  //レスポンスデータキャッチ用
uint8_t responseLen;

bool executeFlag = false;                       //前フレームIDmデータ比較用
uint8_t beforeFrameIDm[8] = { 0 };

void ShowIDm();
void ShowPMm();
void ShowData();

//Initialize
void setup() {
  pinMode(2, OUTPUT);
  Serial1.begin(115200);
  Serial.begin(115200);
  rc_s620s.initDevice();
}

String data;

void loop() {
  for(int i = 0; i < 8; i++){
    beforeFrameIDm[i] = rc_s620s.idm[i];
  }

  if(!Serial.available()) { digitalWrite(LED, LOW); return; }

  digitalWrite(LED, HIGH);

  data = "";
  while(Serial.available()){
    char cache = Serial.read();
    data += cache;
  }

  if(data == "Pooling"){
    if (rc_s620s.polling(SystemCode::code_wild_card)) {
      ShowData();
      rc_s620s.rfOff();
    }
    else{
      Serial.println("NFC card is nothing!!!");
      rc_s620s.rfOff();
    }
  }

  if(data == "Read"){
    if (!rc_s620s.polling(SystemCode::code_wild_card)) {
      Serial.println("NFC card is nothing!!!");
    }

    rc_s620s_cmd.CreateDataReadCommand(rc_s620s.idm, 0x01, ServiceCode::ROAccess, 0x01, RWBlock::S_PAD13);

    if(rc_s620s.cardCommand(rc_s620s_cmd.cmdList, rc_s620s_cmd.cmdListLen, response, &responseLen) == 1){
      for (int i = 0; i < responseLen; i++) {
        Serial.print(response[i], HEX);

        if(i != responseLen - 1) { Serial.print(":"); }
      }
      Serial.println("");
    }
    else{
      Serial.println("Read error");
    }
  }

  if(data == "Write"){
    if (!rc_s620s.polling(SystemCode::code_wild_card)) {
      Serial.println("NFC card is nothing!!!");
    }
    
    uint8_t data[] = { 0x11, 0x45, 0x14, 0x81, 0x01, 0x91, 0x9F};
    rc_s620s_cmd.CreateDataWriteCommand(rc_s620s.idm, 1, ServiceCode::RWAccess, 1, RWBlock::S_PAD13, data, sizeof(data));

    if(rc_s620s.cardCommand(rc_s620s_cmd.cmdList, rc_s620s_cmd.cmdListLen, response, &responseLen) == 1){
      for (int i = 0; i < responseLen; i++) {
        Serial.print(response[i], HEX);

        if(i != responseLen - 1) { Serial.print(":"); }
      }
      Serial.println("");
    }
    else{
      Serial.println("Write error");
    }
  }

  //切断処理
  rc_s620s.rfOff();
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
  digitalWrite(LED, HIGH);
}