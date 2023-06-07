#include "AnyID_SM5002_Device.h"


const u8 DEVICE_VERSION[DEVICE_VERSION_SIZE]@0x08005000 = "SM5002 23033000 G32F103";
const u8 DEVICE_HARD_TYPE[DEVICE_VERSION_SIZE]@0x08005080 = "SM5002-GD-RC663";

DEVICE_OPINFO g_sDeviceOpInfo = {0};

void Device_Delayms(u32 n)
{
    n *= 0x3800;
    n++;
    while(n--);
}

void Device_ReadDeviceParamenter(void)
{
    
    //注意：这里的boot参数保存和其他设备是有差异的 
    
    FRam_ReadKey();
    FRam_EncryptKey((u8 *)STM32_CPUID_ADDR);
}

BOOL Device_WriteDeviceParamenter(void)
{
    BOOL b = FALSE;

    return b;
}

const PORT_INF DEV_ADDR_CLK = {GPIOB, GPIO_Pin_0};      //时钟引脚
const PORT_INF DEV_ADDR_LOAD = {GPIOB, GPIO_Pin_1};     //使能输出引脚
const PORT_INF DEV_ADDR_OUT = {GPIOA, GPIO_Pin_8};      //输入，并转串输入
void Device_InitAddr(void)
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
}

#define DEVICE_ADDR_CHK_TIM         10
u8 Device_ReadAddr(void)
{
    u8 addr = 0, t = 0;
    u8 c = 0;
    u8 i = 0;
    
    while(c < DEVICE_ADDR_CHK_TIM)
    {
        Device_Delayms(1);
        t = 0;
        
        Device_AddrClkLow();      //CLK-LOW
        Device_AddrLoadLow();     //PL-LOW
        Device_AddrLoadHigh();      //PL-HIGH
        for(i = 0; i < 8; i++)
        {
            t <<= 1;
            
            if(Device_GetAddrOut() == 0x0000)
            {
                t |= 0x01;
            }
            Device_AddrClkLow();        //CLK-LOW
            Device_AddrClkHigh();       //CLK-HIGH
        }
        
        Device_AddrClkLow();      //CLK-LOW
        Device_AddrLoadLow();     //PL-LOW
        
        if(t != addr)
        {
            addr = t;
            c = 0;
        }
        else
        {
            c++;
        }
    }

    return addr;
}

const PORT_INF DEV_OUTCTRL_CLK = {GPIOA, GPIO_Pin_12};           //时钟引脚
const PORT_INF DEV_OUTCTRL_DAT = {GPIOA, GPIO_Pin_11};          //输出，并转串
const PORT_INF DEV_OUTCTRL_EN0 = {GPIOB, GPIO_Pin_12};          //使能输出引脚
const PORT_INF DEV_OUTCTRL_EN1 = {GPIOB, GPIO_Pin_13};          //使能输出引脚
void Device_InitOutCtrl(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = DEV_OUTCTRL_CLK.Pin;
    GPIO_Init(DEV_OUTCTRL_CLK.Port, &GPIO_InitStructure);
    Device_OutCtrlClkLow();

    GPIO_InitStructure.GPIO_Pin = DEV_OUTCTRL_DAT.Pin;
    GPIO_Init(DEV_OUTCTRL_DAT.Port, &GPIO_InitStructure);
    Device_OutCtrlClkLow();
    
    GPIO_InitStructure.GPIO_Pin = DEV_OUTCTRL_EN0.Pin;
    GPIO_Init(DEV_OUTCTRL_EN0.Port, &GPIO_InitStructure);
    Device_OutCtrlEn0Low();
    
    GPIO_InitStructure.GPIO_Pin = DEV_OUTCTRL_EN1.Pin;
    GPIO_Init(DEV_OUTCTRL_EN1.Port, &GPIO_InitStructure);
    Device_OutCtrlEn1Low();
}

void Device_OutCtrl(DEVICE_OUTCTRL *pOutCtrl)
{
    u16 t = 0;
    u8 i = 0;
    
    t = *((u16 *)pOutCtrl);

    Device_OutCtrlEn0Low();
    for(i = 0; i < 8; i++)
    {
        Device_OutCtrlClkLow();
        if(t & 0x01)
        {
            Device_OutCtrlDatHigh();
        }
        else
        {
            Device_OutCtrlDatLow();
        }
        Device_OutCtrlClkHigh();
        t >>= 1;
        
    }
    Device_OutCtrlEn0High();
    
    Device_OutCtrlEn1Low();
    for(i = 0; i < 8; i++)
    {
        Device_OutCtrlClkLow();
        if(t & 0x01)
        {
            Device_OutCtrlDatHigh();
        }
        else
        {
            Device_OutCtrlDatLow();
        }
        Device_OutCtrlClkHigh();
        t >>= 1;
        
    }
    Device_OutCtrlEn1High();
}


