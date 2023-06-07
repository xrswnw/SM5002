#include "AnyID_SM5002_FRam.h"

FRAM_BOOTPARAMS g_sFramBootParamenter = {0};
FRAM_KEY g_sFramKeyInfo = {0};
u32 g_nFRamKey1 = 0;
u32 g_nFRamKey2 = 0;
u32 g_nFRamKey3 = 0;

void Fram_ReadBootParamenter(void)
{
    BOOL b = FALSE, bBackup = FALSE;
    b = FRam_ReadBuffer(FRAME_BOOT_INFO_ADDR, sizeof(FRAM_BOOTPARAMS), (u8 *)(&g_sFramBootParamenter));
    if(b)
    {
        u16 crc1 = 0, crc2 = 0;

        crc1 = a_GetCrc((u8 *)(&g_sFramBootParamenter), (sizeof(FRAM_BOOTPARAMS)) - 2);
        crc2 = g_sFramBootParamenter.crc;

        //�������Ƿ���ȷ���������ȷ��ʹ��Ĭ�ϲ�������
        if(crc1 != crc2)
        {
            b = FALSE;
        }
    }  

    bBackup = FRam_ReadBuffer(FRAME_BOOT_INFO_BACKUP_ADDR, sizeof(FRAM_BOOTPARAMS), (u8 *)(&g_sFramBootParamenter));
    if(bBackup)
    {
        u16 crc1 = 0, crc2 = 0;

        crc1 = a_GetCrc((u8 *)(&g_sFramBootParamenter), (sizeof(FRAM_BOOTPARAMS)) - 2);
        crc2 = g_sFramBootParamenter.crc;

        //�������Ƿ���ȷ���������ȷ��ʹ��Ĭ�ϲ�������
        if(crc1 != crc2)
        {
            bBackup = FALSE;
        }
    }

    if(b == FALSE && bBackup == FALSE)
    {
        g_sFramBootParamenter.addr = 0x01;
        g_sFramBootParamenter.appState = FRAM_BOOT_APP_FAIL;
        Fram_WriteBootParamenter();
    }
    else if(b == TRUE && bBackup == FALSE)
    {
        FRam_ReadBuffer(FRAME_BOOT_INFO_ADDR, sizeof(FRAM_BOOTPARAMS), (u8 *)(&g_sFramBootParamenter));
        FRam_WriteBuffer(FRAME_BOOT_INFO_BACKUP_ADDR, sizeof(FRAM_BOOTPARAMS), (u8 *)(&g_sFramBootParamenter));
    }
    else if(b == FALSE && bBackup == TRUE)
    {
        FRam_ReadBuffer(FRAME_BOOT_INFO_BACKUP_ADDR, sizeof(FRAM_BOOTPARAMS), (u8 *)(&g_sFramBootParamenter));
        FRam_WriteBuffer(FRAME_BOOT_INFO_ADDR, sizeof(FRAM_BOOTPARAMS), (u8 *)(&g_sFramBootParamenter));
    }
}

BOOL Fram_WriteBootParamenter(void)
{
    BOOL b = FALSE;

    g_sFramBootParamenter.crc = 0;
    g_sFramBootParamenter.crc = a_GetCrc((u8 *)(&g_sFramBootParamenter), (sizeof(FRAM_BOOTPARAMS)) - 2);


    if(FRam_WriteBuffer(FRAME_BOOT_INFO_ADDR, sizeof(FRAM_BOOTPARAMS), (u8 *)(&g_sFramBootParamenter)) == TRUE &&
        FRam_WriteBuffer(FRAME_BOOT_INFO_BACKUP_ADDR, sizeof(FRAM_BOOTPARAMS), (u8 *)(&g_sFramBootParamenter)) == TRUE)
    {
        b = TRUE;
    }
    
    return b;
}

