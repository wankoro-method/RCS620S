//Copyright 2021 わんころメソッド();

#include "RCS620S.h"

//Wait time
const unsigned int IO_wait_time_ms = 500;

//System code
const int code_wild_card = 0xFFFF;
const int code_suica = 0x0003;
const int code_felica_lite_s = 0x88B4;

//RC-S620S Device class
RCS620S rc_s620s;

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
  if (rc_s620s.polling(code_felica_lite_s)) {
    ShowIDm();
    ShowPMm();
    digitalWrite(2, HIGH);

    uint8_t cmd[] = {
      0x08,
      rc_s620s.idm[0], rc_s620s.idm[1], rc_s620s.idm[2], rc_s620s.idm[3], rc_s620s.idm[4], rc_s620s.idm[5], rc_s620s.idm[6], rc_s620s.idm[7],
      0x01,
      0x09, 0x00,
      0x01,
      0x80, 0x0D,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    };

    /*uint8_t cmd[] = {
      0x06,
      rc_s620s.idm[0], rc_s620s.idm[1], rc_s620s.idm[2], rc_s620s.idm[3], rc_s620s.idm[4], rc_s620s.idm[5], rc_s620s.idm[6], rc_s620s.idm[7],
      0x01,
      0x0B, 0x00,
      0x02,
      0x80, 0x05, 0x80, 0x91
    };*/
    
    uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];
    uint8_t responseLen;
    int cache = rc_s620s.cardCommand(cmd, (uint16_t)sizeof(cmd), response, &responseLen);

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
