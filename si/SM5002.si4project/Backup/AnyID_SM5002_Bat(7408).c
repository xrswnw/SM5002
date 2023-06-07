#include "AnyID_SM5002_Bat.h"

UART_RCVFRAME g_sBatRcvFrame = {0};
UART_TXFRAME g_sBatTxFrame = {0};

void Bat_Init(void)
{
    Bat_InitInterface(BAT_BAUDRARE);
    Bat_ConfigInt(ENABLE);
    Bat_InitTxDma(g_sBatTxFrame.buffer, UART_BUFFER_MAX_LEN);
    Bat_InitRxDma(g_sBatRcvFrame.buffer, UART_BUFFER_MAX_LEN);
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


