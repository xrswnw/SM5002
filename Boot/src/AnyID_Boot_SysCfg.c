#include "AnyID_Boot_SysCfg.h"
#include "AnyID_Boot_Fram.h"
#include "AnyID_Boot_SysTick_HL.h"
#include "AnyID_Boot_FRam.h"
#include "AnyID_Boot_R485.h"

u32 g_nSysState = 0;
u32 g_nDeviceNxtEraseAddr = 0;
u32 g_nDeviceNxtDownloadAddr = 0;


#define SYS_BOOT_VER_SIZE               50
const u8 SYS_BOOT_VERSION[SYS_BOOT_VER_SIZE] = "Boot V3.0_20030600 GD32F1xx";

void Sys_Delayms(u32 n)             //ϵͳ��ʱn����
{
    n *= 0x3800;
    n++;
    while(n--);
}

void Sys_CfgClock(void)
{
    ErrorStatus HSEStartUpStatus;

    RCC_DeInit();
    //Enable HSE
    RCC_HSEConfig(RCC_HSE_ON);

    //Wait till HSE is ready
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if(HSEStartUpStatus == SUCCESS)
    {
        //HCLK = SYSCLK = 67.8M
        RCC_HCLKConfig(RCC_SYSCLK_Div1);

        //PCLK2 = HCLK = 67.8M
        RCC_PCLK2Config(RCC_HCLK_Div1);

        //PCLK1 = HCLK/2 = 33.9M
        RCC_PCLK1Config(RCC_HCLK_Div2);

        //ADCCLK = PCLK2/2
        RCC_ADCCLKConfig(RCC_PCLK2_Div2);

        //Flash 2 wait state
        FLASH_SetLatency(FLASH_Latency_2);

        //Enable Prefetch Buffer
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        //PLLCLK = 12.00MHz * 6 = 72.0 MHz
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);    //PLL���������

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
                           RCC_APB2Periph_ADC1 |
                           RCC_APB2Periph_USART1, state);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 |
                           RCC_APB1Periph_USART3, state);
}

void Sys_CfgNVIC(void)
{
    //NVIC_InitTypeDef NVIC_InitStructure;
#ifdef  VECT_TAB_RAM
    //Set the Vector Table base location at 0x20000000
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  //VECT_TAB_FLASH
    //Set the Vector Table base location at 0x08000000
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
#endif
    //Configure the Priority Group to 2 bits
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}


const PORT_INF SYS_OUTCTRL_R0 = {GPIOB, GPIO_Pin_2};       
const PORT_INF SYS_OUTCTRL_G0 = {GPIOB, GPIO_Pin_3};
const PORT_INF SYS_OUTCTRL_B0 = {GPIOB, GPIO_Pin_4};
const PORT_INF SYS_OUTCTRL_R1 = {GPIOB, GPIO_Pin_5};       
const PORT_INF SYS_OUTCTRL_G1 = {GPIOB, GPIO_Pin_8};
const PORT_INF SYS_OUTCTRL_B1 = {GPIOB, GPIO_Pin_9}; 
void Sys_InitOutCtrl(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = SYS_OUTCTRL_R0.Pin;
    GPIO_Init(SYS_OUTCTRL_R0.Port, &GPIO_InitStructure);
    Sys_CtrlLedR0Off();

    GPIO_InitStructure.GPIO_Pin = SYS_OUTCTRL_G0.Pin;
    GPIO_Init(SYS_OUTCTRL_G0.Port, &GPIO_InitStructure);
    Sys_CtrlLedG0Off();

    GPIO_InitStructure.GPIO_Pin = SYS_OUTCTRL_B0.Pin;
    GPIO_Init(SYS_OUTCTRL_B0.Port, &GPIO_InitStructure);
    Sys_CtrlLedB0Off();

    GPIO_InitStructure.GPIO_Pin = SYS_OUTCTRL_R1.Pin;
    GPIO_Init(SYS_OUTCTRL_R1.Port, &GPIO_InitStructure);
    Sys_CtrlLedR1Off();

    GPIO_InitStructure.GPIO_Pin = SYS_OUTCTRL_G1.Pin;
    GPIO_Init(SYS_OUTCTRL_G1.Port, &GPIO_InitStructure);
    Sys_CtrlLedG1Off();

    GPIO_InitStructure.GPIO_Pin = SYS_OUTCTRL_B1.Pin;
    GPIO_Init(SYS_OUTCTRL_B1.Port, &GPIO_InitStructure);
    Sys_CtrlLedB1Off();
}

