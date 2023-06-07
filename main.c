#include "AnyID_SM5002_SysCfg.h"

int main(void)
{
    Sys_Init();   
    while(1)
    {
        Sys_LedTask(); 
        Sys_R485Task();
        
        Sys_AdTask(DEVICE_GATE_INDEX0);
        Sys_SensorTask(DEVICE_GATE_INDEX0);
        Sys_BatTask(DEVICE_GATE_INDEX0);
        Sys_ChargeTask(DEVICE_GATE_INDEX0);
        Sys_GateTask(DEVICE_GATE_INDEX0);
        
        Sys_AdTask(DEVICE_GATE_INDEX1);
        Sys_SensorTask(DEVICE_GATE_INDEX1);
        Sys_BatTask(DEVICE_GATE_INDEX1);
        Sys_ChargeTask(DEVICE_GATE_INDEX1);
        Sys_GateTask(DEVICE_GATE_INDEX1);
    }
}
