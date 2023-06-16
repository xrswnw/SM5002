#ifndef _ANYID_BOOT_SYSCFG_
#define _ANYID_BOOT_SYSCFG_


#include "AnyID_Boot_Config.h"
#include "AnyID_Boot_Uart.h"

extern u32 g_nSysState;
#define SYS_STAT_IDLE           0x00000001	    //系统初始化后处于空闲状态
#define SYS_STAT_RUNLED         0x00000002
#define SYS_STAT_DOWNLOAD       0x00000040
#define SYS_STAT_JMP            0x00000080
#define SYS_STAT_RESET          0x00000100
#define SYS_STAT_ALARMLED       0x00000200

void Sys_Jump(u32 address);
typedef  void (*pSysFunction)(void);

extern const PORT_INF SYS_OUTCTRL_R0;  
#define Sys_CtrlLedR0On()                   (SYS_OUTCTRL_R0.Port->BSRR = SYS_OUTCTRL_R0.Pin)  
#define Sys_CtrlLedR0Off()                  (SYS_OUTCTRL_R0.Port->BRR = SYS_OUTCTRL_R0.Pin)

extern const PORT_INF SYS_OUTCTRL_G0;
#define Sys_CtrlLedG0On()                   (SYS_OUTCTRL_G0.Port->BSRR = SYS_OUTCTRL_G0.Pin)  
#define Sys_CtrlLedG0Off()                  (SYS_OUTCTRL_G0.Port->BRR = SYS_OUTCTRL_G0.Pin)

extern const PORT_INF SYS_OUTCTRL_B0;
#define Sys_CtrlLedB0On()                   (SYS_OUTCTRL_B0.Port->BSRR = SYS_OUTCTRL_B0.Pin)  
#define Sys_CtrlLedB0Off()                  (SYS_OUTCTRL_B0.Port->BRR = SYS_OUTCTRL_B0.Pin)

extern const PORT_INF SYS_OUTCTRL_R1;  
#define Sys_CtrlLedR1On()                   (SYS_OUTCTRL_R1.Port->BSRR = SYS_OUTCTRL_R1.Pin)  
#define Sys_CtrlLedR1Off()                  (SYS_OUTCTRL_R1.Port->BRR = SYS_OUTCTRL_R1.Pin)

extern const PORT_INF SYS_OUTCTRL_G1;
#define Sys_CtrlLedG1On()                   (SYS_OUTCTRL_G1.Port->BSRR = SYS_OUTCTRL_G1.Pin)  
#define Sys_CtrlLedG1Off()                  (SYS_OUTCTRL_G1.Port->BRR = SYS_OUTCTRL_G1.Pin)

extern const PORT_INF SYS_OUTCTRL_B1;  
#define Sys_CtrlLedB1On()                   (SYS_OUTCTRL_B1.Port->BSRR = SYS_OUTCTRL_B1.Pin)  
#define Sys_CtrlLedB1Off()                  (SYS_OUTCTRL_B1.Port->BRR = SYS_OUTCTRL_B1.Pin)

#define Sys_CtrlLed0(r, g, b)               do{\
                                                if((r)){Sys_CtrlLedR0On();}else{Sys_CtrlLedR0Off();}\
                                                if((g)){Sys_CtrlLedG0On();}else{Sys_CtrlLedG0Off();}\
                                                if((b)){Sys_CtrlLedB0On();}else{Sys_CtrlLedB0Off();}\
                                              }while(0) 
                                                     
#define Sys_CtrlLed1(r, g, b)                do{\
                                                    if((r)){Sys_CtrlLedR1On();}else{Sys_CtrlLedR1Off();}\
                                                    if((g)){Sys_CtrlLedG1On();}else{Sys_CtrlLedG1Off();}\
                                                    if((b)){Sys_CtrlLedB1On();}else{Sys_CtrlLedB1Off();}\
                                            }while(0) 

#define SYS_LED_COLOR_RED                   0x04
#define SYS_LED_COLOR_GREEN                 0x02
#define SYS_LED_COLOR_BLUE                  0x01
#define SYS_LED_COLOR_WHITE                 (SYS_LED_COLOR_RED | SYS_LED_COLOR_GREEN | SYS_LED_COLOR_BLUE)
#define SYS_LED_COLOR_PAPER                 (SYS_LED_COLOR_RED | SYS_LED_COLOR_BLUE)
#define SYS_LED_COLOR_YELLOW                (SYS_LED_COLOR_RED | SYS_LED_COLOR_GREEN)
#define SYS_LED_COLOR_CHING                 (SYS_LED_COLOR_BLUE | SYS_LED_COLOR_GREEN)
                                                     
void Sys_Delay5ms(u32 n);

void Sys_CfgClock(void);
void Sys_CfgPeriphClk(FunctionalState state);
void Sys_CfgNVIC(void);

#define Sys_SoftReset()     (*((u32 *)0xE000ED0C) = 0x05fa0004)

void Sys_CtrlIOInit(void);
void Sys_Init(void);

void Sys_UartTask(void);
void Sys_ProcessUartFrame(u8 *pBuffer, u16 len);
void Sys_LedTask(void);
void Sys_UartTask(void);
void Sys_R485Task(void);
void Sys_Tcp232Task(void);
void Sys_BootTask(void);
void Sys_ProcessBootFrame(UART_RCVFRAME *pRcvFrame, u8 com);

#define SYS_VER_HEAD                            "SM500200"
#define SYS_VER_HEAD_SIZE                       8
    


#define SYS_VERSION_SIZE                        50  
#define SYS_BOOT_VER_ADDR                       0x08005000          //版本固定在当前系统
#define SYS_APP_START_ADDR                      0x08004000
BOOL Sys_CheckVersion(void);

#define SYS_FRAME_BROADCAST_ADDR                0xFFFF

#define SYS_COM_R485        0
#define SYS_COM_UART        1
#endif
