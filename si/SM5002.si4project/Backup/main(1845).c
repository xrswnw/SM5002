#include "AnyID_SM5002_SysCfg.h"

int main(void)
{
    Sys_Init();   
    while(1)
    {
        Sys_LedTask();    
        Sys_AdTask();
        Sys_R485Task();
        Sys_DeviceTask();    
    }
}
