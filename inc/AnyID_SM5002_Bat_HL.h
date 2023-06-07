#ifndef _ANYID_SM5002_BAT_HL_
#define _ANYID_SM5002_BAT_HL_

#include "AnyID_SM5002_Config.h"

#define BAT_TX_FRAME_LEN        32
#define BAT_RX_FRAME_LEN        128

#define BAT_CH1_PORT                            USART3
#define BAT_CH1_BAUDRARE                        9600
#define Bat_Ch1IRQHandler                       USART3_IRQHandler
#define Bat_Ch1ChkTxOver()                      while(((BAT_CH1_PORT)->SR & USART_FLAG_TC) == (u16)RESET)

extern const PORT_INF BAT_CH1_PORT_CTRL;
#define Bat_Ch1EnableRx()                      (BAT_CH1_PORT_CTRL.Port->BRR  = BAT_CH1_PORT_CTRL.Pin)
#define Bat_Ch1EnableTx()                      (BAT_CH1_PORT_CTRL.Port->BSRR = BAT_CH1_PORT_CTRL.Pin)

#define Bat_Ch1ReadByte()                      ((u16)(BAT_CH1_PORT->DR & (u16)0x01FF))

                                                
#define BAT_CH1_SR_IDLE                         0x0010  
#define BAT_CH1_DMA                             DMA1

#define BAT_CH1_TXDMA_CH                        DMA1_Channel2
#define BAT_CH1_TXDMA_INT                       DMA1_Channel2_IRQn
#define Bat_Ch1TxDMAIRQHandler                  DMA1_Channel2_IRQHandler
#define BAT_CH1_TXDMA_TC_FLAG                   DMA1_FLAG_TC2
#define Bat_Ch1EnableTxDma(p, s)                do{\
                                                    Bat_Ch1EnableTx();\
                                                    (BAT_CH1_DMA)->IFCR = BAT_CH1_TXDMA_TC_FLAG; \
                                                    (BAT_CH1_TXDMA_CH)->CMAR = ((u32)(p)); \
                                                    (BAT_CH1_TXDMA_CH)->CNDTR = (s); \
                                                    (BAT_CH1_TXDMA_CH)->CCR |= CCR_ENABLE_Set; \
                                                }while(0)
                                                                                             
#define Bat_Ch1DisableTxDma()                   do{\
                                                    (BAT_CH1_DMA)->IFCR = BAT_CH1_TXDMA_TC_FLAG;\
                                                    (BAT_CH1_TXDMA_CH)->CCR &= CCR_ENABLE_Reset;\
                                                    Bat_Ch1ChkTxOver();\
                                                    Bat_Ch1EnableRx();\
                                                }while(0)


#define BAT_CH1_RXDMA_CH                        DMA1_Channel3
#define BAT_CH1_RXDMA_INT                       DMA1_Channel3_IRQn
#define Bat_Ch1RxDMAIRQHandler                  DMA1_Channel3_IRQHandler
#define BAT_CH1_RXDMA_TC_FLAG                   DMA1_FLAG_TC3
#define Bat_Ch1EnableRxDma()                    do{\
                                                    (BAT_CH1_DMA)->IFCR = BAT_CH1_RXDMA_TC_FLAG; \
                                                    (BAT_CH1_RXDMA_CH)->CNDTR = BAT_RX_FRAME_LEN; \
                                                    (BAT_CH1_RXDMA_CH)->CCR |= CCR_ENABLE_Set; \
                                                }while(0)
                                            
#define Bat_Ch1GetRxLen()                       (BAT_RX_FRAME_LEN - (BAT_CH1_RXDMA_CH)->CNDTR)  

#define Bat_Ch1DisableRxDma()                   do{\
                                                    (BAT_CH1_DMA)->IFCR = BAT_CH1_RXDMA_TC_FLAG;\
                                                    (BAT_CH1_RXDMA_CH)->CCR &= CCR_ENABLE_Reset;\
                                                }while(0)


#define BAT_CH2_PORT                            UART4
#define BAT_CH2_BAUDRARE                        9600
#define Bat_Ch2IRQHandler                       UART4_IRQHandler
#define Bat_Ch2ChkTxOver()                      while(((BAT_CH2_PORT)->SR & USART_FLAG_TC) == (u16)RESET)

