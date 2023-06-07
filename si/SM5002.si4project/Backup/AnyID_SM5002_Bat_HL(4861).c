#include "AnyID_SM5002_Charge_HL.h"


#define CHAG_INT_CHANNEL           USART3_IRQn
const PORT_INF CHAG_PORT_TX     = {GPIOB, GPIO_Pin_10};
const PORT_INF CHAG_PORT_RX     = {GPIOB, GPIO_Pin_11};
const PORT_INF CHAG_PORT_CTRL   = {GPIOA, GPIO_Pin_6};
void Chag_InitInterface(u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = CHAG_PORT_CTRL.Pin;
    GPIO_Init(CHAG_PORT_CTRL.Port, &GPIO_InitStructure);
    Chag_EnableRx();

    USART_DeInit(CHAG_PORT);

    GPIO_InitStructure.GPIO_Pin = CHAG_PORT_TX.Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(CHAG_PORT_TX.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = CHAG_PORT_RX.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(CHAG_PORT_RX.Port, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    // Configure USART
    USART_Init(CHAG_PORT, &USART_InitStructure);
    // Enable the USART
    USART_Cmd(CHAG_PORT, ENABLE);
    USART_ITConfig(CHAG_PORT, USART_IT_IDLE, ENABLE);
    USART_DMACmd(CHAG_PORT, USART_DMAReq_Tx, ENABLE);
    USART_DMACmd(CHAG_PORT, USART_DMAReq_Rx, ENABLE);
}

void Chag_ConfigInt(FunctionalState state)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    NVIC_InitStructure.NVIC_IRQChannel = CHAG_INT_CHANNEL;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_RX >> 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_RX & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = state;

    NVIC_Init(&NVIC_InitStructure);
}

void Chag_InitTxDma(u8 *pTxBuffer, u32 len)
{
    NVIC_InitTypeDef  NVIC_InitStructure = {0};
    DMA_InitTypeDef DMA_InitStructure = {0};

    //DMA1ͨ��5����
    DMA_DeInit(CHAG_TXDMA_CH);
    //�����ַ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(CHAG_PORT->DR));
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
    DMA_Init(CHAG_TXDMA_CH, &DMA_InitStructure);

    DMA_ITConfig(CHAG_TXDMA_CH, DMA_IT_TC, ENABLE);  //open DMA send inttrupt
    Chag_DisableTxDma();// �ر�DMA
    
    //dma interrupt
    NVIC_InitStructure.NVIC_IRQChannel = CHAG_TXDMA_INT;                                       //
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_DMA >> 2;    //
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_DMA & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void Chag_InitRxDma(u8 *pRxBuffer, u32 len)
{
    NVIC_InitTypeDef  NVIC_InitStructure = {0};
    DMA_InitTypeDef DMA_InitStructure = {0};


    DMA_DeInit(CHAG_RXDMA_CH);
    //�����ַ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(CHAG_PORT->DR));
    
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
    
    
    DMA_Init(CHAG_RXDMA_CH, &DMA_InitStructure);
    DMA_ITConfig(CHAG_RXDMA_CH, DMA_IT_TC, ENABLE);  //open DMA send inttrupt
    Chag_DisableRxDma();// �ر�DMA
    
    //dma interrupt
    NVIC_InitStructure.NVIC_IRQChannel = CHAG_RXDMA_INT;   


    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_DMA >> 2;    //
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_DMA & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void Chag_WriteByte(u8 ch)
{
	(CHAG_PORT)->DR = (ch & (u16)0x01FF);
    while(((CHAG_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
}

void Chag_Delayms(u32 n)             //ϵͳ��ʱn����
{
    n *= 0x3800;
    n++;
    while(n--);
}

void Chag_WriteBuffer(u8 *pBuffer, u32 len)
{
    u32 i = 0;
    Chag_EnableTx();
    Chag_Delayms(1);
    for(i = 0; i < len; i++)
    {
        CHAG_WriteByte(pBuffer[i]);
    }
    Chag_Delayms(1);
    Chag_EnableRx();
}
