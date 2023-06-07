#ifndef _ANYID_SM5002_CONFIG_
#define _ANYID_SM5002_CONFIG_

#include "AnyID_Type.h"
#include "AnyID_Stdlib.h"

//�趨����ʱ��
#define SYS_CLOCK_FREQ                  120000000
#define SYS_ENABLE_WDT                  1


//STM32ƽ̨���
#if (defined(_ANYID_STM32_) || defined(_ANYID_GD32_))
    #include "stm32f10x.h"

    //�˿ڶ���
    typedef struct Port_Inf
    {
        GPIO_TypeDef* Port;
        u16 Pin;
    } PORT_INF;
#endif

#define STM32_CPUID_LEN                 12
#define STM32_CPUID_ADDR                (0x1FFFF7E8)

//���ȼ�������2:2
#define INT_PRIORITY_GROUP              2
#define INT_PRIORITY_STICK              0  //STICK�����ȼ����
#define INT_PRIORITY_UART_RX            14  //���ڽ���
#define INT_PRIORITY_UART_DMA           8  //���ڷ���DMA
#define INT_PRIORITY_PCD_IO             4   //15693�ķ��Ͷ�ʱ�����ȼ����

#define Sys_EnableInt()     __enable_irq()
#define Sys_DisableInt()    __disable_irq()

#define Sys_GetCpuId(id)    memcpy((id), (u8 *)(0x1ffff7e8), STM32_CPUID_LEN)
#define Sys_SoftReset()     (*((u32 *)0xE000ED0C) = 0x05fa0004)
#endif


