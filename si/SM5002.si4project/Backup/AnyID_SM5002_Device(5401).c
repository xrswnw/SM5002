#include "AnyID_SM5002_Device.h"


const u8 DEVICE_VERSION[DEVICE_VERSION_SIZE]@0x08005000 = "SM5002 23052300 G32F303";
const u8 DEVICE_HARD_TYPE[DEVICE_VERSION_SIZE]@0x08005080 = "SM5002-GD-RC663";

#define DEVICE_VERSION_INFO          "23052300"



DEVICE_PARAMS g_sDeviceParams = {0};
DEVICE_OPINFO g_sDeviceOpInfo = {0};
DEVICE_INFOBUF g_sDeviceInfoBuf = {0};
DEVICE_GATEINFO g_sDeviceGateInfo[DEVICE_GATE_NUM] = {0};
DEVICE_INCHECK g_sDeviceInCheck[DEVICE_GATE_NUM] = {0};

void Device_Delayms(u32 n)
{
    n *= 0x6000;
    n++;
    while(n--);
}

void Device_Init(void)
{
    //1号仓充电机
    g_sChagOpInfo[CHAG_DEV_INDEX0].mode = CHAG_MODE_INIT;
    g_sChagOpInfo[CHAG_DEV_INDEX0].pChagParams = &g_sDeviceParams.chagParams;
    Chag_GetOp(&g_sChagOpInfo[CHAG_DEV_INDEX0]);
    
    //2号仓充电机
    g_sChagOpInfo[CHAG_DEV_INDEX1].mode = CHAG_MODE_INIT;
    g_sChagOpInfo[CHAG_DEV_INDEX1].pChagParams = &g_sDeviceParams.chagParams;
    Chag_GetOp(&g_sChagOpInfo[CHAG_DEV_INDEX1]);
    
    //1号仓电池
    g_sBatOpInfo[BAT_CH_INDEX0].mode = BAT_MODE_GET_INFO;
    Bat_GetOp(&g_sBatOpInfo[BAT_CH_INDEX0]);
    //2号仓电池
    g_sBatOpInfo[BAT_CH_INDEX1].mode = BAT_MODE_GET_INFO;
    Bat_GetOp(&g_sBatOpInfo[BAT_CH_INDEX1]);

    g_sDeviceGateInfo[DEVICE_GATE_INDEX0].pBatOpInfo = &g_sBatOpInfo[BAT_CH_INDEX0];
    g_sDeviceGateInfo[DEVICE_GATE_INDEX0].pChagOpInfo = &g_sChagOpInfo[BAT_CH_INDEX0];
    g_sDeviceGateInfo[DEVICE_GATE_INDEX0].interVal = DEVICE_IVT_TIME_MID;
    g_sDeviceGateInfo[DEVICE_GATE_INDEX0].rcdIdleTick = DEVICE_INFO_IDLE_TICK_CHAG;

    g_sDeviceGateInfo[DEVICE_GATE_INDEX1].pBatOpInfo = &g_sBatOpInfo[BAT_CH_INDEX1];
    g_sDeviceGateInfo[DEVICE_GATE_INDEX1].pChagOpInfo = &g_sChagOpInfo[BAT_CH_INDEX1];
    g_sDeviceGateInfo[DEVICE_GATE_INDEX0].interVal = DEVICE_IVT_TIME_MID;
    g_sDeviceGateInfo[DEVICE_GATE_INDEX0].rcdIdleTick = DEVICE_INFO_IDLE_TICK_CHAG;
}


void Device_ReadDeviceParamenter(void)
{
    BOOL b = FALSE, bBackup = FALSE;
    b = FRam_ReadBuffer(FRAME_INFO_ADDR, sizeof(DEVICE_PARAMS), (u8 *)(&g_sDeviceParams));
    if(b)
    {
        u32 crc1 = 0, crc2 = 0;

        crc1 = a_GetCrc((u8 *)(&g_sDeviceParams), (sizeof(DEVICE_PARAMS)) - 4);
        crc2 = g_sDeviceParams.crc;

        //检测参数是否正确，如果不正确，使用默认参数操作
        if(crc1 != crc2)
        {
            b = FALSE;
        }
    }  
    bBackup = FRam_ReadBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(DEVICE_PARAMS), (u8 *)(&g_sDeviceParams));
    if(bBackup)
    {
        u32 crc1 = 0, crc2 = 0;

        crc1 = a_GetCrc((u8 *)(&g_sDeviceParams), (sizeof(DEVICE_PARAMS)) - 4);
        crc2 = g_sDeviceParams.crc;

        //检测参数是否正确，如果不正确，使用默认参数操作
        if(crc1 != crc2)
        {
            bBackup = FALSE;
        }
    }

    if(b == FALSE && bBackup == FALSE)
    {
        memset(&g_sDeviceParams, 0, sizeof(DEVICE_PARAMS));

        g_sDeviceParams.ledLowVolLev = DEVICE_LED_LOWVOL_DFT;
        g_sDeviceParams.alarmTmpr = DEVICE_TMPR_ALARM_DFT;
        g_sDeviceParams.chagParams.fulVolLev = CHAG_VOL_LEV_FUL;
        g_sDeviceParams.chagParams.higVolLev = CHAG_VOL_LEV_HIG;
        g_sDeviceParams.chagParams.lowVolLev = CHAG_VOL_LEV_LOW;
        g_sDeviceParams.chagParams.stepParams[CHAG_STEP_IDX0].cur = CHAG_CUR_STEP1;
        g_sDeviceParams.chagParams.stepParams[CHAG_STEP_IDX0].vol = CHAG_VOL_STEP1;
        g_sDeviceParams.chagParams.stepParams[CHAG_STEP_IDX1].cur = CHAG_CUR_STEP2;
        g_sDeviceParams.chagParams.stepParams[CHAG_STEP_IDX1].vol = CHAG_VOL_STEP2;
        g_sDeviceParams.chagParams.stepParams[CHAG_STEP_IDX2].cur = CHAG_CUR_STEP3;
        g_sDeviceParams.chagParams.stepParams[CHAG_STEP_IDX2].vol = CHAG_VOL_STEP3;
        Device_WriteDeviceParamenter();
    }

    Fram_ReadBootParamenter();
    if((g_sFramBootParamenter.appState != FRAM_BOOT_APP_OK) ||
       (g_sFramBootParamenter.addr != g_sDeviceOpInfo.addr))
    {
        g_sFramBootParamenter.appState = FRAM_BOOT_APP_OK;
        g_sFramBootParamenter.addr = g_sDeviceOpInfo.addr;
        Fram_WriteBootParamenter();
    }  
    
    FRam_ReadKey();
    FRam_EncryptKey((u8 *)STM32_CPUID_ADDR);
}

