/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    04/16/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "AnyID_SM5002_SysCfg.h"
#include "AnyID_SM5002_Device.h"


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    g_nSysTick++;

    a_SetStateBit(g_nSysState, SYS_STAT_RUNLED);
    if((g_nSysTick & 0x01) == 0)
    {
        a_SetStateBit(g_nSysState, SYS_STAT_AD);
    }
    else
    {
        a_SetStateBit(g_nSysState, SYS_STAT_IN_SENSOR);
    }

    if(g_sChagOpInfo[CHAG_CH_INDEX1].rxFrame.state == UART_STAT_RX_ING)
    {
        if(g_sChagOpInfo[CHAG_CH_INDEX1].rxFrame.rxTick + UART_RX_TICK_TO < g_nSysTick)
        {
            g_sChagOpInfo[CHAG_CH_INDEX1].rxFrame.state = UART_STAT_RX_END;
        }
    }
}

void R485_RxDMAIRQHandler(void)
{
    R485_DisableRxDma(); //接收缓冲区满，一般不会出现这样的情况，如果有，就可能系统有故障
    g_sR485RcvFrame.len = R485_GetRxLen(); 
    g_sR485RcvFrame.state = UART_STAT_RX_OVR;    //接收数据完成
}

void R485_TxDMAIRQHandler(void)
{
    R485_DisableTxDma();                                             //DMA完成后，最后一个字节可能没有发送出去，需要在主循环中做判断处理
    R485_EnableRxDma();                                              //使能接收
    g_sR485TxFrame.state = UART_STAT_TX_IDLE;
}

u16 g_nR485Sr = 0;
u16 g_nR485Dr = 0;
void R485_IRQHandler(void)
{
    if(R485_PORT->SR & R485_SR_IDLE)
    {    
        R485_DisableRxDma();
        g_sR485RcvFrame.len = R485_GetRxLen(); 
        g_sR485RcvFrame.state = UART_STAT_RX_END;    //接收数据完成
    }

    g_nR485Sr = R485_PORT->SR;  //通过读取SR和DR清空中断标志
    g_nR485Dr = R485_PORT->DR;    
}


void Chag_Ch1RxDMAIRQHandler(void)
{
    Chag_Ch1DisableRxDma(); //接收缓冲区满，一般不会出现这样的情况，如果有，就可能系统有故障
    g_sChagOpInfo[CHAG_CH_INDEX0].rxFrame.len = Chag_Ch1GetRxLen(); 
    g_sChagOpInfo[CHAG_CH_INDEX0].rxFrame.state = UART_STAT_RX_OVR;    //接收数据完成
}

void Chag_Ch1TxDMAIRQHandler(void)
{
    Chag_Ch1DisableTxDma();                                             //DMA完成后，最后一个字节可能没有发送出去，需要在主循环中做判断处理
    Chag_Ch1EnableRxDma();                                              //使能接收
}

u16 g_nChagCh1Sr = 0;
u16 g_nChagCh1Dr = 0;
void Chag_Ch1IRQHandler(void)
{
    if(CHAG_CH1_PORT->SR & CHAG_CH1_SR_IDLE)
    {    
        Chag_Ch1DisableRxDma();
        g_sChagOpInfo[CHAG_CH_INDEX0].rxFrame.len = Chag_Ch1GetRxLen(); 
        g_sChagOpInfo[CHAG_CH_INDEX0].rxFrame.state = UART_STAT_RX_END;     //接收数据完成
    }

    g_nChagCh1Sr = CHAG_CH1_PORT->SR;                                       //通过读取SR和DR清空中断标志
    g_nChagCh1Dr = CHAG_CH1_PORT->DR;    
}

