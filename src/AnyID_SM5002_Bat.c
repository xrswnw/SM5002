#include "AnyID_SM5002_Bat.h"

//BAT_RXFRAME g_sBatRcvFrame[BAT_CH_NUM] = {0};
//BAT_TXFRAME g_sBatTxFrame[BAT_CH_NUM] = {0};
BAT_OPINFO g_sBatOpInfo[BAT_CH_NUM] = {0};
const u8 g_nBatBoardcastSn[BAT_SN_LEN] = {0};      //广播SN（全零），表示不需要


void Bat_Init(u8 index)
{
    memset(g_sBatOpInfo + index, 0, sizeof(BAT_OPINFO));
    
    Bat_InitInterface(index, BAT_CH1_BAUDRARE);
    Bat_ConfigInt(index, ENABLE);
    Bat_InitTxDma(index, g_sBatOpInfo[index].txFrame.buffer, BAT_TX_FRAME_LEN);
    Bat_InitRxDma(index, g_sBatOpInfo[index].rxFrame.buffer, BAT_RX_FRAME_LEN);
}

void Bat_Stop(u8 index)
{
    USART_TypeDef* uart = NULL;
    if(index == BAT_CH_INDEX0)
    {
        (BAT_CH1_RXDMA_CH)->CCR &= CCR_ENABLE_Reset;
        (BAT_CH1_TXDMA_CH)->CCR &= CCR_ENABLE_Reset;
        
        uart = BAT_CH1_PORT;
    }
    else if(index == BAT_CH_INDEX1)
    {
        (BAT_CH2_RXDMA_CH)->CCR &= CCR_ENABLE_Reset;
        (BAT_CH2_TXDMA_CH)->CCR &= CCR_ENABLE_Reset;
        
        uart = BAT_CH2_PORT;
    }

    Bat_ConfigInt(index, DISABLE);
    
    USART_ITConfig(uart, USART_IT_IDLE, DISABLE);
    USART_DMACmd(uart, USART_DMAReq_Tx, DISABLE);
    USART_DMACmd(uart, USART_DMAReq_Rx, DISABLE);
    USART_Cmd(uart, DISABLE);
    USART_DeInit(uart);
}

void Bat_GetOp(BAT_OPINFO *pOpInfo)
{
    Bat_ResetOpInfo(pOpInfo);
    if(pOpInfo->mode == BAT_MODE_GET_INFO)
    {
        pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_VER;
        pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_SN;
        //pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_SN;     //SN可能获取不到
        pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_STATUS;
        pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_UNITVOL;
        pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_ERR;

        pOpInfo->state = BAT_OP_STAT_TX;
    }
    else if(pOpInfo->mode == BAT_MODE_CHK_STATUS)
    {
        pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_STATUS;
        pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_UNITVOL;
        pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_ERR;
        
        pOpInfo->state = BAT_OP_STAT_TX;
    }
    else if(pOpInfo->mode == BAT_MODE_CHARGE)
    {
        pOpInfo->op[pOpInfo->num++] = BAT_CMD_CHAG_HEART;
        pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_STATUS;
        pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_UNITVOL;
        pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_ERR;
        
        pOpInfo->state = BAT_OP_STAT_TX;
    }
}


