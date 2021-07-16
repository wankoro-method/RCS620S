# Arduino用RC-S620Sリードライタープログラム
- このプログラムはArduinoとRC-S620Sをシリアル通信で使用する想定で作られています。  
- RCS620S.hについては別途ヘッダ内に書かれている説明を参照してください。  

**※現在開発中でコマンドリスト等が不完全であり、コマンドの長さを取得する関数等がまだ存在しません**  

# 通信仕様  
※Type-AはISO/IEC 14443 通信です。  
※Type-BはISO/IEC 14443 通信です。  
※Type-FはFelica通信です。  

| 方式 | 仕様 |
|:---:|:---|
| キャリア周波数(fc) | 13.56MHz |
| データ転送速度 | 下記詳細 |
| Type-A<br>Type-B<br>Type-F | 106kbps<br>106kbps<br>212kbps / 424kbps |
| 変調方式 | 下記詳細 |
| Type-A<br><br><br>Type-B<br><br><br>Type-F |  ・送信 - ASK <br> ・受信 - ASK <br><br> ・送信 - ASK <br> ・受信 - BPSK <br><br> ・送信 - ASK <br> ・受信 - ASK|
| ビットコーディング | 下記詳細 |
| Type-A<br><br><br>Type-B<br><br><br>Type-F | ・送信 - Modified Miller <br> ・受信 - Sub carrier 付き Manchester <br><br>・送信 - NRZ <br> ・受信 - Sub carrier 付き NRZ <br><br>・送信 - Manchester <br> ・受信 - Manchester |
| 通信方式 | 下記詳細 |
| Type-A<br>Type-B<br>Type-F | 半二重通信 <br> 半二重通信 <br> 半二重通信、CRC-ITU-T |

# ハードウェア要件

| ハードウェア | 要件 |
|:---:|:---|
| 制御マイコン | ・Arduino ProMicro(16MHz) <br> ・ATMega32u4(16MHz) |
| リードライター | SONY RC-S620S(3.3V / 5V)
  
# RCS620SCommand.h

## CreateDataSendCommand関数
データを書き込む際のコマンドを生成します。  
生成されたコマンドはPublic変数のcmdListに格納されます。  
コマンドに関して不明な点は各チップのユーザーズマニュアルにあるコマンドページを参照してください。
```cpp
//書式
CreateDataSendCommand(RWCommand cmd, const uint8_t *IDm, uint8_t serviceSize, ServiceCode sCode, RWBlock bloac, const uint8_t *data)
```

## CreateDataSendCommand関数
データを読み込む際のコマンドを生成します。  
生成されたコマンドはPublic変数のcmdListに格納されます。  
コマンドに関して不明な点は各チップのユーザーズマニュアルにあるコマンドページを参照してください。
```cpp
//書式
CreateDataReadCommand(RWCommand cmd, const uint8_t *IDm, uint8_t serviceSize, ServiceCode sCode, uint8_t blockSize, RWBlock block)
```

## RWCommand
読み込み/書き込み時に使用するコマンドを提供します  

```cpp
//書式
RWCommand::定義名
```

| 定義名 | 内容 |
|:---:|:---|
| Pooling | プーリングコマンド(0x00)を取得します |
| ReadWithoutEncryption | 非暗号化読み込みコマンド(0x06)を取得します |
| WriteWithoutEncryption | 非暗号化書き込みコマンド(0x08)を取得します |

## RWResponseCode
レスポンス識別用ビットを取得します

```cpp
//書式
RWResponseCode::定義名
```

| 定義名 | 内容 |
|:---:|:---|
| PoolingResponse | プーリングレスポンスコマンド(0x01)を取得します |
| ReadWithoutEncryptionResponse | 非暗号化読み込みレスポンスコマンドを取得します |
| WriteWithoutEncryptionResponse | 非暗号化書き込みレスポンスコマンドを取得します |

## ServiceCode
読み込み/書き込み時に使用するサービスコードコマンドを提供します 

```cpp
//書式
ServiceCode::定義名
```

| 定義名 | 内容 |
|:---:|:---|
| RWAccess | RW権限ブロックに対するデータの読み書きが可能なサービスコードを取得します |
| ROAccess | RO権限ブロック及びRW権限ブロックのデータの読み出しが可能なサービスコードを取得します |

## SystemCode
プーリングコマンドで使用するシステムコードを提供します 

```cpp
//書式
SystemCode::定義名
```

| 定義名 | 内容 |
|:---:|:---|
| code_wild_card | 読み取り可能なすべてのカードに対してプーリングを行います |
| code_suica | Suica / モバイルスイカに対してプーリングを行います |
| code_felica_lite_s | Felica Lite-Sに対してプーリングを行います |

## RWBlock
読み書き時のブロックを提供します 

```cpp
//書式
RWBlock::定義名
```

| 定義名 | 内容 |
|:---:|:---|
| S_PAD0～13 | スクラッチパッド(0～13) |
| REG |減算レジスタブロック |
| RC | ランダムチャレンジブロック |
| MAC | MACブロック |
| ID | IDブロック |
| D_ID | デバイスIDブロック |
| SER_C | サービスコードブロック |
| SYS_C | システムコードブロック |
| CKV | カード鍵バージョンブロック |
| CK | カード鍵ブロック |
| MC | メモリコンフィグレーションブロック |
| WCNT | WCNTブロック(ライトカウンタ) |
| MAC_A | MAC_Aブロック |
| STATE | STATEブロック |
| CRC_CHECK | CRC_CHECKブロック |


