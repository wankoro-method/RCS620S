//Copyright 2021 わんころメソッド();

#include "RCS620S.h"
#include "RCS620SCommand.h"

//Wait time
const unsigned int IO_wait_time_ms = 500;

//RC-S620S Device class
RCS620S rc_s620s;
RCS620SCommand rc_s620s_cmd;

void ShowIDm();
void ShowPMm();

//Initialize
void setup() {
  pinMode(2, OUTPUT);
  Serial1.begin(115200);
  Serial.begin(115200);
  rc_s620s.initDevice();
}

void loop() {
  if (rc_s620s.polling(SystemCode::code_felica_lite_s)) {
    ShowIDm();
    ShowPMm();
    digitalWrite(2, HIGH);
    
    uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];
    uint8_t responseLen;
    rc_s620s_cmd.CreateDataReadCommand(RWCommand::ReadWithoutEncryption, rc_s620s.idm, 0x01, ServiceCode::ROAccess, 0x01, RWBlock::S_PAD13);

    int cache = rc_s620s.cardCommand(rc_s620s_cmd.cmdList, 17, response, &responseLen);

    for (int i = 0; i < responseLen; i++) {
      Serial.print(response[i], HEX);
      Serial.print(":");
    }

    Serial.println(cache);

    //切断処理
    rc_s620s.rfOff();
    digitalWrite(2, LOW);
  }
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
