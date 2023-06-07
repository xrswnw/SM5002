#include "AnyID_SM5002_Device.h"


const u8 DEVICE_VERSION[DEVICE_VERSION_SIZE]@0x08005000 = "SM5002 23033000 G32F103";
const u8 DEVICE_HARD_TYPE[DEVICE_VERSION_SIZE]@0x08005080 = "SM5002-GD-RC663";

DEVICE_OPINFO g_sDeviceOpInfo = {0};
DEVICE_INFOBUF g_sDeviceInfoBuf = {0};
DEVICE_INCHECK g_sDeviceInCheck = {0};

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

const PORT_INF DEV_INSEN_CLK = {GPIOB, GPIO_Pin_0};        
const PORT_INF DEV_INSEN_DAT = {GPIOA, GPIO_Pin_7};         
const PORT_INF DEV_INSEN_LOAD = {GPIOB, GPIO_Pin_1};  
const PORT_INF DEV_INSEN_RFID0 = {GPIOB, GPIO_Pin_8}; 
const PORT_INF DEV_INSEN_RFID1 = {GPIOB, GPIO_Pin_9}; 
void Device_InitInSensor(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = DEV_INSEN_CLK.Pin;
    GPIO_Init(DEV_INSEN_CLK.Port, &GPIO_InitStructure);
    Device_InSensorClkLow();

    GPIO_InitStructure.GPIO_Pin = DEV_INSEN_LOAD.Pin;
    GPIO_Init(DEV_INSEN_LOAD.Port, &GPIO_InitStructure);
    Device_InSensorLoadLow();
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    
    GPIO_InitStructure.GPIO_Pin = DEV_INSEN_DAT.Pin;
    GPIO_Init(DEV_INSEN_DAT.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = DEV_INSEN_RFID0.Pin;
    GPIO_Init(DEV_INSEN_RFID0.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = DEV_INSEN_RFID1.Pin;
    GPIO_Init(DEV_INSEN_RFID1.Port, &GPIO_InitStructure);
}

void Device_ReadInInfo(DEVICE_ININFO *pInInfo)
{
    u16 t = 0;
    u8 i = 0;
    
    Device_InSensorClkLow();      //CLK-LOW
    Device_InSensorLoadLow();     //PL-LOW
    Device_InSensorLoadHigh();      //PL-HIGH
    for(i = 0; i < 8; i++)
    {
        t <<= 1;
        
        if(Device_GetInSensor() == 0x0000)
        {
            t |= 0x01;
        }
        Device_InSensorClkLow();        //CLK-LOW
        Device_InSensorClkHigh();       //CLK-HIGH
    }
    
    Device_InSensorClkLow();      //CLK-LOW
    Device_InSensorLoadLow();     //PL-LOW

    if(Device_GetRfid0())
    {
        t |= 0x0100;
    }

    if(Device_GetRfid1())
    {
        t |= 0x0200;
    }

    *((u16 *)pInInfo) = t;
}

u32 Device_CheckInSensor(DEVICE_INCHECK *pInCheck)
{
    u32 changeFlag = 0;
    u8 i = 0;
    u16 t = 0, checkFlag = 0;
    u16 flag = 0;
    t = *((u32 *)(&pInCheck->curInSensor));
    checkFlag = t ^ pInCheck->inValue;
    for(i = 0; i < DEVICE_IN_SESNOR_NUM; i++)
    {
        flag = 1 << i;
        if(checkFlag & flag)
        {
            pInCheck->checkTick[i]++;
            if(pInCheck->checkTick[i] > DEVICE_IN_CHECK_TIM)
            {
                pInCheck->inValue &= (~flag);
                pInCheck->inValue |= (t & flag);
                changeFlag |= flag;
            }
        }
        else
        {
            pInCheck->checkTick[i] = 0;
        }
    }
    pInCheck->changeFlag = changeFlag;
    return changeFlag;
}

void Device_FomatGate0Record(DEVICE_OPINFO *pOpInfo, DEVICE_INFOBUF *pRcdBuf)
{
    DEVICE_SENSORINFO sensorInfo = {0};

    sensorInfo.addr = 0;
    //温度
    if(pOpInfo->tmpr[0].t > 0)
    {
        sensorInfo.tmpr = pOpInfo->tmpr[0].t;
    }
    else
    {
        sensorInfo.tmpr = 0x80;
        sensorInfo.tmpr |= (~pOpInfo->tmpr[0].t);
        sensorInfo.tmpr += 1;
    }
    if(pOpInfo->inInfo.doorState0 == DEVICE_DOOR_CLOSE_VALUE)
    {
        sensorInfo.sensorState.door = 1;
    }
    if(pOpInfo->inInfo.doorRfid0 == DEVICE_RFID_CLOSE_VALUE)
    {
        sensorInfo.sensorState.rfid = 1;
    }
    if(pOpInfo->inInfo.smoke0 == DEVICE_SMOKE_ALARM_VALUE)
    {
        sensorInfo.sensorState.smoke = 1;
    }
    sensorInfo.sensorState.fan = pOpInfo->outCtrl.fanCtrl0;
    sensorInfo.batState = pOpInfo->batInfo[0].state;
    sensorInfo.chargeState = pOpInfo->chargeInfo[0].state;

    Device_AddRecordInfo(pRcdBuf, &sensorInfo);
}

void Device_FomatGate1Record(DEVICE_OPINFO *pOpInfo, DEVICE_INFOBUF *pRcdBuf)
{
    DEVICE_SENSORINFO sensorInfo = {0};

    sensorInfo.addr = 1;
    //温度
    if(pOpInfo->tmpr[1].t > 0)
    {
        sensorInfo.tmpr = pOpInfo->tmpr[1].t;
    }
    else
    {
        sensorInfo.tmpr = 0x80;
        sensorInfo.tmpr |= (~pOpInfo->tmpr[1].t);
        sensorInfo.tmpr += 1;
    }
    if(pOpInfo->inInfo.doorState0 == DEVICE_DOOR_CLOSE_VALUE)
    {
        sensorInfo.sensorState.door = 1;
    }
    if(pOpInfo->inInfo.doorRfid0 == DEVICE_RFID_CLOSE_VALUE)
    {
        sensorInfo.sensorState.rfid = 1;
    }
    if(pOpInfo->inInfo.smoke0 == DEVICE_SMOKE_ALARM_VALUE)
    {
        sensorInfo.sensorState.smoke = 1;
    }
    sensorInfo.sensorState.fan = pOpInfo->outCtrl.fanCtrl0;
    sensorInfo.batState = pOpInfo->batInfo[1].state;
    sensorInfo.chargeState = pOpInfo->chargeInfo[1].state;

    Device_AddRecordInfo(pRcdBuf, &sensorInfo);
}

void Device_AddRecordInfo(DEVICE_INFOBUF *pRcdBuf, DEVICE_SENSORINFO *pSensorInfo)
{
    pRcdBuf->tick = g_nSysTick;
    
    memcpy(pRcdBuf->buf + pRcdBuf->s, pSensorInfo, sizeof(DEVICE_SENSORINFO));
    pRcdBuf->s++;
    if(pRcdBuf->s == DEVICE_INFO_BUF_NUM)
    {
        pRcdBuf->s = 0;
    }
    if(pRcdBuf->s == pRcdBuf->e)
    {
        pRcdBuf->e++;
        if(pRcdBuf->e == DEVICE_INFO_BUF_NUM)
        {
            pRcdBuf->e=  0;
        }
    }
}


u16 Device_ProcessUsrFrame(u8 *pFrame)
{
    u8 cmd = 0;
    u16 destAddr = 0;
    u16 paramsLen = 0;
    u8 *pParams = NULL;

    destAddr = *((u16 *)(pFrame + UART_FRAME_POS_DESTADDR));
    if((destAddr != UART_FRAME_BROADCAST_ADDR) && (destAddr != g_sDeviceOpInfo.addr))
    {
        return 0;
    }

    
    cmd = *(pFrame + UART_FRAME_POS_CMD);

    paramsLen = pFrame[UART_FRAME_POS_LEN] + 3 - UART_FRAME_MIN_LEN;
    pParams = pFrame + UART_FRAME_POS_PAR;

    g_sR485TxFrame.len = 0;
    switch(cmd)
    {
        case UART_FRAME_CMD_RST:
            g_sR485TxFrame.len = Uart_UsrResponseFrame(NULL, 0, cmd, g_sDeviceOpInfo.addr, g_sR485TxFrame.buffer);
            break;
        case UART_FRAME_CMD_VER:
            g_sR485TxFrame.len = Uart_UsrResponseFrame((u8 *)DEVICE_VERSION, DEVICE_VERSION_SIZE, cmd, g_sDeviceOpInfo.addr, g_sR485TxFrame.buffer);
            break;
        default:
            break;
    }

    return g_sR485TxFrame.len;
}


