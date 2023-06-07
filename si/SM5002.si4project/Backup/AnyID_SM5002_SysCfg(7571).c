#include "AnyID_SM5002_SysCfg.h"
#include "AnyID_SM5002_Device.h"

u32 g_nSysState = SYS_STAT_IDLE;
u32 g_nAlarmDelay = 0;

#define SYS_ENABLE_WDT               0

#if SYS_ENABLE_WDT
    #define SYS_ENABLE_TEST         0
#else
    #define SYS_ENABLE_TEST         1
#endif

void Sys_Delayms(u32 n)
{
    n *= 0x3800;
    n++;
    while(n--);
}

void Sys_CfgClock(void)
{
    ErrorStatus HSEStartUpStatus = ERROR;

    RCC_DeInit();
    //Enable HSE
    RCC_HSEConfig(RCC_HSE_ON);

    //Wait till HSE is ready
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if(HSEStartUpStatus == SUCCESS)
    {
        //HCLK = SYSCLK = 72.0M
        RCC_HCLKConfig(RCC_SYSCLK_Div1);

        //PCLK2 = HCLK = 72.0M
        RCC_PCLK2Config(RCC_HCLK_Div1);

        //PCLK1 = HCLK/2 = 33.9M
        RCC_PCLK1Config(RCC_HCLK_Div2);

        //ADCCLK = PCLK2/6
        RCC_ADCCLKConfig(RCC_PCLK2_Div6);

        // Select USBCLK source 72 / 1.5 = 48M
        RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

        //Flash 2 wait state
        FLASH_SetLatency(FLASH_Latency_2);

        //Enable Prefetch Buffer
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        //PLLCLK = 12.00MHz * 6 = 72 MHz
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);    //PLL在最后设置

        //Enable PLL
        RCC_PLLCmd(ENABLE);

        //Wait till PLL is ready
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        //Select PLL as system clock source
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        //Wait till PLL is used as system clock source
        while(RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
}

void Sys_CfgPeriphClk(FunctionalState state)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOC |
                           RCC_APB2Periph_AFIO |
                           RCC_APB2Periph_ADC1, state);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, state);
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 |
                          RCC_AHBPeriph_DMA2, state);
}

void Sys_CfgNVIC(void)
{
    //NVIC_InitTypeDef NVIC_InitStructure;
#ifdef  VECT_TAB_RAM
    //Set the Vector Table base location at 0x20000000
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  //VECT_TAB_FLASH
    //Set the Vector Table base location at 0x08000000
#ifdef _ANYID_BOOT_STM32_
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
#else
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
#endif
#endif

    //Configure the Priority Group to 2 bits
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}


void Sys_Init(void)
{
    Sys_CfgClock();
    Sys_CfgNVIC();
    Sys_CfgPeriphClk(ENABLE);

#if SYS_ENABLE_WDT
    WDG_InitIWDG();
#endif

    //禁止中断
    Sys_DisableInt();
    
    Device_InitAddr();
    g_sDeviceOpInfo.addr = Device_ReadAddr();
    
    Device_InitOutCtrl();
    g_sDeviceOpInfo.outCtrl.ledBlue0 = 1;
    g_sDeviceOpInfo.outCtrl.ledBlue1 = 1;
    g_sDeviceOpInfo.outCtrl.ledRed0 = 1;
    g_sDeviceOpInfo.outCtrl.ledRed1 = 1;
    g_sDeviceOpInfo.outCtrl.ledGreen0 = 1;
    g_sDeviceOpInfo.outCtrl.ledGreen1 = 1;
    g_sDeviceOpInfo.outCtrl.fanCtrl0 = 1;
    g_sDeviceOpInfo.outCtrl.doorLock1 = 1;
    Device_OutCtrl(&g_sDeviceOpInfo.outCtrl);
    Sys_Delayms(500);
    g_sDeviceOpInfo.outCtrl.doorLock1 = 0;
    Device_OutCtrl(&g_sDeviceOpInfo.outCtrl);

    Device_InitInSensor();
    Device_ReadInInfo(&g_sDeviceOpInfo.inInfo);
    
    Sys_RunLedOn();
    Sys_AlarmLedOn();

    Sys_Delayms(10);    //
    FRam_InitInterface();
#if SYS_ENABLE_WDT
    WDG_FeedIWDog();
#endif
    Device_ReadDeviceParamenter();

    //SysTick 初始化 5ms
    STick_InitSysTick();
    
    AD_InitInterface();
    AD_Init();
    AD_DmaInit();

    Sys_RunLedOff();
    Sys_AlarmLedOff();
    
    //使能中断
    Sys_EnableInt();

    //系统空闲状态
    a_SetState(g_nSysState, SYS_STAT_IDLE);
}

void Sys_LedTask(void)
{
    if(a_CheckStateBit(g_nSysState, SYS_STAT_RUNLED))
    {
        a_ClearStateBit(g_nSysState, SYS_STAT_RUNLED);
        
    #if SYS_ENABLE_WDT
        WDG_FeedIWDog();
    #endif
    }

}

void Sys_AdTask(void)   //检测电压是否符合要求
{
    if(a_CheckStateBit(g_nSysState, SYS_STAT_AD))
    {
        a_ClearStateBit(g_nSysState, SYS_STAT_AD);

        Win_CalAvg(&g_sDeviceOpInfo.tmpr[0], AD_GetTemper1Value());
        Win_CalAvg(&g_sDeviceOpInfo.tmpr[1], AD_GetTemper2Value());

        AD_GetTmpr(&g_sDeviceOpInfo.tmpr[0]);
        AD_GetTmpr(&g_sDeviceOpInfo.tmpr[1]);
    }
}


void Sys_DeviceTask(void)
{
    
}
