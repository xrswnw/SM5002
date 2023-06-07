#include "AnyID_SM5002_Charge_HL.h"


#define CHAG_CH1_INT_CHANNEL            USART1_IRQn
#define CHAG_CH2_INT_CHANNEL            UART5_IRQn

const PORT_INF CHAG_CH1_PORT_TX     = {GPIOA, GPIO_Pin_9};
const PORT_INF CHAG_CH1_PORT_RX     = {GPIOA, GPIO_Pin_10};
const PORT_INF CHAG_CH1_PORT_CTRL   = {GPIOA, GPIO_Pin_11};

const PORT_INF CHAG_CH2_PORT_TX = {GPIOC, GPIO_Pin_12};
const PORT_INF CHAG_CH2_PORT_RX = {GPIOD, GPIO_Pin_2};
const PORT_INF CHAG_CH2_PORT_CTRL = {GPIOC, GPIO_Pin_8};
void Chag_InitInterface(u8 index, u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    if(index == CHAG_CH_INDEX0)
    {
        GPIO_InitStructure.GPIO_Pin = CHAG_CH1_PORT_CTRL.Pin;
        GPIO_Init(CHAG_CH1_PORT_CTRL.Port, &GPIO_InitStructure);
        Chag_Ch1EnableRx();

        USART_DeInit(CHAG_CH1_PORT);

        GPIO_InitStructure.GPIO_Pin = CHAG_CH1_PORT_TX.Pin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(CHAG_CH1_PORT_TX.Port, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = CHAG_CH1_PORT_RX.Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;GPIO_Mode_IPU
        GPIO_Init(CHAG_CH1_PORT_RX.Port, &GPIO_InitStructure);

        USART_InitStructure.USART_BaudRate = baudrate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

        // Configure USART
        USART_Init(CHAG_CH1_PORT, &USART_InitStructure);
        // Enable the USART
        USART_Cmd(CHAG_CH1_PORT, ENABLE);
        USART_ITConfig(CHAG_CH1_PORT, USART_IT_IDLE, ENABLE);
        USART_DMACmd(CHAG_CH1_PORT, USART_DMAReq_Tx, ENABLE);
        USART_DMACmd(CHAG_CH1_PORT, USART_DMAReq_Rx, ENABLE);
    }
    else
    {
        
        GPIO_InitStructure.GPIO_Pin = CHAG_CH2_PORT_CTRL.Pin;
        GPIO_Init(CHAG_CH2_PORT_CTRL.Port, &GPIO_InitStructure);
        Chag_Ch2EnableRx();
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Pin = CHAG_CH2_PORT_TX.Pin;
        GPIO_Init(CHAG_CH2_PORT_TX.Port, &GPIO_InitStructure);
        

        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Pin = CHAG_CH2_PORT_RX.Pin;
        GPIO_Init(CHAG_CH2_PORT_RX.Port, &GPIO_InitStructure);
        
        USART_DeInit(CHAG_CH2_PORT);
        
        USART_InitStructure.USART_BaudRate = baudrate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        // Configure USART
        USART_Init(CHAG_CH2_PORT, &USART_InitStructure);
        // Enable the USART
        USART_Cmd(CHAG_CH2_PORT, ENABLE);
    }
}

void Chag_ConfigInt(u8 index, FunctionalState state)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    if(index == CHAG_CH_INDEX0)
    {
        NVIC_InitStructure.NVIC_IRQChannel = CHAG_CH1_INT_CHANNEL;
    }
    else
    {
        NVIC_InitStructure.NVIC_IRQChannel = CHAG_CH2_INT_CHANNEL;
    }
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_RX >> 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_RX & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = state;

    NVIC_Init(&NVIC_InitStructure);
}

void Chag_EnableInt(u8 ch, FunctionalState rxState, FunctionalState txState)
{
    if(ch == CHAG_CH_INDEX1)
    {
        USART_ClearITPendingBit(CHAG_CH2_PORT, USART_IT_RXNE | USART_FLAG_TC);

        USART_ITConfig(CHAG_CH2_PORT, USART_IT_RXNE, rxState);
        USART_ITConfig(CHAG_CH2_PORT, USART_IT_TC, txState);
    }
}


void Chag_InitTxDma(u8 index, u8 *pTxBuffer, u32 len)
{
    if(index == CHAG_CH_INDEX0)
    {
        NVIC_InitTypeDef  NVIC_InitStructure = {0};
        DMA_InitTypeDef DMA_InitStructure = {0};
        
        //DMA1ͨ��5����
        DMA_DeInit(CHAG_CH1_TXDMA_CH);
        //�����ַ
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(CHAG_CH1_PORT->DR));

        //�ڴ��ַ
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)pTxBuffer;
        //dma���䷽����
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        //����DMA�ڴ���ʱ�������ĳ���
        DMA_InitStructure.DMA_BufferSize = len;
        //����DMA���������ģʽ��һ������
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        //����DMA���ڴ����ģʽ
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        //���������ֳ�
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        //�ڴ������ֳ�
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        //����DMA�Ĵ���ģʽ
        DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        //����DMA�����ȼ���
        DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
        //����DMA��2��memory�еı����������
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

        DMA_Init(CHAG_CH1_TXDMA_CH, &DMA_InitStructure);

        DMA_ITConfig(CHAG_CH1_TXDMA_CH, DMA_IT_TC, ENABLE);  //open DMA send inttrupt
        Chag_Ch1DisableTxDma();// �ر�DMA
        
        //dma interrupt
        NVIC_InitStructure.NVIC_IRQChannel = CHAG_CH1_TXDMA_INT;    //

        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_DMA >> 2;    //
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_DMA & 0x03;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

        NVIC_Init(&NVIC_InitStructure);
    }
}