void Sys_Init(void)
{
#if SYS_ENABLE_WDT
    WDG_InitIWDG();
#endif
    
    Sys_CfgClock();

    Sys_CfgNVIC();
    Sys_CfgPeriphClk(ENABLE);

    //��ֹ�ж�
    Sys_DisableInt();

    Sys_InitOutCtrl();

    Sys_Delayms(10);        //FRAM�ϵ������ʱ10ms

    FRam_InitInterface();
    Fram_ReadBootParamenter();
    //���appState״̬���������ǰ汾��ϢУ����󣬻ָ�Ĭ��״̬
    if(g_sFramBootParamenter.appState == FRAM_BOOT_APP_OK)
    {
        if(Sys_CheckVersion() == FALSE)
        {
            g_sFramBootParamenter.appState = FRAM_BOOT_APP_FAIL;
            g_sFramBootParamenter.br = FRAM_BR_9600;
        }
    }
    
    R485_InitInterface(R485_BAUDRARE);
    R485_ConfigInt();
    R485_EnableInt(ENABLE, DISABLE);

    //SysTick ��ʼ�� 5ms
    STick_InitSysTick();


    //ϵͳ����״̬
    if(g_sFramBootParamenter.appState == FRAM_BOOT_APP_OK)
    {
        a_SetState(g_nSysState, SYS_STAT_IDLE);
    }
    else
    {
        a_SetState(g_nSysState, SYS_STAT_DOWNLOAD);
    }

    //ʹ���ж�
    Sys_EnableInt();
}

void Sys_LedTask(void)
{
    if(a_CheckStateBit(g_nSysState, SYS_STAT_RUNLED))
    {
        static u32 ledTimes = 0;

        a_ClearStateBit(g_nSysState, SYS_STAT_RUNLED);

        ledTimes++;
        if(ledTimes & 0x01)
        {
            Sys_CtrlLed0(0, 0, 0);
            Sys_CtrlLed1(0, 0, 0);
        }
        else
        {
            Sys_CtrlLed0(1, 0, 0);
            Sys_CtrlLed1(1, 0, 0);
        }
    #if SYS_ENABLE_WDT
        WDG_FeedIWDog();
    #endif
    }
}

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;
void Sys_Jump(u32 address)
{
    u32 stackAddr = 0;
    Sys_DisableInt();
    stackAddr = *((vu32 *)address);
    //�鿴ջ��ַ�Ƿ���RAM������CCRAM��
    if((stackAddr & 0x2FFE0000) == 0x20000000)
    {
        JumpAddress = *(vu32 *)(address + 4);
        Jump_To_Application = (pFunction) JumpAddress;

        __set_MSP(*(vu32 *)address);
        Jump_To_Application();
    }
    //else
    {
        a_SetState(g_nSysState, SYS_STAT_IDLE);
        g_nSysTick = 0;
    }
    //while(1)
    {
    #if SYS_ENABLE_WDT
        WDG_FeedIWDog();
    #endif
    }
    Sys_EnableInt();
}

