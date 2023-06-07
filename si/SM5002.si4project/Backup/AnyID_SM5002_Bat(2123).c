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

void Bat_GetInfo(BAT_OPINFO *pOpInfo, BAT_INFO *pBatInfo)
{
    memset(pOpInfo, 0, sizeof(BAT_OPINFO));
    memset(pBatInfo, 0, sizeof(BAT_INFO));

    pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_VER;
    pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_SN;
    pOpInfo->op[pOpInfo->num++] = BAT_CMD_GET_STATUS;
}


#define BAT_FRAME_HEAD              0x7E
#define BAT_FRAME_MIN_LEN           6
#define BAT_FRAME_POS_HEAD          0          
#define BAT_FRAME_POS_LED           1
#define BAT_FRAME_POS_CMD           3
#define BAT_FRAME_POS_DAT           4
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

    crc = a_GetCrc(pTxBuf, pos);
    pTxBuf[pos++] = (crc >> 0) & 0xFF;
    pTxBuf[pos++] = (crc >> 8) & 0xFF;

    return pos;
}


void Bat_TxCmd(BAT_TXFRAME *pTxFrame, u8 cmd)
{
    pTxFrame->len = Bat_FormatCmd(pTxFrame->buffer, cmd, NULL, 0);
    Bat_EnableTxDma(pTxFrame->buffer, pTxFrame->len);
}

BOOL Bat_CheckFrame(BAT_RXFRAME *pRxFrame, u16*pStartPos, u8 cmd, BAT_INFO *pBatInfo)
{
    u16 pos = 0;
    BOOL b = FALSE;
    while(pos + BAT_FRAME_MIN_LEN <= pRxFrame->len)
    {
        
    }
    return b;
}


