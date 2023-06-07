#include "AnyID_SM5002_Charge.h"
#include "AnyID_SM5002_SysTick_HL.h"


CHAG_OPINFO g_sChagOpInfo[CHAG_DEV_NUM] = {0};

void Chag_Init(u8 index)
{
    memset(g_sChagOpInfo + index, 0, sizeof(CHAG_OPINFO));
    if(index == CHAG_CH_INDEX0)
    {
        Chag_InitInterface(index, CHAG_CH1_BAUDRARE);
        Chag_ConfigInt(index, ENABLE);
        Chag_InitTxDma(index, g_sChagOpInfo[index].txFrame.buffer, CHAG_TX_FRAME_LEN);
        Chag_InitRxDma(index, g_sChagOpInfo[index].rxFrame.buffer, CHAG_RX_FRAME_LEN);
    }
    else
    {
        Chag_InitInterface(index, CHAG_CH2_BAUDRARE);
        Chag_ConfigInt(index, ENABLE);
        Chag_EnableInt(index, ENABLE, DISABLE);
    }
}

void Chag_Stop(u8 index)
{
    if(index == CHAG_CH_INDEX0)
    {
        (CHAG_CH1_RXDMA_CH)->CCR &= CCR_ENABLE_Reset;
        (CHAG_CH1_TXDMA_CH)->CCR &= CCR_ENABLE_Reset;
        
        Chag_ConfigInt(index, DISABLE);
    
        USART_ITConfig(CHAG_CH1_PORT, USART_IT_IDLE, DISABLE);
        USART_DMACmd(CHAG_CH1_PORT, USART_DMAReq_Tx, DISABLE);
        USART_DMACmd(CHAG_CH1_PORT, USART_DMAReq_Rx, DISABLE);
        USART_Cmd(CHAG_CH1_PORT, DISABLE);
        USART_DeInit(CHAG_CH1_PORT);
    }
    else if(index == CHAG_CH_INDEX1)
    {
        Chag_ConfigInt(index, DISABLE);
        Chag_EnableInt(index, DISABLE, DISABLE);
        
        USART_Cmd(CHAG_CH2_PORT, DISABLE);
        USART_DeInit(CHAG_CH2_PORT);
    }
}

#define POLYNOMIAL                      0xA001   //x^16 + x^12 + x^5 + 1
#define PRESET_VALUE                    0xFFFF
u16 Chag_CalCrc(u8 *pBuffer, u8 len)
{
    u16 i = 0;
    u16 crc = 0;
    u8 j = 0;

    crc = PRESET_VALUE;
    for(i = 0; i < len; i++)
    {
        crc = crc ^ pBuffer[i];
        for(j = 0; j < 8; j++)
        {
            if(crc & 0x0001)
            {
                crc = (crc >> 1) ^ POLYNOMIAL;
            }
            else
            {
                crc = (crc >> 1);
            }
        }
    }

    return crc;
}

