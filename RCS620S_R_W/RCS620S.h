//Copyright 2010 Sony Corporation
//Copyright 2021 わんころメソッド();

#include <inttypes.h>

#ifndef RCS620S_H_
#define RCS620S_H_

#define RCS620S_MAX_CARD_RESPONSE_LEN    254
#define RCS620S_MAX_RW_RESPONSE_LEN      265

class RCS620S
{
public:
  RCS620S();

  int initDevice(void);
  int polling(uint16_t systemCode = 0xffff);
  int cardCommand( const uint8_t* command, uint8_t commandLen, uint8_t response[RCS620S_MAX_CARD_RESPONSE_LEN], uint8_t* responseLen);
  int rfOff(void);
  int push( const uint8_t* data, uint8_t dataLen);

private:
  int rwCommand( const uint8_t* command, uint16_t commandLen, uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN], uint16_t* responseLen);
  void cancel(void);
  uint8_t calcDCS( const uint8_t* data, uint16_t len);

  void writeSerial( const uint8_t* data, uint16_t len);
  int readSerial( uint8_t* data, uint16_t len);
  void flushSerial(void);

  int checkTimeout(unsigned long t0);

public:
  unsigned long timeout;
  uint8_t idm[8];
  uint8_t pmm[8];
};

#endif /* !RCS620S_H_ */

/*
Arduino 用 RC-S620/S ライブラリ Version 1.0.0.1
===============================================
ソニー株式会社
2010年11月26日

■ はじめに

  本プログラムは、Arduino から RC-S620/S モジュールを操作するための
  ライブラリのサンプルコードです。

  本プログラムは無保証です。本プログラムの使用に起因する損害について、
  当社は一切責任を負いませんので、ご了承ください。

========================================================================
■ ライブラリ使用上のご注意

  本ライブラリは、原則として入力値や内部状態の検査を行いません。
  引数に正しくない値を指定した場合の動作は不定となりますので、
  ご注意ください。

========================================================================
■ クラス名

クラス名は RCS620S です。

========================================================================
■ プロパティ

unsigned long timeout;
    RC-S620/S との通信のタイムアウト (ms) を指定します。
    デフォルトは 1000 ミリ秒です。

uint8_t idm[8];
    捕捉したカードの IDm です。

uint8_t pmm[8];
    捕捉したカードの PMm です。

========================================================================
■ メソッド

------------------------------------------------------------------------
・initDevice()

  【書式】
    int initDevice(void);

  【解説】
      initDevice() は、RC-S620/S の初期設定を行います。最初にこの
      関数を必ず呼び出してください。また、この関数を呼び出す前に、
      Serial.begin(115200) を呼び出してください。

      成功した場合は 1 を返し、失敗した場合は 0 を返します。
     
------------------------------------------------------------------------
・polling()

  【書式】
    int polling(uint16_t systemCode);

  【解説】
      polling() は、指定されたシステムコードを持つ FeliCa カードの
      捕捉を試みます。捕捉に成功した場合は、プロパティ idm と pmm に
      捕捉したカードの IDm および PMm がそれぞれ設定されます。

      捕捉に成功した場合は 1 を返し、失敗した場合は 0 を返します。

      この関数は、initDevice() が成功したあとに呼び出してください。

      この関数を呼び出すと RC-S620/S から搬送波の出力が開始されます。
      搬送波の出力を止めたいときは、rfOff() を呼び出してください。

------------------------------------------------------------------------
・cardCommand()

  【書式】
    int cardCommand(
        const uint8_t* command,
        uint8_t commandLen,
        uint8_t response[RCS620S_MAX_CARD_RESPONSE_LEN],
        uint8_t* responseLen);

  【解説】
      cardCommand() は、polling() で捕捉したカードにカードコマンドを
      送信し、レスポンスを受信します。

      送信するコマンドを command に、その長さを commandLen に指定して
      ください。command や commandLen には LEN バイトを含みません。
      カードレスポンスの受信に成功すると、受信したレスポンスが
      response に、その長さが responseLen に格納されます。送受信できる
      コマンドおよびレスポンスの最大長は 254 バイトです。

      コマンドの送信とレスポンスの受信に成功した場合は 1 を返し、
      失敗した場合は 0 を返します。

      この関数は、polling() が成功したあとに呼び出してください。

------------------------------------------------------------------------
・rfOff()

  【書式】
    int rfOff(void);

  【解説】
      rfOff() は、搬送波の出力を停止します。成功した場合は 1 を返し、
      失敗した場合は 0 を返します。

      この関数は、initDevice() が成功したあとに呼び出してください。

------------------------------------------------------------------------
・push()

  【書式】
    int push(
        const uint8_t* data,
        uint8_t dataLen);

  【解説】
      push() は、三者間通信コマンド (Push および Activate2) を使用して、
      モバイル FeliCa IC チップ搭載機器などに指定されたデータを送信
      します。送信するデータを data に、その長さを dataLen に指定して
      ください。

      この関数は、polling() が成功したあとに呼び出してください。

========================================================================

Copyright 2010 Sony Corporation 
*/
