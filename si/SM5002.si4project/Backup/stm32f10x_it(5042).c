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
}

void R485_RxDMAIRQHandler(void)
{
    R485_DisableRxDma(); //���ջ���������һ�㲻��������������������У��Ϳ���ϵͳ�й���
    g_sR485RcvFrame.len = R485_GetRxLen(); 
    g_sR485RcvFrame.state = UART_STAT_RX_OVR;    //�����������
}

void R485_TxDMAIRQHandler(void)
{
    R485_DisableTxDma();                                             //DMA��ɺ����һ���ֽڿ���û�з��ͳ�ȥ����Ҫ����ѭ�������жϴ���
    R485_EnableRxDma();                                              //ʹ�ܽ���
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
        g_sR485RcvFrame.state = UART_STAT_RX_END;    //�����������
    }

    g_nR485Sr = R485_PORT->SR;  //ͨ����ȡSR��DR����жϱ�־
    g_nR485Dr = R485_PORT->DR;    
}


void Chag_RxDMAIRQHandler(void)
{
    Chag_DisableRxDma(); //���ջ���������һ�㲻��������������������У��Ϳ���ϵͳ�й���
    g_sChagRcvFrame.len = Chag_GetRxLen(); 
    g_sChagRcvFrame.state = UART_STAT_RX_OVR;    //�����������
}

void Chag_TxDMAIRQHandler(void)
{
    Chag_DisableTxDma();                                             //DMA��ɺ����һ���ֽڿ���û�з��ͳ�ȥ����Ҫ����ѭ�������жϴ���
    Chag_EnableRxDma();                                              //ʹ�ܽ���
    g_sChagTxFrame.state = UART_STAT_TX_IDLE;
}

u16 g_nChagSr = 0;
u16 g_nChagDr = 0;
void Chag_IRQHandler(void)
{
    if(CHAG_PORT->SR & CHAG_SR_IDLE)
    {    
        Chag_DisableRxDma();
        g_sChagRcvFrame.len = Chag_GetRxLen(); 
        g_sChagRcvFrame.state = UART_STAT_RX_END;   //�����������
    }

    g_nChagSr = CHAG_PORT->SR;                      //ͨ����ȡSR��DR����жϱ�־
    g_nChagDr = CHAG_PORT->DR;    
}

void Bat_RxDMAIRQHandler(void)
{
    Bat_DisableRxDma(); //���ջ���������һ�㲻��������������������У��Ϳ���ϵͳ�й���
    g_sBatRcvFrame.len = Bat_GetRxLen(); 
    g_sBatRcvFrame.state = UART_STAT_RX_OVR;    //�����������
}

void Bat_TxDMAIRQHandler(void)
{
    Bat_DisableTxDma();                                             //DMA��ɺ����һ���ֽڿ���û�з��ͳ�ȥ����Ҫ����ѭ�������жϴ���
    Bat_EnableRxDma();                                              //ʹ�ܽ���
    g_sBatTxFrame.state = UART_STAT_TX_IDLE;
}

u16 g_nBatSr = 0;
u16 g_nBatDr = 0;
void Bat_IRQHandler(void)
{
    if(BAT_PORT->SR & BAT_SR_IDLE)
    {    
        Bat_DisableRxDma();
        g_sBatRcvFrame.len = Bat_GetRxLen(); 
        g_sBatRcvFrame.state = UART_STAT_RX_END;    //�����������
    }

    g_nBatSr = BAT_PORT->SR;  //ͨ����ȡSR��DR����жϱ�־
    g_nBatDr = BAT_PORT->DR;    
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