u16 Chag_FormatOpFrame(CHAG_OPINFO *pOpInfo, u8 *pTxBuf)
{
    u8 pos = 0;
    u16 crc = 0;
    u8 op = 0;

    op = pOpInfo->op[pOpInfo->index];
    pTxBuf[pos++] = CHAG_DEV_ADDR;
    if(op == CHAG_OP_CFG_PARAMS)
    {
        pTxBuf[pos++] = 0x0F;
        pTxBuf[pos++] = 0x00;
        pTxBuf[pos++] = 0x01;
        pTxBuf[pos++] = 0x00;
        pTxBuf[pos++] = 0x02;

        pTxBuf[pos++] = (pOpInfo->chagVol >> 8) & 0xFF;       
        pTxBuf[pos++] = (pOpInfo->chagVol >> 0) & 0xFF;

        pTxBuf[pos++] = (pOpInfo->chagCur >> 8) & 0xFF;      
        pTxBuf[pos++] = (pOpInfo->chagCur >> 0) & 0xFF;  
        
        pOpInfo->fun = 0x0F;
    }
    else if(op == CHAG_OP_GET_INFO)
    {
        pTxBuf[pos++] = 0x4D;
        pTxBuf[pos++] = 0x03;
        pTxBuf[pos++] = 0x20;
        pTxBuf[pos++] = 0x00;
        pTxBuf[pos++] = 0x08;
        
        pOpInfo->fun = 0x4D;
    }
    else if(op == CHAG_OP_OPEN_CHARGE)
    {
        pTxBuf[pos++] = 0x06;
        pTxBuf[pos++] = 0x00;
        pTxBuf[pos++] = 0x00;
        pTxBuf[pos++] = 0x00;
        pTxBuf[pos++] = 0xFF;
        
        pOpInfo->fun = 0x06;
    }
    else if(op == CHAG_OP_CLOSE_CHARGE)
    {
        pTxBuf[pos++] = 0x06;
        pTxBuf[pos++] = 0x00;
        pTxBuf[pos++] = 0x00;
        pTxBuf[pos++] = 0x00;
        pTxBuf[pos++] = 0x00;
        
        pOpInfo->fun = 0x06;
    }
    else if(op == CHAG_OP_GET_STATUS)
    {
        pTxBuf[pos++] = 0x03;
        pTxBuf[pos++] = 0x00;
        pTxBuf[pos++] = 0x01;
        pTxBuf[pos++] = 0x00;
        pTxBuf[pos++] = 0x03;
        
        pOpInfo->fun = 0x03;
    }

    crc = Chag_CalCrc(pTxBuf, pos);

    pTxBuf[pos++] = (crc >> 0) & 0xFF;
    pTxBuf[pos++] = (crc >> 8) & 0xFF;

    return pos;
}

BOOL Chag_CheckFrame(CHAG_RXFRAME *pRxFrame, u16 *pStartPos, u8 fun)
{
    u16 pos = 0, frameLen = 0;
    BOOL b = FALSE;
    while(pos + CHAG_FRAME_MIN_LEN <= pRxFrame->len)
    {
        if(pRxFrame->buffer[pos + CHAG_FRAME_POS_FUN] == fun)
        {
            if(fun == 0x0F)
            {
                frameLen = CHAG_FRAME_MIN_LEN;
            }
            else if(fun == 0x06)
            {
                frameLen = CHAG_FRAME_MIN_LEN;
            }
            else if(fun == 0x4D)
            {
                frameLen = 21;
            }
            else if(fun == 0x03)
            {
                frameLen = 11;
            }
            if(pos + frameLen <= pRxFrame->len)
            {
                u16 crc1 = 0, crc2 = 0, crc3 = 0;
                
                a_ArrayToU16(crc1, pRxFrame->buffer + pos + frameLen - 2);
                crc3 = *((u16 *)(pRxFrame->buffer + pos + frameLen - 2));
                crc2 = Chag_CalCrc(pRxFrame->buffer + pos, frameLen - 2);
                if(crc1 == crc2 || crc3 == crc2)
                {
                    *pStartPos = pos;
                    b = TRUE;
                    break;
                }
            }
        }
        pos++;
    }
    return b;
}


BOOL Chag_GetNext(CHAG_OPINFO *pOpInfo)
{
    u8 state = 0;
    BOOL b = TRUE;
    
    if(pOpInfo->state == CHAG_OP_STAT_RX)
    {
        pOpInfo->index++;
        pOpInfo->rpt = 0;
        if(pOpInfo->index < pOpInfo->num)
        {
            pOpInfo->state = CHAG_OP_STAT_DLY;
        }
        else
        {
            pOpInfo->state = CHAG_OP_STAT_IDLE;
        }
    }
    else
    {
        pOpInfo->rpt++;
        if(pOpInfo->rpt < CHAG_OP_RPT_TIM)
        {
            pOpInfo->state = CHAG_OP_STAT_DLY;
        }
        else
        {
            pOpInfo->state = CHAG_OP_STAT_IDLE;
            pOpInfo->rpt = 0;
            b = FALSE;
        }
    }
    
    return b;
}