extern const PORT_INF BAT_CH2_PORT_CTRL;
#define Bat_Ch2EnableRx()                      (BAT_CH2_PORT_CTRL.Port->BRR  = BAT_CH2_PORT_CTRL.Pin)
#define Bat_Ch2EnableTx()                      (BAT_CH2_PORT_CTRL.Port->BSRR = BAT_CH2_PORT_CTRL.Pin)

#define Bat_Ch2ReadByte()                      ((u16)(BAT_CH2_PORT->DR & (u16)0x01FF))

                                                
#define BAT_CH2_SR_IDLE                         0x0010  
#define BAT_CH2_DMA                             DMA2

#define BAT_CH2_TXDMA_CH                        DMA2_Channel5
#define BAT_CH2_TXDMA_INT                       DMA2_Channel4_5_IRQn
#define Bat_Ch2TxDMAIRQHandler                  DMA2_Channel4_5_IRQHandler
#define BAT_CH2_TXDMA_TC_FLAG                   DMA2_FLAG_TC5
#define Bat_Ch2EnableTxDma(p, s)                do{\
                                                    Bat_Ch2EnableTx();\
                                                    (BAT_CH2_DMA)->IFCR = BAT_CH2_TXDMA_TC_FLAG; \
                                                    (BAT_CH2_TXDMA_CH)->CMAR = ((u32)(p)); \
                                                    (BAT_CH2_TXDMA_CH)->CNDTR = (s); \
                                                    (BAT_CH2_TXDMA_CH)->CCR |= CCR_ENABLE_Set; \
                                                }while(0)
                                                                                             
#define Bat_Ch2DisableTxDma()                   do{\
                                                    (BAT_CH2_DMA)->IFCR = BAT_CH2_TXDMA_TC_FLAG;\
                                                    (BAT_CH2_TXDMA_CH)->CCR &= CCR_ENABLE_Reset;\
                                                    Bat_Ch2ChkTxOver();\
                                                    Bat_Ch2EnableRx();\
                                                }while(0)


#define BAT_CH2_RXDMA_CH                        DMA2_Channel3
#define BAT_CH2_RXDMA_INT                       DMA2_Channel3_IRQn
#define Bat_Ch2RxDMAIRQHandler                  DMA2_Channel3_IRQHandler
#define BAT_CH2_RXDMA_TC_FLAG                   DMA2_FLAG_TC3
#define Bat_Ch2EnableRxDma()                   do{\
                                                (BAT_CH2_DMA)->IFCR = BAT_CH2_RXDMA_TC_FLAG; \
                                                (BAT_CH2_RXDMA_CH)->CNDTR = BAT_RX_FRAME_LEN; \
                                                (BAT_CH2_RXDMA_CH)->CCR |= CCR_ENABLE_Set; \
                                            }while(0)
                                            
#define Bat_Ch2GetRxLen()                      (BAT_RX_FRAME_LEN - (BAT_CH2_RXDMA_CH)->CNDTR)  

#define Bat_Ch2DisableRxDma()                  do{\
                                                (BAT_CH2_DMA)->IFCR = BAT_CH2_RXDMA_TC_FLAG;\
                                                (BAT_CH2_RXDMA_CH)->CCR &= CCR_ENABLE_Reset;\
                                            }while(0)


#define BAT_CH_INDEX0                       0
#define BAT_CH_INDEX1                       1
#define BAT_CH_NUM                          2
void Bat_InitInterface(u8 index, u32 baudrate);

void Bat_ConfigInt(u8 index, FunctionalState state);

void Bat_InitTxDma(u8 index, u8 *pTxBuffer, u32 len);
void Bat_InitRxDma(u8 index, u8 *pRxBuffer, u32 len);

void Bat_Delayms(u32 n);             //œµÕ≥—” ±n∫¡√Î
void Bat_WriteBuffer(u8 index, u8 *pBuffer, u32 len);


void Bat_DisableTxDMA(u8 index);
void Bat_DisableRxDMA(u8 index);

void Bat_EnableTxDMA(u8 index, u8 *pBuffer, u32 len);
void Bat_EnableRxDMA(u8 index);
BOOL Bat_UartCheckErr(u8 index);
#endif



