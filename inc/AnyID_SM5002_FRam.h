#ifndef _ANYID_SM5002_FRAM_
#define _ANYID_SM5002_FRAM_

#include "AnyID_FRam.h"

#define FRAM_BOOT_APP_OK                    0x5555
#define FRAM_BOOT_APP_FAIL                  0xAAAA

typedef struct framBootDevicePar{
    u16 appState;
    u16 rfu;
    u16 addr;
    u16 crc;
}FRAM_BOOTPARAMS;
extern FRAM_BOOTPARAMS g_sFramBootParamenter;


#define FRAME_MEM_SIZE                      0x2000

#define FRAME_INFO_ADDR                     0x000
#define FRAME_INFO_BACKUP_ADDR              0x100

#define FRAME_BOOT_INFO_BACKUP_ADDR         (FRAME_MEM_SIZE - 512) 
#define FRAME_BOOT_INFO_ADDR                (FRAME_MEM_SIZE - sizeof(FRAM_BOOTPARAMS))      //固件升级从480字节开始写，共512字节

#define FRAME_KEY_ADDR                      0x1200          //key
#define FRAME_KEY_BACKUP_ADDR               0x1280          //key

#define FRAM_KEY_DFT_ADDR                   0x08008000
#define FRAM_KEY1_POS_START                 12
#define FRAM_KEY2_POS_START                 32
#define FRAM_KEY3_POS_START                 52
typedef struct frameKeyParams{
    u8 keyBuffer[60];
    u32 crc;
}FRAM_KEY;
extern FRAM_KEY g_sFramKeyInfo;
extern u32 g_nFRamKey1;
extern u32 g_nFRamKey2;
extern u32 g_nFRamKey3;

#define FRam_CheckKey1()                    (*((u32 *)(g_sFramKeyInfo.keyBuffer + FRAM_KEY1_POS_START)) == g_nFRamKey1)
#define FRam_CheckKey2()                    (*((u32 *)(g_sFramKeyInfo.keyBuffer + FRAM_KEY2_POS_START)) == g_nFRamKey2)
#define FRam_CheckKey3()                    (*((u32 *)(g_sFramKeyInfo.keyBuffer + FRAM_KEY3_POS_START)) == g_nFRamKey3)
#define FRam_CheckKey4()                    (TRUE)
#define FRam_CheckKey5()                    (TRUE)
#define FRam_CheckKey()                     (FRam_CheckKey1() && FRam_CheckKey2() && FRam_CheckKey3() && FRam_CheckKey4() && FRam_CheckKey5())

void Fram_ReadBootParamenter(void);
BOOL Fram_WriteBootParamenter(void);

void FRam_ReadKey(void);
BOOL FRam_WriteKey(void);
void FRam_EncryptKey(u8 *pCpuId);
u32	FRam_KeeloqEncrypt(u32 data, u64 key);
u32	FRam_KeeloqDecrypt(u32 data, u64 key);

#endif