u16 g_nChagCh2Sr = 0;
u16 g_nChagCh2Dr = 0;
void Chag_Ch2IRQHandler(void)
{
    if(CHAG_CH2_PORT->SR & USART_FLAG_RXNE)
    {    
        g_nChagCh2Dr = Chag_Ch2ReadByte();
        g_sChagOpInfo[CHAG_CH_INDEX1].rxFrame.buffer[g_sChagOpInfo[CHAG_CH_INDEX1].rxFrame.len++] = g_nChagCh2Dr;
        if(g_sChagOpInfo[CHAG_CH_INDEX1].rxFrame.len  >= CHAG_RX_FRAME_LEN)
        {
            g_sChagOpInfo[CHAG_CH_INDEX1].rxFrame.state = UART_STAT_RX_OVR;     //接收数据完成
        }
        else
        {
            g_sChagOpInfo[CHAG_CH_INDEX1].rxFrame.state = UART_STAT_RX_ING;
        }
        g_sChagOpInfo[CHAG_CH_INDEX1].rxFrame.rxTick = g_nSysTick;
    }

    g_nChagCh2Sr = CHAG_CH2_PORT->SR;                                       //通过读取SR和DR清空中断标志
    g_nChagCh2Dr = CHAG_CH2_PORT->DR;    
}


void Bat_Ch1RxDMAIRQHandler(void)
{
    Bat_Ch1DisableRxDma(); //接收缓冲区满，一般不会出现这样的情况，如果有，就可能系统有故障
    g_sBatOpInfo[BAT_CH_INDEX0].rxFrame.len = Bat_Ch1GetRxLen(); 
    g_sBatOpInfo[BAT_CH_INDEX0].rxFrame.state = UART_STAT_RX_OVR;    //接收数据完成
}

void Bat_Ch1TxDMAIRQHandler(void)
{
    Bat_DisableTxDMA(BAT_CH_INDEX0);                                             //DMA完成后，最后一个字节可能没有发送出去，需要在主循环中做判断处理
    Bat_EnableRxDMA(BAT_CH_INDEX0);                                              //使能接收
}

u16 g_nCh1BatSr = 0;
u16 g_nCh1BatDr = 0;
void Bat_Ch1IRQHandler(void)
{
    if(BAT_CH1_PORT->SR & BAT_CH1_SR_IDLE)
    {    
        Bat_DisableRxDMA(BAT_CH_INDEX0);
        g_sBatOpInfo[BAT_CH_INDEX0].rxFrame.len = Bat_Ch1GetRxLen(); 
        g_sBatOpInfo[BAT_CH_INDEX0].rxFrame.state = UART_STAT_RX_END;    //接收数据完成
    }

    g_nCh1BatSr = BAT_CH1_PORT->SR;  //通过读取SR和DR清空中断标志
    g_nCh1BatDr = BAT_CH1_PORT->DR;    
}

void Bat_Ch2RxDMAIRQHandler(void)
{
    Bat_Ch2DisableRxDma(); //接收缓冲区满，一般不会出现这样的情况，如果有，就可能系统有故障
    g_sBatOpInfo[BAT_CH_INDEX1].rxFrame.len = Bat_Ch2GetRxLen(); 
    g_sBatOpInfo[BAT_CH_INDEX1].rxFrame.state = UART_STAT_RX_OVR;    //接收数据完成
}

void Bat_Ch2TxDMAIRQHandler(void)
{
    Bat_DisableTxDMA(BAT_CH_INDEX1);                                             //DMA完成后，最后一个字节可能没有发送出去，需要在主循环中做判断处理
    Bat_EnableRxDMA(BAT_CH_INDEX1);                                              //使能接收
}

u16 g_nCh2BatSr = 0;
u16 g_nCh2BatDr = 0;
void Bat_Ch2IRQHandler(void)
{
    if(BAT_CH2_PORT->SR & BAT_CH2_SR_IDLE)
    {    
        Bat_DisableRxDMA(BAT_CH_INDEX1);
        g_sBatOpInfo[BAT_CH_INDEX1].rxFrame.len = Bat_Ch2GetRxLen(); 
        g_sBatOpInfo[BAT_CH_INDEX1].rxFrame.state = UART_STAT_RX_END;    //接收数据完成
    }

    g_nCh2BatSr = BAT_CH2_PORT->SR;  //通过读取SR和DR清空中断标志
    g_nCh2BatDr = BAT_CH2_PORT->DR;    
}



/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
