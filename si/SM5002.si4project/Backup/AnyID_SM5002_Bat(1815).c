#include "AnyID_SM5002_Bat.h"

BAT_RXFRAME g_sBatRcvFrame = {0};
BAT_TXFRAME g_sBatTxFrame = {0};
BAT_OPINFO g_sBatOpInfo = {0};


void Bat_Init(void)
{
    Bat_InitInterface(BAT_BAUDRARE);
    Bat_ConfigInt(ENABLE);
    Bat_InitTxDma(g_sBatTxFrame.buffer, BAT_TX_FRAME_LEN);
    Bat_InitRxDma(g_sBatRcvFrame.buffer, BAT_RX_FRAME_LEN);
}

void Bat_Stop(void)
{
    (BAT_RXDMA_CH)->CCR &= CCR_ENABLE_Reset;
    (BAT_TXDMA_CH)->CCR &= CCR_ENABLE_Reset;
    Bat_ConfigInt(DISABLE);
    
    USART_ITConfig(BAT_PORT, USART_IT_IDLE, DISABLE);
    USART_DMACmd(BAT_PORT, USART_DMAReq_Tx, DISABLE);
    USART_DMACmd(BAT_PORT, USART_DMAReq_Rx, DISABLE);
    USART_Cmd(BAT_PORT, DISABLE);
    USART_DeInit(BAT_PORT);
}

void Bat_GetOp(BAT_OPINFO *pOpInfo)
{
    Bat_ResetOpInfo(pOpInfo);
    if(pOpInfo->mode == BAT_MODE_GET_INFO)
    {
        pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_VER;
        pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_SN;
        pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_STATUS;
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
    pTxBuf[BAT_FRAME_POS_LED + 0] = (frameLen >> 0) & 0xFF;
    pTxBuf[BAT_FRAME_POS_LED + 1] = (frameLen >> 8) & 0xFF;

    crc = Bat_GetCrc(pTxBuf, pos);
    pTxBuf[pos++] = (crc >> 0) & 0xFF;
    pTxBuf[pos++] = (crc >> 8) & 0xFF;

    return pos;
}


void Bat_TxCmd(BAT_TXFRAME *pTxFrame, u8 cmd)
{
    pTxFrame->len = Bat_FormatCmd(pTxFrame->buffer, cmd, NULL, 0);
    Bat_EnableTxDma(pTxFrame->buffer, pTxFrame->len);
}

BOOL Bat_CheckFrame(BAT_RXFRAME *pRxFrame, u16 *pStartPos)//u8 cmd, BAT_INFO *pBatInfo
{
    u16 pos = 0, frameLen = 0;
    BOOL b = FALSE;
    while(pos + BAT_FRAME_MIN_LEN <= pRxFrame->len)
    {
        if(pRxFrame->buffer[pos] == BAT_FRAME_HEAD)
        {
            frameLen = *((u16 *)(pRxFrame->buffer + BAT_FRAME_POS_LED));
            if(pos + frameLen <= pRxFrame->len)
            {
                u16 crc1 = 0, crc2 = 0;
                
                crc1 = *((u16 *)(pRxFrame->buffer + pos + frameLen - 2));
                crc2 = Bat_GetCrc(pRxFrame->buffer + pos, frameLen - 2);
                if(crc1 == crc2)
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

u8 Bat_GetNext(BAT_OPINFO *pOpInfo)
{
    u8 state = 0;
    
    if(pOpInfo->state == BAT_OP_STAT_RX)
    {
        pOpInfo->index++;
        pOpInfo->rpt = 0;
        if(pOpInfo->index < pOpInfo->num)
        {
            state = BAT_OP_STAT_TX;
        }
        else
        {
            state = BAT_OP_STAT_IDLE;
        }
    }
    else
    {
        pOpInfo->rpt++;
        if(pOpInfo->rpt < BAT_OP_RPT_TIM)
        {
            state = BAT_OP_STAT_TX;
        }
        else
        {
            state = BAT_OP_STAT_IDLE;
        }
    }
    
    return state;
}