#define POLYNOMIAL                      0xA001   //x^16 + x^15 + x^2 + 1
#define PRESET_VALUE                    0x0000
u16 Bat_GetCrc(u8 *pFrame, u32 len)
{
    u16 crc = 0;
    u32 i = 0, j = 0;
    crc = PRESET_VALUE;
    for(i = 0; i < len; i++)
    {
        crc = crc ^ pFrame[i];
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

u16 Bat_FormatCmd(u8 *pTxBuf, u8 cmd, u8 *pParams, u16 len)
{
    u16 pos = 0;
    u16 crc  = 0;
    u16 frameLen = 0;
    
    pTxBuf[pos++] = BAT_FRAME_HEAD;
    pTxBuf[pos++] = 0x00;
    pTxBuf[pos++] = 0x00;
    pTxBuf[pos++] = cmd;

    memcpy(pTxBuf+ pos, pParams, len);
    pos += len;

    frameLen = pos + 2;
    pTxBuf[BAT_FRAME_POS_LEN + 0] = (frameLen >> 0) & 0xFF;
    pTxBuf[BAT_FRAME_POS_LEN + 1] = (frameLen >> 8) & 0xFF;

    crc = Bat_GetCrc(pTxBuf, pos);
    pTxBuf[pos++] = (crc >> 0) & 0xFF;
    pTxBuf[pos++] = (crc >> 8) & 0xFF;

    return pos;
}

BOOL Bat_CheckFrame(BAT_RXFRAME *pRxFrame, u16 *pStartPos, u8 cmd)//u8 cmd, BAT_INFO *pBatInfo
{
    u16 pos = 0, frameLen = 0;
    BOOL b = FALSE;
    while(pos + BAT_FRAME_MIN_LEN <= pRxFrame->len)
    {
        if(pRxFrame->buffer[pos] == BAT_FRAME_HEAD)
        {
            frameLen = *((u16 *)(pRxFrame->buffer + BAT_FRAME_POS_LEN));
            if(pos + frameLen <= pRxFrame->len)
            {
                u16 crc1 = 0, crc2 = 0;
                
                crc1 = *((u16 *)(pRxFrame->buffer + pos + frameLen - 2));
                crc2 = Bat_GetCrc(pRxFrame->buffer + pos, frameLen - 2);
                if(crc1 == crc2 && cmd == pRxFrame->buffer[pos + BAT_FRAME_POS_CMD])
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

BOOL Bat_FormatRxInfo(u8 *pFrame, BAT_INFO *pBatInfo, u8 index)
{
    BOOL b = TRUE;
    u8 cmd = 0;
    cmd = pFrame[BAT_FRAME_POS_CMD];
    switch (cmd)
    {
        case BAT_CMD_GET_VER:   
            pBatInfo->verSoft = *((u16 *)(pFrame + BAT_FRAME_POS_DAT + 0));
            pBatInfo->verHard = *((u16 *)(pFrame + BAT_FRAME_POS_DAT + 2));
            if(pBatInfo->verSoft == 0 && pBatInfo->verHard == 0)
            {
                b = FALSE;
            }
        break;
        case BAT_CMD_GET_SN:
            memcpy(pBatInfo->sn, pFrame + BAT_FRAME_POS_DAT, BAT_SN_LEN);
            if(memcmp(pBatInfo->sn, g_nBatBoardcastSn ,BAT_SN_LEN) == 0)
            {
               b = FALSE;
            }
        break;
        case BAT_CMD_GET_STATUS:
            pBatInfo->status.volValue = *((u16 *)(pFrame + BAT_FRAME_POS_DAT + 0));
            pBatInfo->status.volLev = pFrame[BAT_FRAME_POS_DAT + 2];
            pBatInfo->status.cycleCount = *((u16 *)(pFrame + BAT_FRAME_POS_DAT + 3));
            pBatInfo->status.chargeStatus = pFrame[BAT_FRAME_POS_DAT + 5];
            pBatInfo->status.maxChagVol = *((u16 *)(pFrame + BAT_FRAME_POS_DAT + 6));
            pBatInfo->status.maxChagCur = *((u16 *)(pFrame + BAT_FRAME_POS_DAT + 8));
            pBatInfo->status.remainCap = *((u16 *)(pFrame + BAT_FRAME_POS_DAT + 10));
            pBatInfo->status.curValue = *((u16 *)(pFrame + BAT_FRAME_POS_DAT + 12));
            pBatInfo->status.tmpr = pFrame[BAT_FRAME_POS_DAT + 14];
            pBatInfo->status.dltVol = pFrame[BAT_FRAME_POS_DAT + 15];
        break;
        case BAT_CMD_GET_UNITVOL:
            memcpy(pBatInfo->unitVol, pFrame + BAT_FRAME_POS_DAT, BAT_UNIT_NUM << 1);
        break;
        case BAT_CMD_GET_ERR:
            memcpy(&pBatInfo->err, pFrame + BAT_FRAME_POS_DAT, sizeof(BAT_ERRINFO));
        break;
        default:
        break;
    }
    
    return b;
}

BOOL Bat_GetNext(BAT_OPINFO *pOpInfo)
{
    u8 state = 0;
    BOOL b = TRUE;
    
    if(pOpInfo->state == BAT_OP_STAT_RX || pOpInfo->op[pOpInfo->index] == BAT_CMD_CHAG_HEART)
    {
        pOpInfo->index++;
        pOpInfo->rpt = 0;
        if(pOpInfo->index < pOpInfo->num)
        {
            pOpInfo->state = BAT_OP_STAT_DLY;
        }
        else
        {
            pOpInfo->state = BAT_OP_STAT_IDLE;
        }
    }
    else
    {
        pOpInfo->rpt++;
        if(pOpInfo->rpt < BAT_OP_RPT_TIM)
        {
            pOpInfo->state = BAT_OP_STAT_DLY;
        }
        else
        {
            pOpInfo->state = BAT_OP_STAT_IDLE;
            pOpInfo->rpt = 0;
            b = FALSE;
        }
    }
    
    return b;
}


