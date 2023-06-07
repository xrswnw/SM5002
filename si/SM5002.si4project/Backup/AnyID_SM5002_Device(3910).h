#ifndef _ANYID_SM5002_DEVICE_
#define _ANYID_SM5002_DEVICE_

#include "AnyID_SM5002_Config.h"
#include "AnyID_SM5002_FRam.h"
#include "AnyID_SM5002_SysTick_HL.h"
#include "AnyID_SM5002_WDG_HL.h"
#include "AnyID_SM5002_AD.h"
#include "AnyID_Uart_Receive.h"

extern const PORT_INF DEV_ADDR_CLK;
#define Device_AddrClkLow()                     (DEV_ADDR_CLK.Port->BRR = DEV_ADDR_CLK.Pin)  
#define Device_AddrClkHigh()                    (DEV_ADDR_CLK.Port->BSRR = DEV_ADDR_CLK.Pin)

extern const PORT_INF DEV_ADDR_LOAD;
#define Device_AddrLoadLow()                    (DEV_ADDR_LOAD.Port->BRR = DEV_ADDR_LOAD.Pin)  
#define Device_AddrLoadHigh()                   (DEV_ADDR_LOAD.Port->BSRR = DEV_ADDR_LOAD.Pin)

extern const PORT_INF DEV_ADDR_OUT;
#define Device_GetAddrOut()                   (DEV_ADDR_OUT.Port->IDR & DEV_ADDR_OUT.Pin)


#define READER_VERSION_SIZE             50
extern const u8 READER_VERSION[READER_VERSION_SIZE];

#define READER_APP_OK                   0x5555
#define READER_APP_FAIL                 0xAAAA

typedef struct deviceSensorInfo{
    u32 tmpr: 8;                //温度
    u32 fanCtrl: 1;             // 1-开启      0-关闭
    u32 doorCtrl: 1;            // 1-开门      0-保持
    u32 doorState: 1;           // 1-开门      0-关门
    u32 doorRfid: 1;             // 1-开门      0-关门
    u32 smokeState: 1;
    u32 ledRed: 1;
    u32 ledGreen: 1;
    u32 ledBlue: 1;
    u32 rfu: 16;
}DEVICE_SENSORINFO;

#define DEVICE_SENSOR_NUM       2
typedef struct deviceOpInfo{
    WINAVG_INFO tmpr[DEVICE_SENSOR_NUM];           //温度信息
    DEVICE_SENSORINFO sensor[DEVICE_SENSOR_NUM];   
}DEVICE_OPINFO;
extern DEVICE_OPINFO g_sDeviceOpInfo;

void Reader_Delayms(u32 n);

void Reader_ReadDeviceParamenter(void);
BOOL Reader_WriteDeviceParamenter(void);

#endif
