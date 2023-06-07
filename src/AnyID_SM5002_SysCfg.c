#include "AnyID_SM5002_SysCfg.h"


u32 g_nSysState = SYS_STAT_IDLE;
u32 g_nAlarmDelay = 0;

#if SYS_ENABLE_WDT
    #define SYS_ENABLE_TEST         0
#else
    #define SYS_ENABLE_TEST         1
#endif

void Sys_Delayms(u32 n)
{
    n *= 0x6000;
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
        RCC_ADCCLKConfig(RCC_PCLK2_Div8);

        // Select USBCLK source 72 / 1.5 = 48M
        RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

        //Flash 2 wait state
        FLASH_SetLatency(FLASH_Latency_2);

        //Enable Prefetch Buffer
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        //PLLCLK = 12.00MHz * 12 = 120 MHz
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_10);    //PLL在最后设置

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
                           RCC_APB2Periph_GPIOD |
                           RCC_APB2Periph_AFIO |
                           RCC_APB2Periph_ADC1 |
                           RCC_APB2Periph_USART1, state);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 |
                           RCC_APB1Periph_USART3 |
                           RCC_APB1Periph_UART4 |
                           RCC_APB1Periph_UART5, state);
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 |
                          RCC_AHBPeriph_DMA2, state);
    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
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
    
    WDG_InitIWDG();

    //禁止中断
    Sys_DisableInt();
    
    FRam_InitInterface();
    Device_InitAddr();
    g_sDeviceOpInfo.addr = Device_ReadAddr();
    
    Device_InitOutCtrl();

    Device_CtrlFan(TRUE, DEVICE_GATE_INDEX0);
    Device_CtrlFan(TRUE, DEVICE_GATE_INDEX1);

    Device_CtrlLed(DEVICE_GATE_ALL, DEVICE_LED_COLOR_RED);   WDG_FeedIWDog(); Sys_Delayms(1000);
    Device_CtrlLed(DEVICE_GATE_ALL, DEVICE_LED_COLOR_GREEN); WDG_FeedIWDog(); Sys_Delayms(1000);
    Device_CtrlLed(DEVICE_GATE_ALL, DEVICE_LED_COLOR_BLUE);  WDG_FeedIWDog(); Sys_Delayms(1000);
    Device_CtrlLed(DEVICE_GATE_ALL, DEVICE_LED_COLOR_YELLOW);  WDG_FeedIWDog(); Sys_Delayms(1000);
    Device_CtrlLed(DEVICE_GATE_ALL, DEVICE_LED_COLOR_WHITE);  WDG_FeedIWDog(); Sys_Delayms(1000);
    Device_CtrlLed(DEVICE_GATE_ALL, DEVICE_LED_COLOR_CHING);  WDG_FeedIWDog(); Sys_Delayms(1000);
    Device_CtrlLed(DEVICE_GATE_ALL, 0);  WDG_FeedIWDog(); Sys_Delayms(1000);
    WDG_FeedIWDog();
    Device_InitInSensor();
    
    Sys_RunLedOn();
    Sys_AlarmLedOn();

    Device_ReadDeviceParamenter();

    //SysTick 初始化 5ms
    STick_InitSysTick();
    
    AD_InitInterface();
    AD_Init();
    AD_DmaInit();

    R485_Init();
    
    Chag_Init(CHAG_CH_INDEX0);
    Bat_Init(BAT_CH_INDEX0);
    Device_Init(DEVICE_GATE_INDEX0);
    
    Chag_Init(CHAG_CH_INDEX1);
    Bat_Init(BAT_CH_INDEX1);
    Device_Init(DEVICE_GATE_INDEX1);

    memset(&g_sDeviceOpInfo.outCtrl, 0, sizeof(DEVICE_OUTCTRL));
    
    Device_CtrlFan(FALSE, DEVICE_GATE_INDEX0);
    Device_CtrlFan(FALSE, DEVICE_GATE_INDEX1);
    Device_CtrlLed(DEVICE_GATE_ALL, DEVICE_LED_COLOR_WHITE);

    Sys_RunLedOff();
    Sys_AlarmLedOff();
    
    Device_CtrlLock0Open();
    Device_CtrlLock0Close();
    Device_CtrlLock1Open();
    Device_CtrlLock1Close();
    
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