void Sys_BootTask(void)
{
    if(!a_CheckStateBit(g_nSysState, SYS_STAT_DOWNLOAD))
    {
        if(g_nSysTick > 40 && g_sFramBootParamenter.appState == FRAM_BOOT_APP_OK)                //TICK = STICK_TIME_MS = 5MS, ��ʱ200ms�ȴ�bootѡ��
        {
            a_SetStateBit(g_nSysState, SYS_STAT_JMP);
        }
    }

    if(a_CheckStateBit(g_nSysState, SYS_STAT_JMP))
    {
        Sys_Jump(SYS_APP_START_ADDR);
    }
}

void Sys_R485Task(void)
{
    //���ڴ�����:���³�ʼ��
    if(USART_GetFlagStatus(R485_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
    {
        USART_ClearFlag(R485_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
        R485_InitInterface(R485_BAUDRARE);
        R485_ConfigInt();
        R485_EnableInt(ENABLE, DISABLE);
    }

    //��������֡����
    if(Uart_IsRcvFrame(g_sR485RcvFrame))
    {
        Sys_ProcessBootFrame(&g_sR485RcvFrame, SYS_COM_R485);
        Uart_ResetFrame(g_sR485RcvFrame);
    }
}

BOOL Sys_CheckVersion(void)
{
    BOOL b = FALSE;
    u8 *p = (u8 *)SYS_BOOT_VER_ADDR;
    u8 i = 0, c = 0;
        
    if(memcmp(p, SYS_VER_HEAD, SYS_VER_HEAD_SIZE) == 0) //�豸�ͺ���ȷ
    {
        for(i = SYS_VER_HEAD_SIZE; i < SYS_VERSION_SIZE; i++)
        {
            c = *p++;
            if((c < ' ' || c > 127) && (c != 0x00))
            {
                break;
            }
        }
        if(i == SYS_VERSION_SIZE)
        {
            b = TRUE;
        }
    }
    
    return b;
}

void Sys_ProcessBootFrame(UART_RCVFRAME *pRcvFrame, u8 com)
{
    u16 crc1 = 0, crc2 = 0;
    memcpy(&g_sUartTempRcvFrame, pRcvFrame, sizeof(UART_RCVFRAME));

    crc1 = Uart_GetFrameCrc(g_sUartTempRcvFrame.buffer, g_sUartTempRcvFrame.index);
    crc2 = a_GetCrc(g_sUartTempRcvFrame.buffer + UART_FRAME_POS_LEN, g_sUartTempRcvFrame.index - 4);

    if(crc1 == crc2)
    {
        u8 cmd = g_sUartTempRcvFrame.buffer[UART_FRAME_POS_CMD];
        u16 destAddr = 0;
        
        destAddr = *((u16 *)(pRcvFrame->buffer + UART_FRAME_POS_DESTADDR));
        if((destAddr != SYS_FRAME_BROADCAST_ADDR) && (destAddr != g_sFramBootParamenter.addr))
        {
            return;
        }
        switch(cmd)
        {
            case UART_FRAME_CMD_RESET:
                g_nSysTick = 0;
                g_sUartTxFrame.len = Uart_RspReset();
                break;
            case UART_FRAME_CMD_ERASE:
                if(a_CheckStateBit(g_nSysState, SYS_STAT_DOWNLOAD))
                {
                    BOOL bOk = FALSE;
                    u32 addr = 0;
                    u8 sector = 0;
                    
                    sector = g_sUartTempRcvFrame.buffer[UART_FRAME_POS_PAR];
                    addr = SYS_APP_START_ADDR + (sector << 10);         //ÿ������1K
                    
                    if(addr >= SYS_APP_START_ADDR)
                    {
                        if(g_nDeviceNxtEraseAddr == addr)               //������ַ�����������ģ������������δ����
                        {
                            g_nDeviceNxtEraseAddr = addr + (1 << 10);   //ÿ������1K
                            
                            bOk = Uart_EraseFlash(addr); 
                            g_sUartTxFrame.len = Uart_RspErase(bOk);
                        }
                    }
                }
                break;
            case UART_FRAME_CMD_DL:
                if(a_CheckStateBit(g_nSysState, SYS_STAT_DOWNLOAD))
                {
                    BOOL bCheck = FALSE;
                    u32 addr = 0;
                    u32 size = 0;

                    if(g_sUartTempRcvFrame.buffer[UART_FRAME_POS_LEN] == 0x00)
                    {
                        bCheck = (BOOL)(g_sUartTempRcvFrame.buffer[UART_FRAME_POS_PAR + 0]);
                        addr = *((u32 *)(g_sUartTempRcvFrame.buffer + UART_FRAME_POS_PAR + 1));
                        size = *((u32 *)(g_sUartTempRcvFrame.buffer + UART_FRAME_POS_PAR + 5));
                        if(addr >= SYS_APP_START_ADDR)
                        {
                            //��һ�β���Ҫ�ж���ַ�������⣬��Ϊboot�����ǴӺ���ǰд���ݣ���һ�β�֪����ַ��ʲô
                            if(addr + size == g_nDeviceNxtDownloadAddr || g_nDeviceNxtDownloadAddr == 0)
                            {
                                g_nDeviceNxtDownloadAddr = addr;
                                //֡����֮ǰ���� + ��������(1 + 4 + 4) + size + crclen;
                                //frameLen = UART_FRAME_POS_PAR + 9 + size + 2;
                                Sys_CtrlLed0(1, 1, 1);
                                Sys_CtrlLed1(1, 1, 1);
                                if(BL_WriteImagePage(addr, g_sUartTempRcvFrame.buffer + UART_FRAME_POS_PAR + 9, size))
                                {
                                    g_sUartTxFrame.len = Uart_RspDownload(bCheck, addr, size);
                                }
                                Sys_CtrlLed0(0, 0, 0);
                                Sys_CtrlLed1(0, 0, 0);
                            }
                        }
                    }
                }                              
                break;
            case UART_FRAME_CMD_BOOT:
                g_sUartTxFrame.len = Uart_RspBoot();
                if(g_sUartTxFrame.len)      //���������������ʱ��ϳ�
                {
                    R485_WriteBuffer(g_sUartTxFrame.frame, g_sUartTxFrame.len);
                    g_sUartTxFrame.len = 0;
                    Sys_Delayms(2);         //�ȴ����һ���ֽڷ������
                }
                a_ClearStateBit(g_nSysState, SYS_STAT_IDLE);
                a_SetStateBit(g_nSysState, SYS_STAT_DOWNLOAD);
                g_sFramBootParamenter.appState = FRAM_BOOT_APP_FAIL;
                Fram_WriteBootParamenter();
                FLASH_Unlock();
                
                Uart_EraseFlash(SYS_BOOT_VER_ADDR);          //�汾��Ϣ�������
                g_nDeviceNxtEraseAddr = SYS_APP_START_ADDR;
                g_nDeviceNxtDownloadAddr = 0;                   //boot���ɺ���ǰд������
                break;
            case UART_FRAME_CMD_JMP:
                if(Sys_CheckVersion() == TRUE)
                {
                    g_sUartTxFrame.len = Uart_RspJmp();
                    a_SetStateBit(g_nSysState, SYS_STAT_JMP);
                    g_sFramBootParamenter.appState = FRAM_BOOT_APP_OK;
                    Fram_WriteBootParamenter();
                    FLASH_Lock();
                }
                break;
            case UART_FRAME_CMD_VER:
                g_sUartTxFrame.len = Uart_RspFrame(g_sUartTxFrame.frame, cmd, (u8 *)SYS_BOOT_VERSION, SYS_BOOT_VER_SIZE, UART_FRAME_FLAG_OK, UART_FRAME_RSP_NOERR);
                break;
            default:
                break;
        }
    }
    if(g_sUartTxFrame.len)
    {
        R485_WriteBuffer(g_sUartTxFrame.frame, g_sUartTxFrame.len);
        g_sUartTxFrame.len = 0;
        Sys_Delayms(2);         //�ȴ����һ���ֽڷ������
    }
}

