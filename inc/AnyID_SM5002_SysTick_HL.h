#ifndef _ANYID_SM5002_SYSTICK_HL_
#define _ANYID_SM5002_SYSTICK_HL_


#include "AnyID_SM5002_Config.h"


extern vu32 g_nSysTick;
extern vs32 g_nSysDelay;

#define STICK_1MS                           120000                    //120M��ϵͳʱ��

typedef struct stickNow{
    u32 tick;           //��λ5ms
    u32 count;          //��ʱ����countֵ
}STICK_NOW;
//��һ���͵ڶ�����������ԭ�Ӳ����������һ���͵ڶ����м������sysTick�жϣ���ôtick��g_nSysTick�Ͳ����
#define STick_GetTick(t)                    do{(t).tick = g_nSysTick; (t).count = SysTick->VAL; if((t).tick != g_nSysTick) {(t).tick = g_nSysTick; (t).count = SysTick->VAL;} }while(0)  
//#define STick_GetDlt(d, s, e)               do{};
u32 STick_GetDlt(STICK_NOW *s, STICK_NOW *e);
void STick_InitSysTick(void);

#define STick_StartSysTick()                SysTick->CTRL  |= SysTick_CTRL_ENABLE_Msk
#define STick_StopSysTick()                 SysTick->VAL = 0;  SysTick->CTRL  &= (~SysTick_CTRL_ENABLE_Msk)

#define STick_SetDelayTime(delay)           (g_nSysDelay = delay) //��һ��TickΪ��λ
#define STick_IsDelayTimeOver()             (g_nSysDelay <= 0)



#define STICK_TIME_MS                       5

#define STICK_1S                            (1000 / STICK_TIME_MS)

#define STICK_5MS_COUNT                     (STICK_1MS * STICK_TIME_MS) 

#endif

