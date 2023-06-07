#ifndef _ANYID_SM5002_SYSTICK_HL_
#define _ANYID_SM5002_SYSTICK_HL_


#include "AnyID_SM5002_Config.h"


extern vu32 g_nSysTick;
extern vs32 g_nSysDelay;

#define STICK_1MS                           120000                    //120M的系统时钟

typedef struct stickNow{
    u32 tick;           //单位5ms
    u32 count;          //定时器的count值
}STICK_NOW;
//第一步和第二步操作都是原子操作，如果第一步和第二步中间插入了sysTick中断，那么tick和g_nSysTick就不相等
#define STick_GetTick(t)                    do{(t).tick = g_nSysTick; (t).count = SysTick->VAL; if((t).tick != g_nSysTick) {(t).tick = g_nSysTick; (t).count = SysTick->VAL;} }while(0)  
//#define STick_GetDlt(d, s, e)               do{};
u32 STick_GetDlt(STICK_NOW *s, STICK_NOW *e);
void STick_InitSysTick(void);

#define STick_StartSysTick()                SysTick->CTRL  |= SysTick_CTRL_ENABLE_Msk
#define STick_StopSysTick()                 SysTick->VAL = 0;  SysTick->CTRL  &= (~SysTick_CTRL_ENABLE_Msk)

#define STick_SetDelayTime(delay)           (g_nSysDelay = delay) //以一个Tick为单位
#define STick_IsDelayTimeOver()             (g_nSysDelay <= 0)



#define STICK_TIME_MS                       5

#define STICK_1S                            (1000 / STICK_TIME_MS)

#define STICK_5MS_COUNT                     (STICK_1MS * STICK_TIME_MS) 

#endif

