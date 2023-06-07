#include "AnyID_SM5002_SysTick_HL.h"

vu32 g_nSysTick = 0;
vs32 g_nSysDelay = 0;

void STick_InitSysTick(void)
{
    //Configure HCLK clock as SysTick clock source  48M
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    //SysTick interrupt each 1000 Hz with HCLK equal
    SysTick_Config(STICK_1MS * STICK_TIME_MS);

    g_nSysTick = 0;
    g_nSysDelay = 0;

    NVIC_SetPriority(SysTick_IRQn, INT_PRIORITY_STICK);     //优先级最低
}

u32 STick_GetDlt(STICK_NOW *s, STICK_NOW *e)
{
    u32 t = 0, t1 = 0, t2 = 0;
    
    t1 = (STICK_5MS_COUNT - s->count);
    t2 = (STICK_5MS_COUNT - e->count);
    
    t = e->tick - s->tick;
    if(t2 >= t1)
    {
        t2 = t2 - t1;
    }
    else 
    {
        t -= 1;
        t2 = STICK_5MS_COUNT + t2 - t1;
    }
    //四舍五入
    t2 = t2 * STICK_TIME_MS * 10 / STICK_5MS_COUNT;
    if((t2 % 10) > 5)
    {
        t2 += 10;
    }
    t2 /= 10;
    //换算成ms
    t *= STICK_TIME_MS;
    t += t2;
    
    return t;
}



