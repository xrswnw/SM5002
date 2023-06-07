#ifndef _ANYID_SM5002_CHAG_HL_
#define _ANYID_SM5002_CHAG_HL_

#include "AnyID_SM5002_Config.h"

#define CHAG_TX_FRAME_LEN                       32
#define CHAG_RX_FRAME_LEN                       128

#define CHAG_CH1_PORT                           USART1
#define CHAG_CH1_BAUDRARE                       9600
#define Chag_Ch1IRQHandler                      USART1_IRQHandler
#define Chag_Ch1ChkTxOver()                     while(((CHAG_CH1_PORT)->SR & USART_FLAG_TC) == (u16)RESET)


extern const PORT_INF CHAG_CH1_PORT_CTRL;
#define Chag_Ch1EnableRx()                      (CHAG_CH1_PORT_CTRL.Port->BRR  = CHAG_CH1_PORT_CTRL.Pin)
#define Chag_Ch1EnableTx()                      (CHAG_CH1_PORT_CTRL.Port->BSRR = CHAG_CH1_PORT_CTRL.Pin)

#define Chag_Ch1ReadByte()                      ((u16)(CHAG_CH1_PORT->DR & (u16)0x01FF))

                                                
#define CHAG_CH1_SR_IDLE                        0x0010  
#define CHAG_CH1_DMA                            DMA1

#define CHAG_CH1_TXDMA_CH                       DMA1_Channel4
#define CHAG_CH1_TXDMA_INT                      DMA1_Channel4_IRQn
#define Chag_Ch1TxDMAIRQHandler                 DMA1_Channel4_IRQHandler
#define CHAG_CH1_TXDMA_TC_FLAG                  DMA1_FLAG_TC4
#define Chag_Ch1EnableTxDma(p, s)               do{\
                                                    Chag_Ch1EnableTx();\
                                                    (CHAG_CH1_DMA)->IFCR = CHAG_CH1_TXDMA_TC_FLAG; \
                                                    (CHAG_CH1_TXDMA_CH)->CMAR = ((u32)(p)); \
                                                    (CHAG_CH1_TXDMA_CH)->CNDTR = (s); \
                                                    (CHAG_CH1_TXDMA_CH)->CCR |= CCR_ENABLE_Set; \
                                                }while(0)
                                                                                             
#define Chag_Ch1DisableTxDma()                  do{\
                                                    (CHAG_CH1_DMA)->IFCR = CHAG_CH1_TXDMA_TC_FLAG;\
                                                    (CHAG_CH1_TXDMA_CH)->CCR &= CCR_ENABLE_Reset;\
                                                    Chag_Ch1ChkTxOver();\
                                                    Chag_Ch1EnableRx();\
                                                }while(0)


#define CHAG_CH1_RXDMA_CH                       DMA1_Channel5
#define CHAG_CH1_RXDMA_INT                      DMA1_Channel5_IRQn
#define Chag_Ch1RxDMAIRQHandler                 DMA1_Channel5_IRQHandler
#define CHAG_CH1_RXDMA_TC_FLAG                  DMA1_FLAG_TC5
#define Chag_Ch1EnableRxDma()                   do{\
                                                    (CHAG_CH1_DMA)->IFCR = CHAG_CH1_RXDMA_TC_FLAG; \
                                                    (CHAG_CH1_RXDMA_CH)->CNDTR = CHAG_RX_FRAME_LEN; \
                                                    (CHAG_CH1_RXDMA_CH)->CCR |= CCR_ENABLE_Set; \
                                                }while(0)
                                            
#define Chag_Ch1GetRxLen()                      (CHAG_RX_FRAME_LEN - (CHAG_CH1_RXDMA_CH)->CNDTR)  

#define Chag_Ch1DisableRxDma()                  do{\
                                                    (CHAG_CH1_DMA)->IFCR = CHAG_CH1_RXDMA_TC_FLAG;\
                                                    (CHAG_CH1_RXDMA_CH)->CCR &= CCR_ENABLE_Reset;\
                                                }while(0)

#define CHAG_CH2_PORT                           UART5
#define CHAG_CH2_BAUDRARE                       9600
#define Chag_Ch2IRQHandler                      UART5_IRQHandler
#define Chag_Ch2ChkTxOver()                     while(((CHAG_CH2_PORT)->SR & USART_FLAG_TC) == (u16)RESET)


extern const PORT_INF CHAG_CH2_PORT_CTRL;
#define Chag_Ch2EnableRx()                      (CHAG_CH2_PORT_CTRL.Port->BRR  = CHAG_CH2_PORT_CTRL.Pin)
#define Chag_Ch2EnableTx()                      (CHAG_CH2_PORT_CTRL.Port->BSRR = CHAG_CH2_PORT_CTRL.Pin)

#define Chag_Ch2ReadByte()                      ((u16)(CHAG_CH2_PORT->DR & (u16)0x01FF))

                                            
#define CHAG_CH_INDEX0                          0
#define CHAG_CH_INDEX1                          1
#define CHAG_CH_NUM                             2
void Chag_InitInterface(u8 index, u32 baudrate);

void Chag_ConfigInt(u8 index, FunctionalState state);
void Chag_EnableInt(u8 ch, FunctionalState rxState, FunctionalState txState);

void Chag_InitTxDma(u8 index, u8 *pTxBuffer, u32 len);
void Chag_InitRxDma(u8 index, u8 *pRxBuffer, u32 len);

void Chag_Delayms(u32 n);             //œµÕ≥—” ±n∫¡√Î
void Chag_WriteByte(u8 index, u8 ch);
void Chag_WriteBuffer(u8 index, u8 *pBuffer, u32 len);

void Chag_EnableTxDMA(u8 index, u8 *pBuffer, u32 len);
void Chag_EnableRxDMA(u8 index);
void Chag_DisableTxDMA(u8 index);
void Chag_DisableRxDMA(u8 index);
BOOL Chag_UartCheckErr(u8 index);

#endif



