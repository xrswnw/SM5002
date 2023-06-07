#include "AnyID_SM5002_Charge.h"

UART_RCVFRAME g_sChagRcvFrame = {0};
UART_TXFRAME g_sChagTxFrame = {0};

void Chag_Init(void)
{
    Chag_InitInterface(CHAG_BAUDRARE);
    Chag_ConfigInt(ENABLE);
    Chag_InitTxDma(g_sChagTxFrame.buffer, UART_BUFFER_MAX_LEN);
    Chag_InitRxDma(g_sChagRcvFrame.buffer, UART_BUFFER_MAX_LEN);
}

void Chag_Stop(void)
{
    (CHAG_RXDMA_CH)->CCR &= CCR_ENABLE_Reset;
    (CHAG_TXDMA_CH)->CCR &= CCR_ENABLE_Reset;
    Chag_ConfigInt(DISABLE);
    
    USART_ITConfig(CHAG_PORT, USART_IT_IDLE, DISABLE);
    USART_DMACmd(CHAG_PORT, USART_DMAReq_Tx, DISABLE);
    USART_DMACmd(CHAG_PORT, USART_DMAReq_Rx, DISABLE);
    USART_Cmd(CHAG_PORT, DISABLE);
    USART_DeInit(CHAG_PORT);
}


