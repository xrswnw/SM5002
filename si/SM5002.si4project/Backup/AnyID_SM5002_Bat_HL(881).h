#ifndef _ANYID_SM5002_CHAG_HL_
#define _ANYID_SM5002_CHAG_HL_

#include "AnyID_SM5002_Config.h"

#define CHAG_PORT                           USART1
#define CHAG_BAUDRARE                       115200
#define Chag_IRQHandler                     USART1_IRQHandler
#define Chag_ChkTxOver()                    while(((CHAG_PORT)->SR & USART_FLAG_TC) == (u16)RESET)


extern const PORT_INF CHAG_PORT_CTRL;
#define Chag_EnableRx()                     (CHAG_PORT_CTRL.Port->BRR  = CHAG_PORT_CTRL.Pin)
#define Chag_EnableTx()                     (CHAG_PORT_CTRL.Port->BSRR = CHAG_PORT_CTRL.Pin)

#define Chag_ReadByte()                     ((u16)(CHAG_PORT->DR & (u16)0x01FF))

                                                
#define CHAG_SR_IDLE                        0x0010  
#define CHAG_DMA                            DMA1

#define CHAG_TXDMA_CH                       DMA1_Channel4
#define CHAG_TXDMA_INT                      DMA1_Channel4_IRQn
#define Chag_TxDMAIRQHandler                DMA1_Channel4_IRQHandler
#define CHAG_TXDMA_TC_FLAG                  DMA1_FLAG_TC4
#define Chag_EnableTxDma(p, s)              do{\
                                                Chag_EnableTx();\
                                                (CHAG_DMA)->IFCR = CHAG_TXDMA_TC_FLAG; \
                                                (CHAG_TXDMA_CH)->CMAR = ((u32)(p)); \
                                                (CHAG_TXDMA_CH)->CNDTR = (s); \
                                                (CHAG_TXDMA_CH)->CCR |= CCR_ENABLE_Set; \
                                             }while(0)
                                                                                             
#define Chag_DisableTxDma()                 do{\
                                                (CHAG_DMA)->IFCR = CHAG_TXDMA_TC_FLAG;\
                                                (CHAG_TXDMA_CH)->CCR &= CCR_ENABLE_Reset;\
                                                Chag_ChkTxOver();\
                                                Chag_EnableRx();\
                                             }while(0)


#define CHAG_RXDMA_CH                       DMA1_Channel5
#define CHAG_RXDMA_INT                      DMA1_Channel5_IRQn
#define Chag_RxDMAIRQHandler                DMA1_Channel5_IRQHandler
#define CHAG_RXDMA_TC_FLAG                  DMA1_FLAG_TC5
#define Chag_EnableRxDma()                  do{\
                                                (CHAG_DMA)->IFCR = CHAG_RXDMA_TC_FLAG; \
                                                (CHAG_RXDMA_CH)->CNDTR = UART_BUFFER_MAX_LEN; \
                                                (CHAG_RXDMA_CH)->CCR |= CCR_ENABLE_Set; \
                                            }while(0)
                                            
#define Chag_GetRxLen()                     (UART_BUFFER_MAX_LEN - (CHAG_RXDMA_CH)->CNDTR)  

#define Chag_DisableRxDma()                 do{\
                                                (CHAG_DMA)->IFCR = CHAG_RXDMA_TC_FLAG;\
                                                (CHAG_RXDMA_CH)->CCR &= CCR_ENABLE_Reset;\
                                            }while(0)
                                            

void Chag_InitInterface(u32 baudrate);

void Chag_ConfigInt(FunctionalState state);

void Chag_InitTxDma(u8 *pTxBuffer, u32 len);
void Chag_InitRxDma(u8 *pRxBuffer, u32 len);

void Chag_Delayms(u32 n);             //œµÕ≥—” ±n∫¡√Î
void Chag_WriteByte(u8 ch);
void Chag_WriteBuffer(u8 *pBuffer, u32 len);

#endif