BOOL Device_WriteDeviceParamenter(void)
{
    BOOL b = FALSE;

    g_sDeviceParams.crc = 0;
    g_sDeviceParams.crc = a_GetCrc((u8 *)(&g_sDeviceParams), (sizeof(DEVICE_PARAMS)) - 2);

    b = FRam_WriteBuffer(FRAME_INFO_ADDR, sizeof(DEVICE_PARAMS), (u8 *)(&g_sDeviceParams));
    b = FRam_WriteBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(DEVICE_PARAMS), (u8 *)(&g_sDeviceParams));
    
    return b;
}


const PORT_INF DEV_ADDR_PIN = {GPIOC, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5};      //地址引脚
void Device_InitAddr(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = DEV_ADDR_PIN.Pin;
    GPIO_Init(DEV_ADDR_PIN.Port, &GPIO_InitStructure);
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
        
       t = Device_GetAddrOut();
        
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

const PORT_INF DEV_OUTCTRL_LOCK0 = {GPIOB, GPIO_Pin_15};       
const PORT_INF DEV_OUTCTRL_LOCK1 = {GPIOA, GPIO_Pin_5};
const PORT_INF DEV_OUTCTRL_FAN0 = {GPIOA, GPIO_Pin_7};       
const PORT_INF DEV_OUTCTRL_FAN1 = {GPIOA, GPIO_Pin_8};
const PORT_INF DEV_OUTCTRL_R0 = {GPIOB, GPIO_Pin_2};       
const PORT_INF DEV_OUTCTRL_G0 = {GPIOB, GPIO_Pin_3};
const PORT_INF DEV_OUTCTRL_B0 = {GPIOB, GPIO_Pin_4};
const PORT_INF DEV_OUTCTRL_R1 = {GPIOB, GPIO_Pin_5};       
const PORT_INF DEV_OUTCTRL_G1 = {GPIOB, GPIO_Pin_8};
const PORT_INF DEV_OUTCTRL_B1 = {GPIOB, GPIO_Pin_9};         
void Device_InitOutCtrl(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = DEV_OUTCTRL_LOCK0.Pin;
    GPIO_Init(DEV_OUTCTRL_LOCK0.Port, &GPIO_InitStructure);
    Device_CtrlLock0Close();

    GPIO_InitStructure.GPIO_Pin = DEV_OUTCTRL_LOCK1.Pin;
    GPIO_Init(DEV_OUTCTRL_LOCK1.Port, &GPIO_InitStructure);
    Device_CtrlLock1Close();
    
    GPIO_InitStructure.GPIO_Pin = DEV_OUTCTRL_FAN0.Pin;
    GPIO_Init(DEV_OUTCTRL_FAN0.Port, &GPIO_InitStructure);
    Device_CtrlFan0Close();

    GPIO_InitStructure.GPIO_Pin = DEV_OUTCTRL_FAN1.Pin;
    GPIO_Init(DEV_OUTCTRL_FAN1.Port, &GPIO_InitStructure);
    Device_CtrlFan1Close();

    GPIO_InitStructure.GPIO_Pin = DEV_OUTCTRL_R0.Pin;
    GPIO_Init(DEV_OUTCTRL_R0.Port, &GPIO_InitStructure);
    Device_CtrlLedR0Off();

    GPIO_InitStructure.GPIO_Pin = DEV_OUTCTRL_G0.Pin;
    GPIO_Init(DEV_OUTCTRL_G0.Port, &GPIO_InitStructure);
    Device_CtrlLedG0Off();

    GPIO_InitStructure.GPIO_Pin = DEV_OUTCTRL_B0.Pin;
    GPIO_Init(DEV_OUTCTRL_B0.Port, &GPIO_InitStructure);
    Device_CtrlLedB0Off();

    GPIO_InitStructure.GPIO_Pin = DEV_OUTCTRL_R1.Pin;
    GPIO_Init(DEV_OUTCTRL_R1.Port, &GPIO_InitStructure);
    Device_CtrlLedR1Off();

    GPIO_InitStructure.GPIO_Pin = DEV_OUTCTRL_G1.Pin;
    GPIO_Init(DEV_OUTCTRL_G1.Port, &GPIO_InitStructure);
    Device_CtrlLedG1Off();

    GPIO_InitStructure.GPIO_Pin = DEV_OUTCTRL_B1.Pin;
    GPIO_Init(DEV_OUTCTRL_B1.Port, &GPIO_InitStructure);
    Device_CtrlLedB1Off();
}

const PORT_INF DEV_INSEN_LOCK_FB0 = {GPIOA, GPIO_Pin_12};   
const PORT_INF DEV_INSEN_LOCK_FB1 = {GPIOA, GPIO_Pin_6};
const PORT_INF DEV_INSEN_SMK_FB0 = {GPIOB, GPIO_Pin_0};   
const PORT_INF DEV_INSEN_SMK_FB1 = {GPIOB, GPIO_Pin_1}; 
const PORT_INF DEV_INSEN_BAT_FB0 = {GPIOC, GPIO_Pin_6};   
const PORT_INF DEV_INSEN_BAT_FB1 = {GPIOC, GPIO_Pin_7}; 
const PORT_INF DEV_INSEN_LOCK_RFID0 = {GPIOB, GPIO_Pin_13};   
const PORT_INF DEV_INSEN_LOCK_RFID1 = {GPIOB, GPIO_Pin_14}; 
void Device_InitInSensor(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};


    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    
    GPIO_InitStructure.GPIO_Pin = DEV_INSEN_LOCK_FB0.Pin;
    GPIO_Init(DEV_INSEN_LOCK_FB0.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = DEV_INSEN_SMK_FB0.Pin;
    GPIO_Init(DEV_INSEN_SMK_FB0.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = DEV_INSEN_BAT_FB0.Pin;
    GPIO_Init(DEV_INSEN_BAT_FB0.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = DEV_INSEN_LOCK_RFID0.Pin;
    GPIO_Init(DEV_INSEN_LOCK_RFID0.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = DEV_INSEN_LOCK_FB1.Pin;
    GPIO_Init(DEV_INSEN_LOCK_FB1.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = DEV_INSEN_SMK_FB1.Pin;
    GPIO_Init(DEV_INSEN_SMK_FB1.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = DEV_INSEN_BAT_FB1.Pin;
    GPIO_Init(DEV_INSEN_BAT_FB1.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = DEV_INSEN_LOCK_RFID1.Pin;
    GPIO_Init(DEV_INSEN_LOCK_RFID1.Port, &GPIO_InitStructure);
}

void Device_CtrlLed(u8 index, u8 color)
{
    if(index == DEVICE_GATE_INDEX0)
    {
        g_sDeviceOpInfo.outCtrl.ledRed0 = (color >> 2) & 0x01;
        g_sDeviceOpInfo.outCtrl.ledGreen0 = (color >> 1) & 0x01;
        g_sDeviceOpInfo.outCtrl.ledBlue0 = (color >> 0) & 0x01;

        Device_CtrlLed0(g_sDeviceOpInfo.outCtrl.ledRed0, g_sDeviceOpInfo.outCtrl.ledGreen0, g_sDeviceOpInfo.outCtrl.ledBlue0);
        
        g_sDeviceGateInfo[DEVICE_GATE_INDEX0].ledColor = color;
    }
    else if(index == DEVICE_GATE_INDEX1)
    {
        g_sDeviceOpInfo.outCtrl.ledRed1 = (color >> 2) & 0x01;
        g_sDeviceOpInfo.outCtrl.ledGreen1 = (color >> 1) & 0x01;
        g_sDeviceOpInfo.outCtrl.ledBlue1 = (color >> 0) & 0x01;

        Device_CtrlLed1(g_sDeviceOpInfo.outCtrl.ledRed1, g_sDeviceOpInfo.outCtrl.ledGreen1, g_sDeviceOpInfo.outCtrl.ledBlue1);
        
        g_sDeviceGateInfo[DEVICE_GATE_INDEX1].ledColor = color;
    }
    else if(index == DEVICE_GATE_NUM)
    {
        g_sDeviceOpInfo.outCtrl.ledRed0 = (color >> 2) & 0x01;
        g_sDeviceOpInfo.outCtrl.ledGreen0 = (color >> 1) & 0x01;
        g_sDeviceOpInfo.outCtrl.ledBlue0 = (color >> 0) & 0x01;
        g_sDeviceOpInfo.outCtrl.ledRed1 = (color >> 2) & 0x01;
        g_sDeviceOpInfo.outCtrl.ledGreen1 = (color >> 1) & 0x01;
        g_sDeviceOpInfo.outCtrl.ledBlue1 = (color >> 0) & 0x01;

        Device_CtrlLed0(g_sDeviceOpInfo.outCtrl.ledRed0, g_sDeviceOpInfo.outCtrl.ledGreen0, g_sDeviceOpInfo.outCtrl.ledBlue0);
        Device_CtrlLed1(g_sDeviceOpInfo.outCtrl.ledRed1, g_sDeviceOpInfo.outCtrl.ledGreen1, g_sDeviceOpInfo.outCtrl.ledBlue1);

        g_sDeviceGateInfo[DEVICE_GATE_INDEX0].ledColor = color;
        g_sDeviceGateInfo[DEVICE_GATE_INDEX1].ledColor = color;
    }
}

void Device_CtrlDoor(BOOL bOpen, u8 index)
{
    if(index == DEVICE_GATE_INDEX0)
    {
        g_sDeviceOpInfo.outCtrl.doorLock0 = bOpen;
        Device_CtrlLock0(bOpen);
    }
    else if(index == DEVICE_GATE_INDEX1)
    {
        g_sDeviceOpInfo.outCtrl.doorLock1 = bOpen;
        Device_CtrlLock1(bOpen);
    }
}

void Device_CtrlFan(BOOL bOpen, u8 index)
{
    if(index == DEVICE_GATE_INDEX0)
    {
        g_sDeviceOpInfo.outCtrl.fanCtrl0 = bOpen;
        Device_CtrlFan0(bOpen);
    }
    else if(index == DEVICE_GATE_INDEX1)
    {
        g_sDeviceOpInfo.outCtrl.fanCtrl1 = bOpen;
        Device_CtrlFan1(bOpen);
    }
}

u8 Device_CheckUsrFan(DEVICE_OUTCTRL *pOutCtrl, u8 index)
{
    if(index == DEVICE_GATE_INDEX0)
    {
        return pOutCtrl->fanCtrl0;
    }
    else
    {
        return pOutCtrl->fanCtrl1;
    }
}


void Device_ReadInInfo(DEVICE_STATINFO *pStateInfo, u8 index)
{
    if(index == DEVICE_GATE_INDEX0)
    {
        if(Device_GetLockFB0())
        {
            pStateInfo->door = DEVICE_DOOR_OPEN;
        }
        else
        {
            pStateInfo->door = DEVICE_DOOR_CLOSE;
        }

        if(Device_GetRfidFB0())
        {
            pStateInfo->rfid = DEVICE_RFID_OPEN;
        }
        else
        {
            pStateInfo->rfid = DEVICE_RFID_CLOSE;
        }

        if(Device_GetSmokeFB0())
        {
            pStateInfo->smoke = DEVICE_SMOKE_ALARM;
        }
        else
        {
            pStateInfo->smoke = DEVICE_SMOKE_NORM;
        }
    }
    else if(index == DEVICE_GATE_INDEX1)
    {
        if(Device_GetLockFB1())
        {
            pStateInfo->door = DEVICE_DOOR_OPEN;
        }
        else
        {
            pStateInfo->door = DEVICE_DOOR_CLOSE;
        }

        if(Device_GetRfidFB1())
        {
            pStateInfo->rfid = DEVICE_RFID_OPEN;
        }
        else
        {
            pStateInfo->rfid = DEVICE_RFID_CLOSE;
        }

        if(Device_GetSmokeFB1())
        {
            pStateInfo->smoke = DEVICE_SMOKE_ALARM;
        }
        else
        {
            pStateInfo->smoke = DEVICE_SMOKE_NORM;
        }
    }
}

u32 Device_CheckInSensor(DEVICE_INCHECK *pInCheck, u8 *pUsrState)
{
    u32 changeFlag = 0;
    u8 i = 0;
    u16 t = 0, checkFlag = 0;
    u16 flag = 0;
    t = *((u32 *)(&pInCheck->curInState));
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
                *pUsrState &= (~flag);
                *pUsrState |= (t & flag);
                
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

void Device_FomatGateRecord(u8 index, DEVICE_GATEINFO *pGateInfo, DEVICE_INFOBUF *pRcdBuf)
{
    memset(&pRcdBuf->tmp, 0, sizeof(DEVICE_STATINFO));
    
    pGateInfo->rcdFlag = 0;
    
    pRcdBuf->tmp.addr = index;
    pRcdBuf->tmp.sensorState = pGateInfo->sensorState;
    //温度
    if(pGateInfo->tmpr.t > 0)
    {
        pRcdBuf->tmp.tmpr = pGateInfo->tmpr.t;
    }
    else
    {
        pRcdBuf->tmp.tmpr = 0x80;
        pRcdBuf->tmp.tmpr |= (~pGateInfo->tmpr.t);
        pRcdBuf->tmp.tmpr += 1;
    }
    memcpy(&pRcdBuf->tmp.batInfo, &pGateInfo->pBatOpInfo->batInfo, sizeof(BAT_INFO));
    memcpy(&pRcdBuf->tmp.chagInfo, &pGateInfo->pChagOpInfo->chagInfo, sizeof(CHAG_INFO));

    pGateInfo->rcdIdleDelay = g_nSysTick;
    Device_AddRecordInfo(pRcdBuf, &pRcdBuf->tmp);
}

void Device_AddRecordInfo(DEVICE_INFOBUF *pRcdBuf, DEVICE_SENSORINFO *pSensorInfo)
{
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

u16 Device_GetInInfoRspFrame(DEVICE_INFOBUF *pInfoBuf, u16 addr, u8 *pBuffer)
{
    u16 pos = 0;
    u16 crc = 0;
    u8 num = 0, i = 0;
    u16 len = 0;
    
    num = (pInfoBuf->s + DEVICE_INFO_BUF_NUM - pInfoBuf->e) % DEVICE_INFO_BUF_NUM;
    if(num * sizeof(DEVICE_SENSORINFO) > UART_DATA_MAX_LEN)
    {
        num = UART_DATA_MAX_LEN / sizeof(DEVICE_SENSORINFO);
    }
    len = 1 + num * sizeof(DEVICE_SENSORINFO) + 2;

    pBuffer[pos++] = UART_FRAME_FLAG_HEAD1;     // frame head first byte
    pBuffer[pos++] = UART_FRAME_FLAG_HEAD2;     // frame haed second byte
    pBuffer[pos++] = 0x00;                      // length
    pBuffer[pos++] = (addr >> 0) & 0xFF; 
    pBuffer[pos++] = (addr >> 8) & 0xFF;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = UART_FRAME_RESPONSE_FLAG;
    pBuffer[pos++] = UART_FRAME_CMD_GET_ININFO;                       // cmd
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;      // RFU

    pBuffer[pos++] = (len >> 0) & 0xFF;
    pBuffer[pos++] = (len >> 8) & 0xFF;
    
    pBuffer[pos++] = num;
    for(i = 0; i < num; i++)
    {
        memcpy(pBuffer + pos, pInfoBuf->buf + pInfoBuf->e, sizeof(DEVICE_SENSORINFO));
        pos += sizeof(DEVICE_SENSORINFO);
        //if(pInfoBuf->buf[pInfoBuf->e].batState)

        pInfoBuf->e++;
        if(pInfoBuf->e == DEVICE_INFO_BUF_NUM)
        {
            pInfoBuf->e = 0;
        }
    }
    

    pBuffer[pos++] = UART_FRAME_FLAG_OK;
    pBuffer[pos++] = UART_FRAME_RSP_NOERR;
    
    crc = a_GetCrc(pBuffer + UART_FRAME_POS_LEN, pos - 2); //从LEN开始计算crc
    pBuffer[pos++] = (crc >> 0) & 0xFF;
    pBuffer[pos++] = (crc >> 8) & 0xFF;

    return pos;
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
        case UART_FRAME_CMD_SET_PARAMS:
            if(paramsLen == DEVICE_GATE_PARAMS_LEN)
            {
                u8 versionInfo[4] = {0}; 
                if(memcmp(pParams, &g_sDeviceParams.rfu0, DEVICE_GATE_PARAMS_LEN) != 0)
                {
                    memcpy(&g_sDeviceParams.rfu0, pParams, DEVICE_GATE_PARAMS_LEN);
                    Device_WriteDeviceParamenter();
                }
                 a_Str2Hex((char *)DEVICE_VERSION_INFO, versionInfo);
                
                g_sR485TxFrame.len = Uart_UsrResponseFrame(versionInfo, 4, cmd, g_sDeviceOpInfo.addr, g_sR485TxFrame.buffer);
            }
            break;
        case UART_FRAME_CMD_GET_PARAMS:
            g_sR485TxFrame.len = Uart_UsrResponseFrame((u8 *)&g_sDeviceParams.rfu0, DEVICE_GATE_PARAMS_LEN, cmd, g_sDeviceOpInfo.addr, g_sR485TxFrame.buffer);
            break;
        case UART_FRAME_CMD_GET_ININFO:
            g_sR485TxFrame.len = Device_GetInInfoRspFrame(&g_sDeviceInfoBuf, g_sDeviceOpInfo.addr, g_sR485TxFrame.buffer);
            break;
        case UART_FRAME_CMD_SET_OUTINFO:
            if(paramsLen == 2)
            {
                u8 index = 0;
                index = pParams[0];
                if(index < DEVICE_GATE_NUM)
                {
                    if(pParams[1] & DEVICE_OUT_CTRL_POS_DOOR)
                    {
                        g_sDeviceGateInfo[index].ctrlDoor.bOpen = TRUE;
                        g_sDeviceGateInfo[index].ctrlDoor.tick = g_nSysTick;
                        Device_CtrlDoor(TRUE, index);
                        //Device_Delayms(100);             //第二仓开仓问题   ,鎖邏輯反
                    }
                    //Device_CtrlFan((BOOL)(pParams[1] & DEVICE_OUT_CTRL_POS_FAN), index);
                    
                    g_sDeviceOpInfo.usrCtrl = g_sDeviceOpInfo.outCtrl;
                    g_sR485TxFrame.len = Uart_UsrResponseFrame(NULL, 0, cmd, g_sDeviceOpInfo.addr, g_sR485TxFrame.buffer);
                }
            }
            break;
        case UART_FRAME_CMD_CHARGE:
            if(paramsLen == 2)
            {
                u8 index = 0;
                index = pParams[0];
                if(index < DEVICE_GATE_NUM)
                {
                    if(pParams[1] == 1)
                    {
                        Device_StartCharge(&g_sDeviceGateInfo[index], index);
                    }
                    else
                    {
                        Device_StopCharge(&g_sDeviceGateInfo[index], index);
                    }
                    g_sR485TxFrame.len = Uart_UsrResponseFrame(NULL, 0, cmd, g_sDeviceOpInfo.addr, g_sR485TxFrame.buffer);
                }
            }
            break;
        case UART_FRAME_CMD_RTNBAT:
            if(paramsLen == 1 + BAT_SN_LEN)
            {
                u8 index = pParams[0];
                u8 buf[2] = {0};
                buf[0] = DEVICE_RTNBAT_STEP_OVER;
                buf[1] = DEVICE_RTNBAT_ERR_PARAMS;
                if(index < DEVICE_GATE_NUM)
                {

                    if(Bat_IsExist(g_sDeviceGateInfo[index].pBatOpInfo) == FALSE)
                    {
                        Device_ClearRcdBuf();
                        
                        memcpy(g_sDeviceOpInfo.rtnBatSn, pParams + 1, BAT_SN_LEN);
                        
                        g_sDeviceGateInfo[index].rtnBatStep = DEVICE_RTNBAT_STEP_OPENDOR;
                        g_sDeviceGateInfo[index].rtnTick = g_nSysTick;
                        g_sDeviceGateInfo[index].rtnOpenDoorRpt = 0;
                        
                        g_sDeviceGateInfo[index].mode = DEVICE_GATE_MODE_RTNBAT;
                        
                        g_sDeviceGateInfo[index].ctrlDoor.bOpen = TRUE;
                        g_sDeviceGateInfo[index].ctrlDoor.tick = g_nSysTick;
                        g_sDeviceGateInfo[index].interVal = DEVICE_IVT_TIME_FAST;
                        g_sDeviceGateInfo[index].tick = g_nSysTick;
                        Device_CtrlDoor(TRUE, index);

                        //默认就是等待开启仓门
                        buf[0] = DEVICE_RTNBAT_STEP_OPENDOR;
                        buf[1] = DEVICE_RTNBAT_ERR_WAIT;
                        g_sR485TxFrame.len = Uart_UsrResponseFrame(buf, 2, cmd, g_sDeviceOpInfo.addr, g_sR485TxFrame.buffer);
                    }
                    else
                    {
                        g_sR485TxFrame.len = Uart_UsrResponseFrame(buf, 2, cmd, g_sDeviceOpInfo.addr, g_sR485TxFrame.buffer);
                    }
                }
                else
                {
                    g_sR485TxFrame.len = Uart_UsrResponseFrame(buf, 2, cmd, g_sDeviceOpInfo.addr, g_sR485TxFrame.buffer);
                }
            }
            break;
        case UART_FRAME_CMD_BRWBAT:
            if(paramsLen == 1 + BAT_SN_LEN)
            {
                u8 index = pParams[0];
                u8 buf[2] = {0};
                buf[0] = DEVICE_BRWBAT_STEP_OVER;
                buf[1] = DEVICE_BRWBAT_ERR_PARAMS;
                    
                index = pParams[0];
                if(index < DEVICE_GATE_NUM)
                {
                    if(Bat_IsExist(g_sDeviceGateInfo[index].pBatOpInfo) &&
                        (memcmp(g_sDeviceGateInfo[index].pBatOpInfo->batInfo.sn, pParams + 1, BAT_SN_LEN) == 0 ||
                         memcmp(g_nBatBoardcastSn, pParams + 1, BAT_SN_LEN) == 0))
                    {
                        Device_ClearRcdBuf();
                        
                        memcpy(g_sDeviceOpInfo.brwBatSn, pParams + 1, BAT_SN_LEN);
                        
                        g_sDeviceGateInfo[index].brwBatStep = DEVICE_BRWBAT_STEP_OPENDOR;
                        g_sDeviceGateInfo[index].brwTick = g_nSysTick;
                        
                        g_sDeviceGateInfo[index].mode = DEVICE_GATE_MODE_BRWBAT;
                        
                        g_sDeviceGateInfo[index].ctrlDoor.bOpen = TRUE;
                        g_sDeviceGateInfo[index].ctrlDoor.tick = g_nSysTick;
                        g_sDeviceGateInfo[index].interVal = DEVICE_IVT_TIME_FAST;
                        g_sDeviceGateInfo[index].tick = g_nSysTick;
                        Device_CtrlDoor(TRUE, index);
                        //默认就是等待开启仓门
                        buf[0] = DEVICE_BRWBAT_STEP_OPENDOR;
                        buf[1] = DEVICE_BRWBAT_ERR_WAIT;
                        g_sR485TxFrame.len = Uart_UsrResponseFrame(buf, 2, cmd, g_sDeviceOpInfo.addr, g_sR485TxFrame.buffer);  //这里应该回复等待开门
                    }
                    else
                    {
                        g_sR485TxFrame.len = Uart_UsrResponseFrame(buf, 2, cmd, g_sDeviceOpInfo.addr, g_sR485TxFrame.buffer);
                    }
                }
                else
                {
                    g_sR485TxFrame.len = Uart_UsrResponseFrame(buf, 2, cmd, g_sDeviceOpInfo.addr, g_sR485TxFrame.buffer);
                }
            }
            break;
            case UART_FRAME_CMD_PLANE_BAT:
            if(paramsLen == 1 + BAT_SN_LEN + 1)
            {
                u8 index = 0;
                index = pParams[0];
                u8 buf[1] = {0};
                buf[0] = DEVICE_PLANE_BAT_FAIL;
                if(index < DEVICE_GATE_NUM)
                {
                  
                    if(Bat_IsExist(g_sDeviceGateInfo[index].pBatOpInfo) == TRUE  &&
                       (memcmp(g_sDeviceGateInfo[index].pBatOpInfo->batInfo.sn, pParams + 1, BAT_SN_LEN) == 0 ||    
                        memcmp(g_nBatBoardcastSn, pParams + 1, BAT_SN_LEN) == 0))
                    {
                        g_sDeviceParams.planeBat[index] = pParams[1 + BAT_SN_LEN];
                        buf[0] = DEVICE_PLANE_BAT_OK;
                        Device_WriteDeviceParamenter();
                        Device_GateRefreshLed(g_sDeviceGateInfo + index, index);
                        g_sR485TxFrame.len = Uart_UsrResponseFrame(buf, 1, cmd, g_sDeviceOpInfo.addr, g_sR485TxFrame.buffer);
                    }
                    else
                    {
                        g_sR485TxFrame.len = Uart_UsrResponseFrame(buf, 1, cmd, g_sDeviceOpInfo.addr, g_sR485TxFrame.buffer);
                    }
                }
            }
            break;
        default:
            break;
    }

    return g_sR485TxFrame.len;
}


void Device_StartCharge(DEVICE_GATEINFO *pGateInfo, u8 index)
{
    BAT_OPINFO *pBatOpInfo = pGateInfo->pBatOpInfo;
    CHAG_OPINFO *pChagOpInfo = pGateInfo->pChagOpInfo;
    
    pBatOpInfo->mode = BAT_MODE_CHARGE;
    pBatOpInfo->batInfo.state |= BAT_STAT_CHARGE;
    pBatOpInfo->batInfo.state &= (~BAT_STAT_FULL);
    Bat_GetOp(pBatOpInfo);

    pChagOpInfo->mode = CHAG_MODE_OPEN;
    pChagOpInfo->chagInfo.state |= CHAG_STAT_CHARGE;
    pChagOpInfo->batVolLv = pBatOpInfo->batInfo.status.volLev;
    Chag_GetOp(pChagOpInfo);

    pGateInfo->mode = DEVICE_GATE_MODE_CHARGE;
    pGateInfo->interVal = DEVICE_IVT_TIME_MID;
    pGateInfo->rcdFlag = DEVICE_RCD_FLAG_SENSOR;            //产生记录
    
    
    pGateInfo->rcdIdleTick = DEVICE_INFO_IDLE_TICK_CHAG;
}

void Device_Charge(BAT_OPINFO *pBatOpInfo, CHAG_OPINFO *pChagOpInfo, u8 index)
{
    pBatOpInfo->mode = BAT_MODE_CHARGE;
    Bat_GetOp(pBatOpInfo);
    if(pBatOpInfo->batInfo.status.volLev >= g_sDeviceParams.chagParams.fulVolLev)
    {
        pBatOpInfo->batInfo.state |= BAT_STAT_FULL;
    }

    pChagOpInfo->mode = CHAG_MODE_CHARGE;
    pChagOpInfo->batVolLv = pBatOpInfo->batInfo.status.volLev;
    Chag_GetOp(pChagOpInfo);
}

void Device_StopCharge(DEVICE_GATEINFO *pGateInfo, u8 index)
{
    BAT_OPINFO *pBatOpInfo = pGateInfo->pBatOpInfo;
    CHAG_OPINFO *pChagOpInfo = pGateInfo->pChagOpInfo;
    
    pBatOpInfo->batInfo.state &= (~BAT_STAT_CHARGE);
    if(pBatOpInfo->batInfo.status.volLev >= g_sDeviceParams.chagParams.fulVolLev)
    {
        pBatOpInfo->batInfo.state |= BAT_STAT_FULL;
    }

    pChagOpInfo->mode = CHAG_MODE_CLOSE;
    pChagOpInfo->chagInfo.state &= (~CHAG_STAT_CHARGE);
    Chag_GetOp(pChagOpInfo);

    pGateInfo->mode = DEVICE_GATE_MODE_IDLE;
    pGateInfo->interVal = DEVICE_IVT_TIME_SLOW;
    pGateInfo->rcdFlag = DEVICE_RCD_FLAG_SENSOR;            //产生记录
    
    pGateInfo->rcdIdleTick = DEVICE_INFO_IDLE_TICK_NORMAL;
}

void Device_CheckCharge(BAT_OPINFO *pBatOpInfo, CHAG_OPINFO *pChagOpInfo, u8 index)
{
    pBatOpInfo->mode = BAT_MODE_CHK_STATUS;
    Bat_GetOp(pBatOpInfo);
    if(pBatOpInfo->batInfo.status.volLev >= g_sDeviceParams.chagParams.fulVolLev)
    {
        pBatOpInfo->batInfo.state |= BAT_STAT_FULL;
    }

    pChagOpInfo->mode = CHAG_MODE_IDLE;
    Chag_GetOp(pChagOpInfo);
}

void Device_OpBatRspFrame(u8 cmd, u8 step, u8 err)
{
    u8 buf[2] = {0};
    buf[0] = step;
    buf[1] = err;
    g_sR485TxFrame.len = Uart_UsrResponseFrame(buf, 2, cmd, g_sDeviceOpInfo.addr, g_sR485TxFrame.buffer);

    g_sR485TxFrame.state = UART_STAT_TX_BUSY;
    g_sR485TxFrame.tick = g_nSysTick;
    R485_EnableTxDma(g_sR485TxFrame.buffer, g_sR485TxFrame.len);
}

void Device_RtnWaitBat(BAT_OPINFO *pBatOpInfo, CHAG_OPINFO *pChagOpInfo, u8 index)
{
    pBatOpInfo->mode = BAT_MODE_GET_INFO;
    Bat_GetOp(pBatOpInfo);

    pChagOpInfo->mode = CHAG_MODE_OPEN;     //开启电源预充
    pChagOpInfo->batVolLv = 0;
    Chag_GetOp(pChagOpInfo);
}

void Device_BrwWaitBat(BAT_OPINFO *pBatOpInfo, CHAG_OPINFO *pChagOpInfo, u8 index)
{
    pBatOpInfo->mode = BAT_MODE_GET_INFO;
    Bat_GetOp(pBatOpInfo);

    pChagOpInfo->mode = CHAG_MODE_CLOSE;     //关闭充电机
    pChagOpInfo->batVolLv = 0;
    Chag_GetOp(pChagOpInfo);
}


void Device_GateRefreshLed(DEVICE_GATEINFO *pGateInfo, u8 index)
{
    u8 ledColor = 0;
    if(Chag_IsExist(pGateInfo->pChagOpInfo) == FALSE)                                   //充电机不存在
    {
        ledColor = DEVICE_LED_COLOR_YELLOW;
    }
    else if(Bat_IsExist(pGateInfo->pBatOpInfo) == FALSE)                                //电池不存在
    {
        ledColor = 0;                                                                   //这里以后是不是需要一个电池在位检测的信号
    }
    else
    {
        if(Bat_IsErr(pGateInfo->pBatOpInfo) || Chag_IsErr(pGateInfo->pChagOpInfo))      //电池故障
        {
            ledColor = DEVICE_LED_COLOR_WHITE;
        }
        else
        {
            if(pGateInfo->pBatOpInfo->batInfo.status.volLev  > g_sDeviceParams.ledLowVolLev)    //电压指示
            {
                if(g_sDeviceParams.planeBat[index])
                {
                    ledColor = DEVICE_LED_COLOR_PAPER;
                }
                else
                {
                    if(Bat_IsFull(pGateInfo->pBatOpInfo))
                    {
                        ledColor = DEVICE_LED_COLOR_GREEN;
                    }
                    else
                    {
                        ledColor = DEVICE_LED_COLOR_BLUE;
                    }
                }
            }
            else
            {
                ledColor = DEVICE_LED_COLOR_RED;
            }
        }
    }
    if(ledColor != pGateInfo->ledColor)
    {
        Device_CtrlLed(index, ledColor);
    }
}

void Device_GateStartup(DEVICE_GATEINFO *pGateInfo, u8 index)
{
    if(Chag_IsExist(pGateInfo->pChagOpInfo))              //充电机和电池都在位，并且正常，这里还要判断消防和门等状态是否正常！！！！！！
    {
        if(Bat_IsExist(pGateInfo->pBatOpInfo))
        {
            if(pGateInfo->pBatOpInfo->batInfo.status.volLev >= g_sDeviceParams.chagParams.fulVolLev)  //满电，结束
            {
                Device_StopCharge(pGateInfo, index);
            }
            else
            {
                Device_StartCharge(pGateInfo, index);
            }
        }
        else 
        {
        	if(pGateInfo->pChagOpInfo->chagTime + CHAG_LOW_VOL_MIN_TIM < g_nSysTick)
        	{
            	Device_StopCharge(pGateInfo, index);
    		}
			else
			{
			
			}
        }
    }
    else
    { 
        Device_StopCharge(pGateInfo, index);
    }
}

void Device_GateCharge(DEVICE_GATEINFO *pGateInfo, u8 index)
{
    //设备故障或者设备不存在
    if(Bat_IsErr(pGateInfo->pBatOpInfo) || Chag_IsErr(pGateInfo->pChagOpInfo) || Chag_IsExist(pGateInfo->pChagOpInfo) == FALSE || Bat_IsExist(pGateInfo->pBatOpInfo) == FALSE)
    {
        Device_StopCharge(pGateInfo, index);
    }
    else //充电机和电池都在位，并且正常
    {
        if(Bat_IsFull(pGateInfo->pBatOpInfo))
        {
            Device_StopCharge(pGateInfo, index);
        }
        else
        {
            Device_Charge(pGateInfo->pBatOpInfo, pGateInfo->pChagOpInfo, index);
        }
    }

}

void Device_GateReturnBat(DEVICE_GATEINFO *pGateInfo, u8 index)
{
    if(pGateInfo->rtnBatStep == DEVICE_RTNBAT_STEP_OPENDOR)                                     //开锁流程接收
    {
        if( pGateInfo->sensorState.door == DEVICE_DOOR_CLOSE || pGateInfo->ctrlDoor.bOpen)       //开锁未完成 pGateInfo->sensorState.rfid == DEVICE_RFID_CLOSE || 
        {
            if(pGateInfo->rtnTick + DEVICE_OPENDOR_TIM < g_nSysTick)                            //开锁超过时间
            {
                Device_OpBatRspFrame(UART_FRAME_CMD_RTNBAT, DEVICE_BRWBAT_STEP_OVER, DEVICE_RTNBAT_ERR_OPENDOR);     //失败，仓门未打开
                //关闭充电
                Device_StopCharge(pGateInfo, index);
            }
            else
            {
                Device_OpBatRspFrame(UART_FRAME_CMD_RTNBAT, DEVICE_BRWBAT_STEP_OPENDOR, DEVICE_RTNBAT_ERR_WAIT);     //开仓门：等待结果
            }
        }
        else
        {
            Device_OpBatRspFrame(UART_FRAME_CMD_RTNBAT, DEVICE_BRWBAT_STEP_OPENDOR, DEVICE_BRWBAT_ERR_OK);              //成功
            
            pGateInfo->rtnBatStep = DEVICE_RTNBAT_STEP_CHKBAT;                              //等待电池插入
            pGateInfo->rtnTick = g_nSysTick;
            Device_RtnWaitBat(pGateInfo->pBatOpInfo, pGateInfo->pChagOpInfo, index);
        }
    }
    else if(pGateInfo->rtnBatStep == DEVICE_RTNBAT_STEP_CHKBAT)                             //等待电池
    {
        //仓门未关闭，这个仓控没有办法控制，只能等待，等待期间搜索电池，并且开启充电
        if(pGateInfo->sensorState.door == DEVICE_DOOR_OPEN )    // || pGateInfo->sensorState.rfid == DEVICE_RFID_OPEN                             
        {
            if(Bat_IsExist(pGateInfo->pBatOpInfo))
            {
                Device_OpBatRspFrame(UART_FRAME_CMD_RTNBAT, DEVICE_RTNBAT_STEP_CHKBAT, DEVICE_RTN_CHKBAT_ERR_BATEXIT);      //电池已经插入，等待关门
            }
            else
            {
                Device_OpBatRspFrame(UART_FRAME_CMD_RTNBAT, DEVICE_RTNBAT_STEP_CHKBAT, DEVICE_RTNBAT_ERR_WAIT);             //等待关门，等待电池插入
            }
            Device_RtnWaitBat(pGateInfo->pBatOpInfo, pGateInfo->pChagOpInfo, index);
        }
        else
        {
            //关门，电池插入，并且电池正确
            if(Bat_IsExist(pGateInfo->pBatOpInfo))
            {
                //SN匹配，或者
                if(memcmp(pGateInfo->pBatOpInfo->batInfo.sn, g_sDeviceOpInfo.rtnBatSn, BAT_SN_LEN) == 0 ||
                   memcmp(g_nBatBoardcastSn, g_sDeviceOpInfo.rtnBatSn, BAT_SN_LEN) == 0)
                {
                    Device_OpBatRspFrame(UART_FRAME_CMD_RTNBAT, DEVICE_RTNBAT_STEP_OVER, DEVICE_RTNBAT_ERR_OK);             //电池正确。结束
                    //还电池正常，开始充电流程
                    Device_StartCharge(pGateInfo, index);
                }
                else
                {
                    //电池编号错误
                    Device_OpBatRspFrame(UART_FRAME_CMD_RTNBAT, DEVICE_RTNBAT_STEP_CHKBAT, DEVICE_RTN_CHKBAT_ERR_BATSN);    //非法电池
                    //这里加一个延时------------------------------------------------------------------
                    WDG_FeedIWDog()Device_Delayms(500);WDG_FeedIWDog()Device_Delayms(500);WDG_FeedIWDog()Device_Delayms(500);WDG_FeedIWDog()Device_Delayms(500);
                    //重新弹开门
                    pGateInfo->rtnBatStep = DEVICE_RTNBAT_STEP_OPENDOR;
                    pGateInfo->rtnTick = g_nSysTick;
                    
                    pGateInfo->ctrlDoor.bOpen = TRUE;
                    pGateInfo->ctrlDoor.tick = g_nSysTick;
                    pGateInfo->interVal = DEVICE_IVT_TIME_FAST;
                    pGateInfo->tick = g_nSysTick;
                    Device_CtrlDoor(TRUE, index);
                }
            }
            else
            {
                pGateInfo->rtnOpenDoorRpt++;
                if(pGateInfo->rtnOpenDoorRpt <= DEVICE_RTN_BAT_CHK_RPT)
                {
                    Device_OpBatRspFrame(UART_FRAME_CMD_RTNBAT, DEVICE_RTNBAT_STEP_CHKBAT, DEVICE_RTN_CHKBAT_ERR_BATNO);      //没有电池，继续等待
                    //这里加一个延时------------------------------------------------------------------
                    WDG_FeedIWDog()Device_Delayms(500);WDG_FeedIWDog()Device_Delayms(500);WDG_FeedIWDog()Device_Delayms(500);WDG_FeedIWDog()Device_Delayms(500);
                    //重新弹开门
                    pGateInfo->rtnBatStep = DEVICE_RTNBAT_STEP_OPENDOR;
                    pGateInfo->rtnTick = g_nSysTick;
                    
                    pGateInfo->ctrlDoor.bOpen = TRUE;
                    pGateInfo->ctrlDoor.tick = g_nSysTick;
                    pGateInfo->interVal = DEVICE_IVT_TIME_FAST;
                    pGateInfo->tick = g_nSysTick;
                    Device_CtrlDoor(TRUE, index);
                }
                else
                {
                    Device_OpBatRspFrame(UART_FRAME_CMD_RTNBAT, DEVICE_RTNBAT_STEP_OVER, DEVICE_RTNBAT_ERR_BATNO);          //没有电池，结束还电池操作
                    //关闭充电（前面打开了充电电压，这里要关闭）
                    Device_StopCharge(pGateInfo, index);
                }
            }
        }
    }
    
    pGateInfo->rcdIdleDelay = g_nSysTick;        //借还电池不需要产生记录
    pGateInfo->rcdFlag = 0;
}

void Device_GateBorrowBat(DEVICE_GATEINFO *pGateInfo, u8 index)
{
    if(pGateInfo->brwBatStep == DEVICE_BRWBAT_STEP_OPENDOR)                                 //开锁流程接收
    {
        if( pGateInfo->sensorState.door == DEVICE_DOOR_CLOSE || pGateInfo->ctrlDoor.bOpen)   //开锁未完成 pGateInfo->sensorState.rfid == DEVICE_RFID_CLOSE || 
        {
            if(pGateInfo->brwTick + DEVICE_OPENDOR_TIM < g_nSysTick)                    //开锁超过时间
            {
                Device_OpBatRspFrame(UART_FRAME_CMD_BRWBAT, DEVICE_BRWBAT_STEP_OVER, DEVICE_BRWBAT_ERR_OPENDOR);        //失败，仓门未打开
                //关闭充电
                Device_StopCharge(pGateInfo, index);                                    //结束
            }
            else
            {
                Device_OpBatRspFrame(UART_FRAME_CMD_BRWBAT, DEVICE_BRWBAT_STEP_OPENDOR, DEVICE_BRWBAT_ERR_WAIT);        //开仓门：等待结果
            }
        }
        else 
        {
            Device_OpBatRspFrame(UART_FRAME_CMD_BRWBAT, DEVICE_BRWBAT_STEP_OPENDOR, DEVICE_BRWBAT_ERR_OK);              //开仓门：成功
            //下一步
            pGateInfo->brwBatStep = DEVICE_BRWBAT_STEP_CHKBAT;                              
            pGateInfo->brwTick = g_nSysTick;
            Device_BrwWaitBat(pGateInfo->pBatOpInfo, pGateInfo->pChagOpInfo, index);
        }
    }
    else if(pGateInfo->brwBatStep == DEVICE_BRWBAT_STEP_CHKBAT)                             //等待电池
    {
        //仓门未关闭，这个仓控没有办法控制，只能等待，等待期间搜索电池，并且开启充电
        if(pGateInfo->sensorState.door == DEVICE_DOOR_OPEN )   // || pGateInfo->sensorState.rfid == DEVICE_RFID_OPEN                              
        {
            if(Bat_IsExist(pGateInfo->pBatOpInfo))
            {
                Device_OpBatRspFrame(UART_FRAME_CMD_BRWBAT, DEVICE_BRWBAT_STEP_CHKBAT, DEVICE_BRWBAT_ERR_WAIT);             //仓门未关闭，电池未取走
            }
            else
            {
                Device_OpBatRspFrame(UART_FRAME_CMD_BRWBAT, DEVICE_BRWBAT_STEP_CHKBAT, DEVICE_BRW_CHKBAT_ERR_BATNO);            //仓门未关闭，电池取走
            }
            Device_BrwWaitBat(pGateInfo->pBatOpInfo, pGateInfo->pChagOpInfo, index);
        }
        else
        {
            //关门，电池插入，并且电池正确
            if(Bat_IsExist(pGateInfo->pBatOpInfo))
            {
                //SN匹配，表示用户放弃了借出电池
                if(memcmp(pGateInfo->pBatOpInfo->batInfo.sn, g_sDeviceOpInfo.brwBatSn, BAT_SN_LEN) == 0 ||
                   memcmp(g_nBatBoardcastSn, g_sDeviceOpInfo.brwBatSn, BAT_SN_LEN) == 0)
                {
                    Device_OpBatRspFrame(UART_FRAME_CMD_BRWBAT, DEVICE_BRWBAT_STEP_OVER, DEVICE_BRWBAT_ERR_BATEXIST);        //仓门关闭，电池未取走
                    Device_StopCharge(pGateInfo, index);
                }
                else
                {
                    //电池编号错误
                    Device_OpBatRspFrame(UART_FRAME_CMD_BRWBAT, DEVICE_BRWBAT_STEP_CHKBAT, DEVICE_BRW_CHKBAT_ERR_BATSN);
                    //这里加一个延时------------------------------------------------------------------
                    WDG_FeedIWDog()Device_Delayms(500);WDG_FeedIWDog()Device_Delayms(500);WDG_FeedIWDog()Device_Delayms(500);WDG_FeedIWDog()Device_Delayms(500);
                    //重新弹开门
                    pGateInfo->brwBatStep = DEVICE_BRWBAT_STEP_OPENDOR;
                    pGateInfo->brwTick = g_nSysTick;
                    
                    pGateInfo->ctrlDoor.bOpen = TRUE;
                    pGateInfo->ctrlDoor.tick = g_nSysTick;
                    pGateInfo->interVal = DEVICE_IVT_TIME_FAST;
                    pGateInfo->tick = g_nSysTick;
                    Device_CtrlDoor(TRUE, index);
                }
            }
            else
            {
                Device_OpBatRspFrame(UART_FRAME_CMD_BRWBAT, DEVICE_BRWBAT_STEP_OVER, DEVICE_BRWBAT_ERR_OK);                 //没有电池，结束借电池操作
                //关闭充电
                Device_StopCharge(pGateInfo, index);
                //这里预约应该关闭
                if(g_sDeviceParams.planeBat[index] == 1)
                {
                    g_sDeviceParams.planeBat[index] = 0;
                    Device_WriteDeviceParamenter();
                }
            }
        }
    }
    
    pGateInfo->rcdIdleDelay = g_nSysTick;        //借还电池不需要产生记录
    pGateInfo->rcdFlag = 0;
}



BOOL Device_CheckLockOpen(u8 index)
{
    if(index == DEVICE_GATE_INDEX0)
    {
        return (BOOL)Device_ChkLock0Open();
    }
    else
    {
        return (BOOL)Device_ChkLock1Open();
    }
}