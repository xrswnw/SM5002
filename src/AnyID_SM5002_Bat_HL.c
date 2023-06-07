#include "AnyID_SM5002_Bat_HL.h"


#define BAT_CH1_INT_CHANNEL         USART3_IRQn
#define BAT_CH2_INT_CHANNEL         UART4_IRQn

const PORT_INF BAT_CH1_PORT_TX     = {GPIOB, GPIO_Pin_10};
const PORT_INF BAT_CH1_PORT_RX     = {GPIOB, GPIO_Pin_11};
const PORT_INF BAT_CH1_PORT_CTRL   = {GPIOB, GPIO_Pin_12};

const PORT_INF BAT_CH2_PORT_TX     = {GPIOC, GPIO_Pin_10};
const PORT_INF BAT_CH2_PORT_RX     = {GPIOC, GPIO_Pin_11};
const PORT_INF BAT_CH2_PORT_CTRL   = {GPIOC, GPIO_Pin_9};
void Bat_InitInterface(u8 index, u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    if(index == BAT_CH_INDEX0)
    {
        GPIO_InitStructure.GPIO_Pin = BAT_CH1_PORT_CTRL.Pin;
        GPIO_Init(BAT_CH1_PORT_CTRL.Port, &GPIO_InitStructure);
        Bat_Ch1EnableRx();

        USART_DeInit(BAT_CH1_PORT);

        GPIO_InitStructure.GPIO_Pin = BAT_CH1_PORT_TX.Pin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(BAT_CH1_PORT_TX.Port, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = BAT_CH1_PORT_RX.Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;
        GPIO_Init(BAT_CH1_PORT_RX.Port, &GPIO_InitStructure);

        USART_InitStructure.USART_BaudRate = baudrate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

        // Configure USART
        USART_Init(BAT_CH1_PORT, &USART_InitStructure);
        // Enable the USART
        USART_Cmd(BAT_CH1_PORT, ENABLE);
        USART_ITConfig(BAT_CH1_PORT, USART_IT_IDLE, ENABLE);
        USART_DMACmd(BAT_CH1_PORT, USART_DMAReq_Tx, ENABLE);
        USART_DMACmd(BAT_CH1_PORT, USART_DMAReq_Rx, ENABLE);
    }
    else if(index == BAT_CH_INDEX1)
    {
        GPIO_InitStructure.GPIO_Pin = BAT_CH2_PORT_CTRL.Pin;
        GPIO_Init(BAT_CH2_PORT_CTRL.Port, &GPIO_InitStructure);
        Bat_Ch2EnableRx();

        USART_DeInit(BAT_CH2_PORT);

        GPIO_InitStructure.GPIO_Pin = BAT_CH2_PORT_TX.Pin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(BAT_CH2_PORT_TX.Port, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = BAT_CH2_PORT_RX.Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;
        GPIO_Init(BAT_CH2_PORT_RX.Port, &GPIO_InitStructure);

        USART_InitStructure.USART_BaudRate = baudrate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

        // Configure USART
        USART_Init(BAT_CH2_PORT, &USART_InitStructure);
        // Enable the USART
        USART_Cmd(BAT_CH2_PORT, ENABLE);
        USART_ITConfig(BAT_CH2_PORT, USART_IT_IDLE, ENABLE);
        USART_DMACmd(BAT_CH2_PORT, USART_DMAReq_Tx, ENABLE);
        USART_DMACmd(BAT_CH2_PORT, USART_DMAReq_Rx, ENABLE);
    }
}

void Bat_ConfigInt(u8 index, FunctionalState state)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    if(index == BAT_CH_INDEX0)
    {
        NVIC_InitStructure.NVIC_IRQChannel = BAT_CH1_INT_CHANNEL;
    }
    else if(index == BAT_CH_INDEX1)
    {
        NVIC_InitStructure.NVIC_IRQChannel = BAT_CH2_INT_CHANNEL;
    }
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_RX >> 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_RX & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = state;

    NVIC_Init(&NVIC_InitStructure);
}

void Bat_InitTxDma(u8 index, u8 *pTxBuffer, u32 len)
{
    NVIC_InitTypeDef  NVIC_InitStructure = {0};
    DMA_InitTypeDef DMA_InitStructure = {0};
    if(index == BAT_CH_INDEX0)
    {
        //DMA1通道5配置
        DMA_DeInit(BAT_CH1_TXDMA_CH);
        //外设地址
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(BAT_CH1_PORT->DR));
    }
    else if(index == BAT_CH_INDEX1)
    {
        //DMA1通道5配置
        DMA_DeInit(BAT_CH2_TXDMA_CH);
        //外设地址
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(BAT_CH2_PORT->DR));
    }
    
    //内存地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)pTxBuffer;
    //dma传输方向单向
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    //设置DMA在传输时缓冲区的长度
    DMA_InitStructure.DMA_BufferSize = len;
    //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //设置DMA的内存递增模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //外设数据字长
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    //内存数据字长
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    //设置DMA的传输模式
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    //设置DMA的优先级别
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    //设置DMA的2个memory中的变量互相访问
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    if(index == BAT_CH_INDEX0)
    {
        DMA_Init(BAT_CH1_TXDMA_CH, &DMA_InitStructure);

        DMA_ITConfig(BAT_CH1_TXDMA_CH, DMA_IT_TC, ENABLE);  //open DMA send inttrupt
        Bat_Ch1DisableTxDma();// 关闭DMA
        
        //dma interrupt
        NVIC_InitStructure.NVIC_IRQChannel = BAT_CH1_TXDMA_INT;                                       //
    }
    else if(index == BAT_CH_INDEX1)
    {
        DMA_Init(BAT_CH2_TXDMA_CH, &DMA_InitStructure);

        DMA_ITConfig(BAT_CH2_TXDMA_CH, DMA_IT_TC, ENABLE);  //open DMA send inttrupt
        Bat_Ch2DisableTxDma();// 关闭DMA
        
        //dma interrupt
        NVIC_InitStructure.NVIC_IRQChannel = BAT_CH2_TXDMA_INT;                                       //
    }
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_DMA >> 2;    //
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_DMA & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void Bat_InitRxDma(u8 index, u8 *pRxBuffer, u32 len)
{
    NVIC_InitTypeDef  NVIC_InitStructure = {0};
    DMA_InitTypeDef DMA_InitStructure = {0};

    if(index == BAT_CH_INDEX0)
    {
        DMA_DeInit(BAT_CH1_RXDMA_CH);
        //外设地址
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(BAT_CH1_PORT->DR));
    }
    else if(index == BAT_CH_INDEX1)
    {
        DMA_DeInit(BAT_CH2_RXDMA_CH);
        //外设地址
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(BAT_CH2_PORT->DR));
    }
    
    //内存地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)pRxBuffer;
    //dma传输方向单向
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    //设置DMA在传输时缓冲区的长度
    DMA_InitStructure.DMA_BufferSize = len;
    //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //设置DMA的内存递增模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //外设数据字长
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    //内存数据字长
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    //设置DMA的传输模式
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    //设置DMA的优先级别
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    //设置DMA的2个memory中的变量互相访问
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    
    if(index == BAT_CH_INDEX0)
    {
        DMA_Init(BAT_CH1_RXDMA_CH, &DMA_InitStructure);
        DMA_ITConfig(BAT_CH1_RXDMA_CH, DMA_IT_TC, ENABLE);  //open DMA send inttrupt
        Bat_Ch1DisableRxDma();// 关闭DMA
        
        //dma interrupt
        NVIC_InitStructure.NVIC_IRQChannel = BAT_CH1_RXDMA_INT; 
    }
    else if(index == BAT_CH_INDEX1)
    {
        DMA_Init(BAT_CH2_RXDMA_CH, &DMA_InitStructure);
        DMA_ITConfig(BAT_CH2_RXDMA_CH, DMA_IT_TC, ENABLE);  //open DMA send inttrupt
        Bat_Ch2DisableRxDma();// 关闭DMA
        
        //dma interrupt
        NVIC_InitStructure.NVIC_IRQChannel = BAT_CH2_RXDMA_INT; 
    }

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_DMA >> 2;    //
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_DMA & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void Bat_Delayms(u32 n)             //系统延时n毫秒
{
    n *= 0x6000;
    n++;
    while(n--);
}