void Chag_InitRxDma(u8 index, u8 *pRxBuffer, u32 len)
{
    if(index == CHAG_CH_INDEX0)
    {

        NVIC_InitTypeDef  NVIC_InitStructure = {0};
        DMA_InitTypeDef DMA_InitStructure = {0};

        DMA_DeInit(CHAG_CH1_RXDMA_CH);
        //�����ַ
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(CHAG_CH1_PORT->DR));
            
        //�ڴ��ַ
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)pRxBuffer;
        //dma���䷽����
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        //����DMA�ڴ���ʱ�������ĳ���
        DMA_InitStructure.DMA_BufferSize = len;
        //����DMA���������ģʽ��һ������
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        //����DMA���ڴ����ģʽ
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        //���������ֳ�
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        //�ڴ������ֳ�
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        //����DMA�Ĵ���ģʽ
        DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        //����DMA�����ȼ���
        DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
        //����DMA��2��memory�еı����������
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        
        DMA_Init(CHAG_CH1_RXDMA_CH, &DMA_InitStructure);
        DMA_ITConfig(CHAG_CH1_RXDMA_CH, DMA_IT_TC, ENABLE);  //open DMA send inttrupt
        Chag_Ch1DisableRxDma();// �ر�DMA
        
        //dma interrupt
        NVIC_InitStructure.NVIC_IRQChannel = CHAG_CH1_RXDMA_INT;   

        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_DMA >> 2;    //
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_DMA & 0x03;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

        NVIC_Init(&NVIC_InitStructure);
    }
}

void Chag_WriteByte(u8 index, u8 ch)
{
    if(index == CHAG_CH_INDEX0)
    {
    	(CHAG_CH1_PORT)->DR = (ch & (u16)0x01FF);
        while(((CHAG_CH1_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
    }
    else
    {
        (CHAG_CH2_PORT)->DR = (ch & (u16)0x01FF);
        while(((CHAG_CH2_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
    }
}

void Chag_Delayms(u32 n)             //ϵͳ��ʱn����
{
    n *= 0x6000;
    n++;
    while(n--);
}

void Chag_WriteBuffer(u8 index, u8 *pBuffer, u32 len)
{
    u32 i = 0;
    if(index == CHAG_CH_INDEX0)
    {
        Chag_Ch1EnableTx();
        Chag_Delayms(1);
        for(i = 0; i < len; i++)
        {
            Chag_WriteByte(index, pBuffer[i]);
        }
        Chag_Delayms(1);
        Chag_Ch1EnableRx();
    }
    else
    {
        Chag_Ch2EnableTx();
        for(i = 0; i < len; i++)
        {
            Chag_WriteByte(index, pBuffer[i]);
        }
        Chag_Delayms(1);
        Chag_Ch2ChkTxOver();
        Chag_Ch2EnableRx();
    }
}

void Chag_EnableTxDMA(u8 index, u8 *pBuffer, u32 len)
{
    if(index == CHAG_CH_INDEX0)
    {
        Chag_Ch1EnableTxDma(pBuffer, len);
    }
    else if(index == CHAG_CH_INDEX1)
    {
        Chag_WriteBuffer(index, pBuffer, len);
    }
}

void Chag_EnableRxDMA(u8 index)
{
    if(index == CHAG_CH_INDEX0)
    {
        Chag_Ch1EnableRxDma();
    }
    else if(index == CHAG_CH_INDEX1)
    {
        //Chag_Ch2EnableRxDma();
        Chag_Ch2EnableRx();
        //USART_ClearITPendingBit(CHAG_CH2_PORT, USART_IT_RXNE);
        //USART_ITConfig(CHAG_CH2_PORT, USART_IT_RXNE, ENABLE);
    }
}

void Chag_DisableTxDMA(u8 index)
{
    if(index == CHAG_CH_INDEX0)
    {
        Chag_Ch1DisableTxDma();
    }
    else if(index == CHAG_CH_INDEX1)
    {
        //Chag_Ch2DisableTxDma();
    }
}

void Chag_DisableRxDMA(u8 index)
{
    if(index == CHAG_CH_INDEX0)
    {
        Chag_Ch1DisableRxDma();
    }
    else if(index == CHAG_CH_INDEX1)
    {
        //Chag_Ch2DisableRxDma();
        //USART_ITConfig(CHAG_CH2_PORT, USART_IT_TC, DISABLE);
    }
}

BOOL Chag_UartCheckErr(u8 index)
{
    BOOL b = FALSE;
    if(index == CHAG_CH_INDEX0)
    {
        if(USART_GetFlagStatus(CHAG_CH1_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
        {
            USART_ClearFlag(CHAG_CH1_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
            b = TRUE;
        }
    }
    else if(index == CHAG_CH_INDEX1)
    {
        if(USART_GetFlagStatus(CHAG_CH2_PORT, USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
        {
            USART_ClearFlag(CHAG_CH2_PORT, USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
            b = TRUE;
        }
    }
    
    return b;
}


