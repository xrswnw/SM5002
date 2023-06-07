#ifndef _ANYID_BOOT_CONFIG_
#define _ANYID_BOOT_CONFIG_
//******************************************************************************
//����ͷ�ļ�
//******************************************************************************
#include "AnyID_Type.h"
#include "AnyID_Stdlib.h"
#include <stdio.h>

//******************************************************************************
//�궨��
//******************************************************************************
#define SYS_CLOCK_FREQ                  72000000                                    //�趨����ʱ��
#define STM32_CPUID_LEN                 12
#define SYS_ENABLE_WDT                  0                                           //���Ź��궨��

#if (defined(_ANYID_STM32_) || defined(_ANYID_GD32_))                                                    //STM32ƽ̨���
    #include "stm32f10x.h"

    //�˿ڶ���
    typedef struct Port_Inf
    {
        GPIO_TypeDef* Port;
        u16 Pin;
    } PORT_INF;
#endif


#define INT_PRIORITY_GROUP              2                                       //���ȼ�������2:2
#define INT_PRIORITY_STICK              15                                      //STICK�����ȼ����
#define INT_PRIORITY_RF433              0
#define INT_PRIORITY_UART               4                                      //���ڽ���
#define INT_PRIORITY_R485               0                                      //485����
#define INT_PRIORITY_UART_DMA           8                                       //���ڷ���DMA
#define INT_PRIORITY_DOUTDMA            4                                       //ADDMA
#define INT_PRIORITY_EM4094_DIN         0                                       //15693�ķ��Ͷ�ʱ�����ȼ����
#define INT_PRIORITY_SAM_IO             0                                       //SAM��IO���ݣ���INT_PRIORITY_EM4094_DIN����ͬһ��ʱ�̹���
#define INT_PRIORITY_AUDIO_IO           0

#define Sys_EnableInt()                 __enable_irq()                          //����ϵͳ�ж�
#define Sys_DisableInt()                __disable_irq()

#endif