void Bat_WriteBuffer(u8 index, u8 *pBuffer, u32 len)
{
    u32 i = 0;
    if(index == BAT_CH_INDEX0)
    {
        Bat_Ch1EnableTx();
        Bat_Delayms(1);
        for(i = 0; i < len; i++)
        {
            (BAT_CH1_PORT)->DR = pBuffer[i];
            while(((BAT_CH1_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
        }
        Bat_Delayms(1);
        Bat_Ch1EnableRx();
    }
    else if(index == BAT_CH_INDEX1)
    {
        Bat_Ch2EnableTx();
        Bat_Delayms(1);
        for(i = 0; i < len; i++)
        {
            (BAT_CH2_PORT)->DR = pBuffer[i];
            while(((BAT_CH2_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
        }
        Bat_Delayms(1);
        Bat_Ch2EnableRx();
    }
}

void Bat_EnableTxDMA(u8 index, u8 *pBuffer, u32 len)
{
    if(index == BAT_CH_INDEX0)
    {
        Bat_Ch1EnableTxDma(pBuffer, len);
    }
    else if(index == BAT_CH_INDEX1)
    {
        Bat_Ch2EnableTxDma(pBuffer, len);
    }
}

void Bat_EnableRxDMA(u8 index)
{
    if(index == BAT_CH_INDEX0)
    {
        Bat_Ch1EnableRxDma();
    }
    else if(index == BAT_CH_INDEX1)
    {
        Bat_Ch2EnableRxDma();
    }
}

void Bat_DisableTxDMA(u8 index)
{
    if(index == BAT_CH_INDEX0)
    {
        Bat_Ch1DisableTxDma();
    }
    else if(index == BAT_CH_INDEX1)
    {
        Bat_Ch2DisableTxDma();
    }
}

void Bat_DisableRxDMA(u8 index)
{
    if(index == BAT_CH_INDEX0)
    {
        Bat_Ch1DisableRxDma();
    }
    else if(index == BAT_CH_INDEX1)
    {
        Bat_Ch2DisableRxDma();
    }
}


BOOL Bat_UartCheckErr(u8 index)
{
    BOOL b = FALSE;
    if(index == BAT_CH_INDEX0)
    {
        if(USART_GetFlagStatus(BAT_CH1_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
        {
            USART_ClearFlag(BAT_CH1_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
            b = TRUE;
        }
    }
    else if(index == BAT_CH_INDEX1)
    {
        if(USART_GetFlagStatus(BAT_CH2_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
        {
            USART_ClearFlag(BAT_CH2_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
            b = TRUE;
        }
    }
    
    return b;
}