void Sys_R485Task(void)
{
    if(USART_GetFlagStatus(R485_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
    {
        USART_ClearFlag(R485_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);

        R485_Stop();
        Sys_Delayms(10);
        R485_Init();
    }

    if(Uart_IsRcvFrame(g_sR485RcvFrame) && g_sR485TxFrame.state == UART_STAT_TX_IDLE)
    {
        BOOL bTx = FALSE;
        memcpy(&g_sUartTempRcvFrame, &g_sR485RcvFrame, sizeof(UART_RCVFRAME));
        Uart_ResetFrame(g_sR485RcvFrame);
        
        if(g_sUartTempRcvFrame.len >= UART_FRAME_MIN_LEN)
        {
            u16 startPos = 0;
            if(Uart_UsrCheckFrame(&g_sUartTempRcvFrame, &startPos, g_sDeviceOpInfo.addr))
            {
                g_sR485TxFrame.len = Device_ProcessUsrFrame(g_sUartTempRcvFrame.buffer + startPos);
                if(g_sR485TxFrame.len)
                {
                    if(g_sR485TxFrame.buffer[UART_FRAME_POS_CMD + 1] == UART_FRAME_CMD_RST)  //响应帧命令的位置和请求帧不一样
                    {
                        R485_WriteBuffer(g_sR485TxFrame.buffer, g_sR485TxFrame.len);
                        Sys_Delayms(5);
                        Sys_SoftReset();
                    }
                    
                    g_sR485TxFrame.state = UART_STAT_TX_BUSY;
                    g_sR485TxFrame.tick = g_nSysTick;
                    R485_EnableTxDma(g_sR485TxFrame.buffer, g_sR485TxFrame.len);

                    bTx = TRUE;
                }
            }
        }
        if(bTx == FALSE)
        {
            R485_EnableRxDma();
        }
    }

    if(g_sR485TxFrame.state == UART_STAT_TX_BUSY)
    {
        if(g_sR485TxFrame.tick + UART_TX_TIMEOUT < g_nSysTick)
        {
            g_sR485TxFrame.state = UART_STAT_TX_IDLE;
            R485_DisableTxDma();
            R485_EnableRxDma();
        }
    }
}


void Sys_AdTask(u8 index)   //检测电压是否符合要求
{
    if(a_CheckStateBit(g_nSysState, (SYS_STAT_AD0 << index)))
    {
        DEVICE_GATEINFO *pGateInfo = &g_sDeviceGateInfo[index];
        
        a_ClearStateBit(g_nSysState, (SYS_STAT_AD0 << index));
        
        Win_CalAvg(&pGateInfo->tmpr, AD_GetTemperValue(index));
        AD_GetTmpr(&pGateInfo->tmpr);
        if(AD_CheckAlarm(&pGateInfo->tmpr, g_sDeviceParams.alarmTmpr))
        {
            if(pGateInfo->sensorState.fan == 0)
            {
                Device_CtrlFan(TRUE, index);
                pGateInfo->sensorState.fan = 1;
                pGateInfo->rcdFlag = DEVICE_RCD_FLAG_SENSOR;
            }
        }
        else
        {
            if(pGateInfo->sensorState.fan == 1 && Device_CheckUsrFan(&g_sDeviceOpInfo.usrCtrl, index) == 0)  //用户没有手动控制风扇
            {
                Device_CtrlFan(FALSE, index);
                pGateInfo->sensorState.fan = 0;
                pGateInfo->rcdFlag = DEVICE_RCD_FLAG_SENSOR;
            }
        }
    }
}

void Sys_SensorTask(u8 index)
{
    if(a_CheckStateBit(g_nSysState, (SYS_STAT_IN_SENSOR0 << index)))        //定时读取传感器状态
    {
        u16 changFlag = 0;
        
        a_ClearStateBit(g_nSysState, (SYS_STAT_IN_SENSOR0 << index));

        Device_ReadInInfo(&g_sDeviceInCheck[index].curInState, index);
        changFlag = Device_CheckInSensor(&g_sDeviceInCheck[index], (u8 *)&g_sDeviceGateInfo[index].sensorState);
        if(changFlag)
        {
            g_sDeviceGateInfo[index].rcdFlag = DEVICE_RCD_FLAG_SENSOR;
            if(changFlag & DEVICE_IN_CHK_FLAG_DOOR)                                 //门状态发生变化
            {
                if(g_sDeviceGateInfo[index].sensorState.door == DEVICE_DOOR_CLOSE &&
                   (g_sDeviceGateInfo[index].mode != DEVICE_GATE_MODE_RTNBAT) &&
                   (g_sDeviceGateInfo[index].mode != DEVICE_GATE_MODE_BRWBAT))  //门被关闭
                {
                    Device_Init(index);
                }
            }
        }
    }
}

void Sys_ChargeTask(u8 index)
{
    CHAG_OPINFO *pOpInfo = NULL;

    pOpInfo = g_sChagOpInfo + index;
    
    if(Chag_UartCheckErr(index))
    {
        Chag_Stop(index);
        Sys_Delayms(10);
        Chag_Init(index);
    }

    if(pOpInfo->state)
    {
        if(Uart_IsRcvFrame(pOpInfo->rxFrame))
        {
            if(pOpInfo->rxFrame.len >= CHAG_FRAME_MIN_LEN && pOpInfo->state == CHAG_OP_STAT_WAIT)
            {
                u16 startPos = 0;
                if(Chag_CheckFrame(&pOpInfo->rxFrame, &startPos, pOpInfo->fun))
                {
                    Chag_FormatRxInfo(pOpInfo->rxFrame.buffer + startPos, pOpInfo);
                    pOpInfo->state = CHAG_OP_STAT_RX | CHAG_OP_STAT_NEXT;
                }
            }
            Uart_ResetFrame(pOpInfo->rxFrame);
            if(pOpInfo->state == CHAG_OP_STAT_WAIT)  //如果没有获取想要的响应帧，继续接收数据
            {
                Chag_EnableRxDMA(index);
            }
        }
        
        if(pOpInfo->state == CHAG_OP_STAT_TX)
        {
            if(pOpInfo->index < pOpInfo->num)
            {
                pOpInfo->txFrame.len = Chag_FormatOpFrame(pOpInfo, pOpInfo->txFrame.buffer);
                Chag_EnableTxDMA(index, pOpInfo->txFrame.buffer, pOpInfo->txFrame.len);

                pOpInfo->tick = g_nSysTick;
                pOpInfo->state = CHAG_OP_STAT_WAIT;
            }
        }
        else if(pOpInfo->state == CHAG_OP_STAT_WAIT)
        {
            if(pOpInfo->tick + CHAG_OP_WAIT_TIME < g_nSysTick)
            {
                pOpInfo->state = CHAG_OP_STAT_TO | CHAG_OP_STAT_NEXT;
                Chag_DisableTxDMA(index);
                Chag_DisableRxDMA(index);
            }
        }
        else if(a_CheckStateBit(pOpInfo->state, CHAG_OP_STAT_NEXT))
        {
            BOOL bOk = FALSE;
            a_ClearStateBit(pOpInfo->state, CHAG_OP_STAT_NEXT);
            bOk = Chag_GetNext(pOpInfo);
            if(pOpInfo->state == CHAG_OP_STAT_IDLE)
            {
                u8 state = pOpInfo->chagInfo.state;
                if(bOk == FALSE)
                {
                    memset(&pOpInfo->chagInfo, 0, sizeof(CHAG_INFO));
                    pOpInfo->chagInfo.state = CHAG_STAT_EMPTY;            
                }
                else
                {
                    pOpInfo->chagInfo.state |= CHAG_STAT_EXIST;
                    if(Chag_CheckErr(pOpInfo->chagInfo.errStatusInfo))
                    {
                        pOpInfo->chagInfo.state |= CHAG_STAT_ERR;
                    }
                    else
                    {
                        pOpInfo->chagInfo.state &= (~CHAG_STAT_ERR);          //这里要不要清空--------待定？？？？？？？？？？？？？？？？？？？？？？？？？？？
                        if(g_sDeviceGateInfo[index].mode == DEVICE_GATE_MODE_IDLE)    //如果空閒狀態，充電機恢復了，需要重新啟動一次
                        {
                            if((state & CHAG_STAT_EXIST) == 0x00 || (state & CHAG_STAT_ERR) == CHAG_STAT_ERR)
                            {
                                Device_Init(index);
                            }
                        }
                    }
                }
                if(state != pOpInfo->chagInfo.state)
                {
                    g_sDeviceGateInfo[index].rcdFlag = DEVICE_RCD_FLAG_SENSOR;
                }
            }
            pOpInfo->tick = g_nSysTick;
        }
        else if(pOpInfo->state == BAT_OP_STAT_DLY)
        {
            if(pOpInfo->tick + BAT_DELAY_TIME < g_nSysTick)
            {
                pOpInfo->state = BAT_OP_STAT_TX;
            }
        }
    }
}

void Sys_BatTask(u8 index)
{
    BAT_OPINFO *pOpInfo = NULL;

    pOpInfo = g_sBatOpInfo + index;
    
    if(Bat_UartCheckErr(index))
    {
        Bat_Stop(index);
        Sys_Delayms(10);
        Bat_Init(index);
    }

    if(pOpInfo->state)
    {
        if(Uart_IsRcvFrame(pOpInfo->rxFrame))
        {
            if(pOpInfo->rxFrame.len >= BAT_FRAME_MIN_LEN && pOpInfo->state == BAT_OP_STAT_WAIT)
            {
                u16 startPos = 0;
                if(Bat_CheckFrame(&pOpInfo->rxFrame, &startPos, pOpInfo->op[pOpInfo->index]))
                {
                    if(Bat_FormatRxInfo(pOpInfo->rxFrame.buffer + startPos, &pOpInfo->batInfo, pOpInfo->index))
                    {
                        pOpInfo->state = BAT_OP_STAT_RX | BAT_OP_STAT_NEXT;
                    }
                }
            }
            Uart_ResetFrame(pOpInfo->rxFrame);
            if(pOpInfo->state == BAT_OP_STAT_WAIT)  //如果没有获取想要的响应帧，继续接收数据
            {
                Bat_EnableRxDMA(index);
            }
        }
        
        if(pOpInfo->state == BAT_OP_STAT_TX)
        {
            if(pOpInfo->index < pOpInfo->num)
            {
                pOpInfo->txFrame.len = Bat_FormatCmd(pOpInfo->txFrame.buffer, pOpInfo->op[pOpInfo->index], NULL, 0);
                Bat_EnableTxDMA(index, pOpInfo->txFrame.buffer, pOpInfo->txFrame.len);

                pOpInfo->tick = g_nSysTick;
                pOpInfo->state = BAT_OP_STAT_WAIT;
            }
        }
        else if(pOpInfo->state == BAT_OP_STAT_WAIT)
        {
            if(pOpInfo->tick + BAT_OP_WAIT_TIME < g_nSysTick)
            {
                pOpInfo->state = BAT_OP_STAT_TO | BAT_OP_STAT_NEXT;
                Bat_DisableTxDMA(index);
                Bat_DisableRxDMA(index);
            }
        }
        else if(a_CheckStateBit(pOpInfo->state, BAT_OP_STAT_NEXT))
        {
            BOOL bOk = FALSE;
            a_ClearStateBit(pOpInfo->state, BAT_OP_STAT_NEXT);
            bOk = Bat_GetNext(pOpInfo);
            if(pOpInfo->state == BAT_OP_STAT_IDLE)
            {
                u8 state = pOpInfo->batInfo.state;
                
                if(bOk == FALSE)
                {
                    memset(&pOpInfo->batInfo, 0, sizeof(BAT_INFO));
                    pOpInfo->batInfo.state = BAT_STAT_EMPTY;            //无电池，需要关闭充电，增加一条记录
                }
                else
                {
                    pOpInfo->batInfo.state |= BAT_STAT_EXIST;
                    if(Bat_CheckErr(pOpInfo->batInfo.err))
                    {
                        pOpInfo->batInfo.state |= BAT_STAT_ERR;
                    }
                    else
                    {
                        pOpInfo->batInfo.state &= (~BAT_STAT_ERR);                      //这里要不要清空--------待定？？？？？？？？？？？？？？？？？？？？？？？？？？？
                        if(g_sDeviceGateInfo[index].mode == DEVICE_GATE_MODE_IDLE)      //如果空閒狀態，电池恢復了，需要重新啟動一次
                        {
                            if(((state & BAT_STAT_EXIST) == 0x00 || (state & BAT_STAT_ERR) == BAT_STAT_ERR))
                            {
                                Device_Init(index);
                            }
                        }
                    }
                }
                if(state != pOpInfo->batInfo.state)
                {
                    g_sDeviceGateInfo[index].rcdFlag = DEVICE_RCD_FLAG_SENSOR;
                }
            }
            pOpInfo->tick = g_nSysTick;
        }
        else if(pOpInfo->state == BAT_OP_STAT_DLY)
        {
            if(pOpInfo->tick + BAT_DELAY_TIME < g_nSysTick)
            {
                pOpInfo->state = BAT_OP_STAT_TX;
            }
        }
    }
}

void Sys_GateTask(u8 index)
{
    DEVICE_GATEINFO *pGateInfo = g_sDeviceGateInfo + index;
    
    //空闲也添加记录信息
    //开机状态，需要等待操作结束，才可以上报记录
    if((pGateInfo->rcdIdleDelay + pGateInfo->rcdIdleTick < g_nSysTick || pGateInfo->rcdFlag) &&
       (pGateInfo->mode == DEVICE_GATE_MODE_CHARGE || pGateInfo->mode == DEVICE_GATE_MODE_IDLE))
    {
        Device_FomatGateRecord(index, pGateInfo, &g_sDeviceInfoBuf);
    }

    //开门逻辑
    if(pGateInfo->ctrlDoor.bOpen || Device_CheckLockOpen(index))
    {
        if(pGateInfo->sensorState.door == DEVICE_DOOR_OPEN || pGateInfo->ctrlDoor.tick + DEVICE_CTRL_DOOR_TICK < g_nSysTick)
        {
            Device_CtrlDoor(FALSE, index);
            pGateInfo->ctrlDoor.bOpen = FALSE;
        }
    }
    
    if(Chag_IsIdle(pGateInfo->pChagOpInfo) && Bat_IsIdle(pGateInfo->pBatOpInfo) && pGateInfo->tick + pGateInfo->interVal < g_nSysTick)
    {
        pGateInfo->tick = g_nSysTick;
        //任何设备出现故障，都需要停止操作

        if(pGateInfo->mode == DEVICE_GATE_MODE_STARTUP)
        {
            Device_GateStartup(pGateInfo, index);
            Device_GateRefreshLed(pGateInfo, index);
        }
        else if(pGateInfo->mode == DEVICE_GATE_MODE_CHARGE)
        {
            Device_GateCharge(pGateInfo, index);
            Device_GateRefreshLed(pGateInfo, index);
        }
        else if(pGateInfo->mode == DEVICE_GATE_MODE_IDLE)
        {
            Device_CheckCharge(pGateInfo->pBatOpInfo, pGateInfo->pChagOpInfo, index);
            Device_GateRefreshLed(pGateInfo, index);
        }
        else if(pGateInfo->mode == DEVICE_GATE_MODE_RTNBAT)     //借还电池都不需要更新LED的
        {
            Device_GateReturnBat(pGateInfo, index);
            Device_GateRefreshLed(pGateInfo, index);
        }
        else if(pGateInfo->mode == DEVICE_GATE_MODE_BRWBAT)     //借还电池都不需要更新LED的
        {
            Device_GateBorrowBat(pGateInfo, index);
            Device_GateRefreshLed(pGateInfo, index);
        }
    }    
}