void FRam_ReadKey(void)
{
    BOOL b = FALSE, bBackup = FALSE;
    b = FRam_ReadBuffer(FRAME_KEY_ADDR, sizeof(FRAM_KEY), (u8 *)(&g_sFramKeyInfo));
    if(b)
    {
        u16 crc1 = 0, crc2 = 0;

        crc1 = a_GetCrc((u8 *)(&g_sFramKeyInfo), (sizeof(FRAM_KEY)) - 4);
        crc2 = g_sFramKeyInfo.crc;

        //�������Ƿ���ȷ���������ȷ��ʹ��Ĭ�ϲ�������
        if(crc1 != crc2)
        {
            b = FALSE;
        }
    }  
    bBackup = FRam_ReadBuffer(FRAME_KEY_BACKUP_ADDR, sizeof(FRAM_KEY), (u8 *)(&g_sFramKeyInfo));
    if(bBackup)
    {
        u16 crc1 = 0, crc2 = 0;

        crc1 = a_GetCrc((u8 *)(&g_sFramKeyInfo), (sizeof(FRAM_KEY)) - 4);
        crc2 = g_sFramKeyInfo.crc;

        //�������Ƿ���ȷ���������ȷ��ʹ��Ĭ�ϲ�������
        if(crc1 != crc2)
        {
            bBackup = FALSE;
        }
    }

    if(b == FALSE && bBackup == FALSE)
    {
        memcpy(&g_sFramKeyInfo, (u8 *)FRAM_KEY_DFT_ADDR, sizeof(FRAM_KEY));
        FRam_WriteKey();
    }
    else if(b == TRUE && bBackup == FALSE)
    {
        FRam_ReadBuffer(FRAME_KEY_ADDR, sizeof(FRAM_KEY), (u8 *)(&g_sFramKeyInfo));
        FRam_WriteBuffer(FRAME_KEY_BACKUP_ADDR, sizeof(FRAM_KEY), (u8 *)(&g_sFramKeyInfo));
    }
    else if(b == FALSE && bBackup == TRUE)
    {
        FRam_ReadBuffer(FRAME_KEY_BACKUP_ADDR, sizeof(FRAM_KEY), (u8 *)(&g_sFramKeyInfo));
        FRam_WriteBuffer(FRAME_KEY_ADDR, sizeof(FRAM_KEY), (u8 *)(&g_sFramKeyInfo));
    }
}

BOOL FRam_WriteKey(void)
{
    BOOL b = FALSE;
    
    g_sFramKeyInfo.crc = 0;
    g_sFramKeyInfo.crc = a_GetCrc((u8 *)(&g_sFramKeyInfo), (sizeof(FRAM_KEY)) - 4);

    b = FRam_WriteBuffer(FRAME_KEY_ADDR, sizeof(FRAM_KEY), (u8 *)(&g_sFramKeyInfo));
    b = FRam_WriteBuffer(FRAME_KEY_BACKUP_ADDR, sizeof(FRAM_KEY), (u8 *)(&g_sFramKeyInfo));
    return b;
}


#define Keeloq_NLF		0x3A5C742E
#define bit(x,n)		(((x)>>(n))&1)
#define g5(x,a,b,c,d,e)	(bit(x,a)+bit(x,b)*2+bit(x,c)*4+bit(x,d)*8+bit(x,e)*16)
u32	FRam_KeeloqEncrypt(u32 data, u64 key)
{
	u32 x = data, r;

	for (r = 0; r < 528; r++)
	{
		x = (x>>1)^((bit(x,0)^bit(x,16)^(u32)bit(key,r&63)^bit(Keeloq_NLF,g5(x,1,9,20,26,31)))<<31);
	}
	return x;
}

u32	FRam_KeeloqDecrypt(u32 data, u64 key)
{
	u32 x = data, r;

	for (r = 0; r < 528; r++)
	{
		x = (x<<1)^bit(x,31)^bit(x,15)^(u32)bit(key,(15-r)&63)^bit(Keeloq_NLF,g5(x,0,8,19,25,30));
	}
	return x;
}

void FRam_EncryptKey(u8 *pCpuId)
{
    u64 key1 = 0x198308185516;
    u64 key2 = 0x201210043117;
    u64 key3 = 0x201611293117;
    
    g_nFRamKey1 = FRam_KeeloqEncrypt(*((u32 *)(pCpuId + 0)), key1);
    g_nFRamKey2 = FRam_KeeloqEncrypt(*((u32 *)(pCpuId + 4)), key2);
    g_nFRamKey3 = FRam_KeeloqEncrypt(*((u32 *)(pCpuId + 8)), key3);
}