void Chag_GetOp(CHAG_OPINFO *pOpInfo)
{
    Chag_ResetOpInfo(pOpInfo);
    if(pOpInfo->mode & CHAG_MODE_INIT)
    {
        pOpInfo->op[pOpInfo->num++] = CHAG_OP_GET_INFO;
        
        pOpInfo->chagStep = CHAG_STEP_IDX0;                                         //初始化设备，就开启充电，以激活“亏电”电池
        pOpInfo->chagCur = pOpInfo->pChagParams->stepParams[pOpInfo->chagStep].cur; 
        pOpInfo->chagVol = pOpInfo->pChagParams->stepParams[pOpInfo->chagStep].vol;
        pOpInfo->activeBatTime = g_nSysTick;
        
        pOpInfo->op[pOpInfo->num++] = CHAG_OP_CFG_PARAMS;
        pOpInfo->op[pOpInfo->num++] = CHAG_OP_OPEN_CHARGE;
        
        pOpInfo->state = CHAG_OP_STAT_TX;
    }
    else if(pOpInfo->mode & CHAG_MODE_OPEN)
    {
        pOpInfo->chagStep = CHAG_STEP_IDX0;     //所有设备都是从步骤1开始充电
        pOpInfo->chagCur = pOpInfo->pChagParams->stepParams[pOpInfo->chagStep].cur; 
        pOpInfo->chagVol = pOpInfo->pChagParams->stepParams[pOpInfo->chagStep].vol;
        pOpInfo->chagTime = g_nSysTick;
        
        pOpInfo->op[pOpInfo->num++] = CHAG_OP_CFG_PARAMS;
        pOpInfo->op[pOpInfo->num++] = CHAG_OP_OPEN_CHARGE;
        
        pOpInfo->state = CHAG_OP_STAT_TX;
    }
    else if(pOpInfo->mode & CHAG_MODE_CHARGE)
    {
        //变更充电参数
        //低电压充电(步骤1)，必须满足一定时间
        //如果发现出现高压(10%~95%)，切换到高压充电
        //如果出现更高电压>95%，继续低电压充电
        if(pOpInfo->chagStep == CHAG_STEP_IDX0 && pOpInfo->chagTime + CHAG_LOW_VOL_MIN_TIM < g_nSysTick) 
        {
            if(pOpInfo->batVolLv < CHAG_VOL_LEV_HIG)    //高电压，进入步骤2，高电压充电
            {
                pOpInfo->chagStep = CHAG_STEP_IDX1;
                pOpInfo->chagCur = pOpInfo->pChagParams->stepParams[pOpInfo->chagStep].cur; 
                pOpInfo->chagVol = pOpInfo->pChagParams->stepParams[pOpInfo->chagStep].vol; 
            }
            else                                                                                //接近满电压，进入步骤3，低电压充电
            {
                pOpInfo->chagStep = CHAG_STEP_IDX2;
                pOpInfo->chagCur = pOpInfo->pChagParams->stepParams[pOpInfo->chagStep].cur; 
                pOpInfo->chagVol = pOpInfo->pChagParams->stepParams[pOpInfo->chagStep].vol;
            }
            pOpInfo->chagTime = g_nSysTick;
            pOpInfo->op[pOpInfo->num++] = CHAG_OP_CFG_PARAMS;
        }
        else if(pOpInfo->chagStep == CHAG_STEP_IDX1 && pOpInfo->chagTime + CHAG_LOW_VOL_MIN_TIM < g_nSysTick)
        {
            if(pOpInfo->batVolLv >= CHAG_VOL_LEV_HIG)
            {
                pOpInfo->chagStep = CHAG_STEP_IDX2;
                pOpInfo->chagCur = pOpInfo->pChagParams->stepParams[pOpInfo->chagStep].cur; 
                pOpInfo->chagVol = pOpInfo->pChagParams->stepParams[pOpInfo->chagStep].vol;

                pOpInfo->chagTime = g_nSysTick;
                pOpInfo->op[pOpInfo->num++] = CHAG_OP_CFG_PARAMS;
            }
           
        }
        if(pOpInfo->chagInfo.chagCur == 0 && pOpInfo->chagInfo.chagVol == 0)
        {
            pOpInfo->op[pOpInfo->num++] = CHAG_OP_OPEN_CHARGE;      //如果充电电流和电压都是0， 重新开启充电指令
        }
        pOpInfo->op[pOpInfo->num++] = CHAG_OP_GET_STATUS;
        
        pOpInfo->state = CHAG_OP_STAT_TX;
    }
    else if(pOpInfo->mode & CHAG_MODE_CLOSE)
    {
        pOpInfo->op[pOpInfo->num++] = CHAG_OP_CLOSE_CHARGE;
        
        pOpInfo->state = CHAG_OP_STAT_TX;
    }
    else if(pOpInfo->mode & CHAG_MODE_IDLE)
    {
        pOpInfo->op[pOpInfo->num++] = CHAG_OP_GET_INFO;
        pOpInfo->op[pOpInfo->num++] = CHAG_OP_GET_STATUS;
        pOpInfo->state = CHAG_OP_STAT_TX;
    }
}
/*
void Chag_AddOpDev(CHAG_OPBUF *pOpBuf, u8 index)
{
    u8 s = 0;
    s = pOpBuf->s;
    pOpBuf->devIndex[pOpBuf->s] = index;
    pOpBuf->s++;
    if(pOpBuf->s == CHAG_OP_BUF_SIZE)
    {
        pOpBuf->s = 0;
    }
    if(pOpBuf->s == pOpBuf->e)
    {
        pOpBuf->s = s;  //如果满了，就还原，理论上不能再满的情况
    }
}

u8 Chag_GetCurDev(CHAG_OPBUF *pOpBuf)
{
    u8 index = CHAG_DEV_NUM;

    index = pOpBuf->devIndex[pOpBuf->e];

    pOpBuf->devIndex[pOpBuf->e] = CHAG_DEV_NUM;
    pOpBuf->e++;
    if(pOpBuf->e == CHAG_OP_BUF_SIZE)
    {
        pOpBuf->e = 0;
    }

    return index;
}
*/
void Chag_FormatRxInfo(u8 *pRxBuf, CHAG_OPINFO *pOpInfo)
{
    u8 op = 0;
    u8 pos = 3;
    
    op = pOpInfo->op[pOpInfo->index];
    if(op == CHAG_OP_GET_INFO)
    {
        memcpy(pOpInfo->chagInfo.vendorName, pRxBuf + pos, CHAG_VENDOR_NAME_LEN); pos += CHAG_VENDOR_NAME_LEN;
        a_ArrayToU16(pOpInfo->chagInfo.softVer, pRxBuf + pos); pos += 2;
        a_ArrayToU16(pOpInfo->chagInfo.hardVer, pRxBuf + pos); pos += 2;
        a_ArrayToU16(pOpInfo->chagInfo.type, pRxBuf + pos); pos += 2;
        a_ArrayToU16(pOpInfo->chagInfo.pwr, pRxBuf + pos); pos += 2;
        a_ArrayToU16(pOpInfo->chagInfo.maxVol, pRxBuf + pos); pos += 2;
        a_ArrayToU16(pOpInfo->chagInfo.maxCur, pRxBuf + pos); pos += 2;
    }
    else if(op == CHAG_OP_GET_STATUS)
    {
        a_ArrayToU16(pOpInfo->chagInfo.chagVol, pRxBuf + pos); pos += 2;
        pOpInfo->chagInfo.chagCur = *((u16 *)(pRxBuf + pos)); pos += 2;
        a_ArrayToU16(pOpInfo->chagInfo.errStatusInfo, pRxBuf + pos); pos += 2;
    }
}


