#include <inttypes.h>

enum RWCommand : uint8_t
{
    Polling                 = 0x00, //プーリングコマンド
    ReadWithoutEncryption   = 0x06, //非暗号化読み書き込みコマンド
    WriteWithoutEncryption  = 0x08  //非暗号化読み込みコマンド
};

typedef enum RWResponseCode : uint8_t
{
    PollingResponse                 = 0x01, //プーリングレスポンスコード
    ReadWithoutEncryptionResponse   = 0x07, //非暗号化読み書き込みレスポンスコード
    WriteWithoutEncryptionResponse  = 0x09  //非暗号化読み込みレスポンスコード
};

enum ServiceCode : uint8_t
{
    RWAccess = 0x09, //RW権限ブロックに対するデータの読み書きが可能
    ROAccess = 0x0B  //RO権限ブロック及びRW権限ブロックのデータの読み出しが可能
};

enum SystemCode : unsigned int
{
    code_wild_card      = 0xFFFF,
    code_suica          = 0x0003,
    code_felica_lite_s  = 0x88B4
};

enum RWBlock : uint8_t
{
    S_PAD0      = 0x00, //スクラッチパッド0
    S_PAD1      = 0x01, //スクラッチパッド1
    S_PAD2      = 0x02, //スクラッチパッド2
    S_PAD3      = 0x03, //スクラッチパッド3
    S_PAD4      = 0x04, //スクラッチパッド4
    S_PAD5      = 0x05, //スクラッチパッド5
    S_PAD6      = 0x06, //スクラッチパッド6
    S_PAD7      = 0x07, //スクラッチパッド7
    S_PAD8      = 0x08, //スクラッチパッド8
    S_PAD9      = 0x09, //スクラッチパッド9
    S_PAD10     = 0x0A, //スクラッチパッド11
    S_PAD11     = 0x0B, //スクラッチパッド12
    S_PAD12     = 0x0C, //スクラッチパッド13
    S_PAD13     = 0x0D, //スクラッチパッド14
    REG         = 0x0E, //減算レジスタブロック
    RC          = 0x80, //ランダムチャレンジブロック
    MAC         = 0x81, //MACブロック
    ID          = 0x82, //IDブロック
    D_ID        = 0x83, //デバイスIDブロック
    SER_C       = 0x84, //サービスコードブロック
    SYS_C       = 0x85, //システムコードブロック
    CKV         = 0x86, //カード鍵バージョンブロック
    CK          = 0x87, //カード鍵ブロック
    MC          = 0x88, //メモリコンフィグレーションブロック
    WCNT        = 0x90, //WCNTブロック(ライトカウンタ)
    MAC_A       = 0x91, //MAC_Aブロック
    STATE       = 0x92, //STATEブロック
    CRC_CHECK   = 0xA0  //CRC_CHECKブロック
};

class RCS620SCommand
{
public:
    void CreateDataSendCommand(RWCommand cmd, const uint8_t *IDm, uint8_t serviceSize, ServiceCode sCode, RWBlock bloac, const uint8_t *data);
    void CreateDataReadCommand(RWCommand cmd, const uint8_t *IDm, uint8_t serviceSize, ServiceCode sCode, uint8_t blockSize, RWBlock block);

public:
    uint8_t *cmdList;
};