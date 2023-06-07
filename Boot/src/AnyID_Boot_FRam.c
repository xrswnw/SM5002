#include "AnyID_Boot_FRam.h"

FRAM_BOOTPARAMS g_sFramBootParamenter = {0};

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
        g_sFramBootParamenter.br = FRAM_BR_38400;
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