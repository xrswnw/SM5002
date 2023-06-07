#ifndef _ANYID_SM5002_SYSCFG_
#define _ANYID_SM5002_SYSCFG_


#include "AnyID_SM5002_Config.h"

extern u32 g_nAlarmDelay;

extern u32 g_nSysState;
#define SYS_STAT_IDLE           0x00000001	    //系统初始化后处于空闲状�?
#define SYS_STAT_RUNLED         0x00000002
#define SYS_STAT_AD             0x00000004
#define SYS_STAT_RESET          0x00000008
#define SYS_STAT_IN_SENSOR      0x00000010


extern const PORT_INF SYS_RUNLED_COM;
#define Sys_RunLedOn()          //SYS_RUNLED_COM.Port->BRR = SYS_RUNLED_COM.Pin
#define Sys_RunLedOff()         //SYS_RUNLED_COM.Port->BSRR = SYS_RUNLED_COM.Pin

#define Sys_AlarmLedOff()           
#define Sys_AlarmLedOn()            
    
void Sys_CfgClock(void);
void Sys_CfgPeriphClk(FunctionalState state);
void Sys_CfgNVIC(void);


void Sys_Init(void);

void Sys_LedTask(void);
void Sys_AdTask(void);
void Sys_SensorTask(void);

void Sys_R485Task(void);
void Sys_ChargeTask(void);

void Sys_DeviceTask(void);

#endif
