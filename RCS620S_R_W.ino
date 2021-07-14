#include "RCS620S.h"

RCS620S rc_s620s;
uint8_t m_idm[8] = {0x01,0x2E,0x50,0xE5,0xCB,0x04,0x82,0x80};
uint8_t cache[8];
int counter = 0;

void setup() {
  pinMode(2, OUTPUT);
  Serial1.begin(115200);
  Serial.begin(115200);
  rc_s620s.initDevice();
}

void loop() {
  if (rc_s620s.polling()) {
        for (int i = 0; i < sizeof(rc_s620s.idm); i++) {
          if(rc_s620s.idm[i] != m_idm[i]){ break; }
          counter ++;
        }

        if(counter == 8){
          Serial.println("まゆり");
          counter = 0;
        }
        else{
          Serial.println("UnknownUser");
          counter = 0;
        }
        digitalWrite(2, HIGH);
    }
    rc_s620s.rfOff();
    digitalWrite(2, LOW);
    delay(1000);
}
