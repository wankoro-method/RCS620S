//Copyright 2021 わんころメソッド();

#include "RCS620S.h"
#include "RCS620SCommand.h"
#include "HardwareSerial.h"

#include <SPI.h>
#include <SD.h>
#include <TFT.h>  // Arduino LCD library

#include <string.h>

const int LED = 2;                              //LED
const unsigned int IO_wait_time_ms = 250;       //Wait time

RCS620S rc_s620s;                               //RC-S620S Device class
RCS620SCommand rc_s620s_cmd;                    //RC-S620S Command class

uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];  //レスポンスデータキャッチ用
uint8_t responseLen;

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

void loop() {
  if(!Serial.available()) { digitalWrite(LED, LOW); return; }

  //アクセスランプ点灯
  digitalWrite(LED, HIGH);

  uint8_t data[9];
  int counter = 0;
  while(Serial.available()){
    data[counter] = Serial.read();
    counter++;
  }

  for(int i = 0; i < 8; i++){
    Serial.write(data, HEX);
  }
//  rc_s620s.polling();
//
//  for(int i = 0; i < sizeof(rc_s620s.idm); i++){
//    Serial.print(rc_s620s.idm[i], HEX);
//  }
//  Serial.println("");
//
  //アクセスランプ消灯
  digitalWrite(LED, LOW);

  //切断処理
//  rc_s620s.rfOff();
//  delay(100);
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
