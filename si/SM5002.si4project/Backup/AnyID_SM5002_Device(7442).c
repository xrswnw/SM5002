#include "AnyID_SM5002_Reader.h"


const u8 READER_VERSION[READER_VERSION_SIZE]@0x08005000 = "SM5002 23033000 G32F103";
const u8 READER_HARD_TYPE[READER_VERSION_SIZE]@0x08005080 = "SM5002-GD-RC663";

void Reader_Delayms(u32 n)
{
    n *= 0x3800;
    n++;
    while(n--);
}

void Reader_ReadDeviceParamenter(void)
{
    BOOL b = FALSE, bBackup = FALSE;
    b = FRam_ReadBuffer(FRAME_INFO_ADDR, sizeof(READER_PARAMSREG), (u8 *)(&g_sReaderParamsRegs));
    if(b)
    {
        u16 crc1 = 0, crc2 = 0;

        crc1 = a_GetCrc((u8 *)(&g_sReaderParamsRegs), (sizeof(READER_PARAMSREG)) - 2);
        crc2 = g_sReaderParamsRegs.crc;

        //检测参数是否正确，如果不正确，使用默认参数操作
        if(crc1 != crc2)
        {
            b = FALSE;
        }
        else
        {
            b = Reader_CheckDeviceParamenter(&g_sReaderParamsRegs);
        }
    }  
    bBackup = FRam_ReadBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(READER_PARAMSREG), (u8 *)(&g_sReaderParamsRegs));
    if(bBackup)
    {
        u16 crc1 = 0, crc2 = 0;

        crc1 = a_GetCrc((u8 *)(&g_sReaderParamsRegs), (sizeof(READER_PARAMSREG)) - 2);
        crc2 = g_sReaderParamsRegs.crc;

        //检测参数是否正确，如果不正确，使用默认参数操作
        if(crc1 != crc2)
        {
            bBackup = FALSE;
        }
        else
        {
            bBackup = Reader_CheckDeviceParamenter(&g_sReaderParamsRegs);
        }
    }

    if(b == FALSE && bBackup == FALSE)
    {
        
        Reader_ResetDeviceParamenter();
        Reader_WriteDeviceParamenter();
    }
    else if(b == TRUE && bBackup == FALSE)
    {
        FRam_ReadBuffer(FRAME_INFO_ADDR, sizeof(READER_PARAMSREG), (u8 *)(&g_sReaderParamsRegs));
        FRam_WriteBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(READER_PARAMSREG), (u8 *)(&g_sReaderParamsRegs));
    }
    else if(b == FALSE && bBackup == TRUE)
    {
        FRam_ReadBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(READER_PARAMSREG), (u8 *)(&g_sReaderParamsRegs));
        FRam_WriteBuffer(FRAME_INFO_ADDR, sizeof(READER_PARAMSREG), (u8 *)(&g_sReaderParamsRegs));
    }
    //注意：这里的boot参数保存和其他设备是有差异的 
    
    FRam_ReadKey();
    FRam_EncryptKey((u8 *)STM32_CPUID_ADDR);
}

BOOL Reader_WriteDeviceParamenter(void)
{
    BOOL b = FALSE;

#if defined(_ANYID_FR320P_)
    g_sReaderParamsRegs.br = READER_BR_115200;
#endif
    g_sReaderParamsRegs.crc = 0;
    g_sReaderParamsRegs.crc = a_GetCrc((u8 *)(&g_sReaderParamsRegs), (sizeof(READER_PARAMSREG)) - 2);

    b = FRam_WriteBuffer(FRAME_INFO_ADDR, sizeof(READER_PARAMSREG), (u8 *)(&g_sReaderParamsRegs));
    b = FRam_WriteBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(READER_PARAMSREG), (u8 *)(&g_sReaderParamsRegs));
    
    return b;
}

const PORT_INF DEV_ADDR_CLK = {GPIOB, GPIO_Pin_0};      //时钟引脚
const PORT_INF DEV_ADDR_LOAD = {GPIOB, GPIO_Pin_1};     //使能输出引脚
const PORT_INF DEV_ADDR_OUT = {GPIOA, GPIO_Pin_8};      //输入，并转串输入
void Device_InitRollSwitch(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = DEV_ADDR_CLK.Pin;
    GPIO_Init(DEV_ADDR_CLK.Port, &GPIO_InitStructure);
    Device_AddrClkLow();

    GPIO_InitStructure.GPIO_Pin = DEV_ADDR_LOAD.Pin;
    GPIO_Init(DEV_ADDR_LOAD.Port, &GPIO_InitStructure);
    Device_AddrLoadLow();
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    
    GPIO_InitStructure.GPIO_Pin = DEV_ADDR_OUT.Pin;
    GPIO_Init(DEV_ADDR_OUT.Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = DEV_ADDR_OUT.Pin;
    GPIO_Init(DEV_ADDR_OUT.Port, &GPIO_InitStructure);
}

u32 Device_ReadRollSwitch(void)
{
    u32 t = 0, s1 = 0, s2 = 0, s3 = 0;
        
    u32 i = 0, portA = 0, portB = 0;

    Device_AddrClkLow();      //CLK-LOW
    
    Device_AddrLoadLow();     //PL-LOW
    Device_AddrLoadHigh();      //PL-HIGH
    for(i = 0; i < 8; i++)
    {
        portA <<= 1;
        portB <<= 1;
        
        if(Device_GetAddrPortA() == 0x0000)
        {
            portA |= 0x01;
        }
        if(Device_GetAddrPortB() == 0x0000)
        {
            portB |= 0x01;
        }
        Device_AddrClkLow();        //CLK-LOW
        Device_AddrClkHigh();       //CLK-HIGH
    }
    
    Device_AddrClkLow();      //CLK-LOW
    Device_AddrLoadLow();     //PL-LOW
    
    s1 = (portA >> 0) & 0x0F;
    s2 = (portA >> 4) & 0x0F;
    s3 = (portB >> 0) & 0x0F;

    t += g_aDeviceAddrOffset[s1] * 100;
    t += g_aDeviceAddrOffset[s2] * 10;
    t += g_aDeviceAddrOffset[s3];
    
    return t;
}


