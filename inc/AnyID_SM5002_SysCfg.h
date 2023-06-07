#ifndef _ANYID_SM5002_SYSCFG_
#define _ANYID_SM5002_SYSCFG_

#include "AnyID_SM5002_Config.h"
#include "AnyID_SM5002_Device.h"

extern u32 g_nAlarmDelay;

extern u32 g_nSysState;
#define SYS_STAT_IDLE           0x00000001	    //系统初始化后处于空闲状�?
#define SYS_STAT_RUNLED         0x00000002
#define SYS_STAT_RESET          0x00000008
#define SYS_STAT_IN_SENSOR0     0x00000010
#define SYS_STAT_IN_SENSOR1     0x00000020
#define SYS_STAT_AD0            0x00000040
#define SYS_STAT_AD1            0x00000080
#define SYS_STAT_AD             (SYS_STAT_AD0 | SYS_STAT_AD1)
#define SYS_STAT_IN_SENSOR      (SYS_STAT_IN_SENSOR0 | SYS_STAT_IN_SENSOR1)

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


void Sys_R485Task(void);
void Sys_ChargeTask(u8 index);
void Sys_AdTask(u8 index);
void Sys_SensorTask(u8 index);
void Sys_BatTask(u8 index);
void Sys_GateTask(u8 index);

#endif
