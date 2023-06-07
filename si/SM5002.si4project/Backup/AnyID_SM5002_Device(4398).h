#ifndef _ANYID_SM5002_DEVICE_
#define _ANYID_SM5002_DEVICE_

#include "AnyID_SM5002_Config.h"
#include "AnyID_SM5002_FRam.h"
#include "AnyID_SM5002_SysTick_HL.h"
#include "AnyID_SM5002_WDG_HL.h"
#include "AnyID_SM5002_AD.h"
#include "AnyID_SM5002_R485.h"
#include "AnyID_SM5002_Charge.h"


extern const PORT_INF DEV_ADDR_CLK;
#define Device_AddrClkLow()                     (DEV_ADDR_CLK.Port->BRR = DEV_ADDR_CLK.Pin)  
#define Device_AddrClkHigh()                    (DEV_ADDR_CLK.Port->BSRR = DEV_ADDR_CLK.Pin)

extern const PORT_INF DEV_ADDR_LOAD;
#define Device_AddrLoadLow()                    (DEV_ADDR_LOAD.Port->BRR = DEV_ADDR_LOAD.Pin)  
#define Device_AddrLoadHigh()                   (DEV_ADDR_LOAD.Port->BSRR = DEV_ADDR_LOAD.Pin)

extern const PORT_INF DEV_ADDR_OUT;
#define Device_GetAddrOut()                     (DEV_ADDR_OUT.Port->IDR & DEV_ADDR_OUT.Pin)


extern const PORT_INF DEV_OUTCTRL_CLK;
#define Device_OutCtrlClkLow()                  (DEV_OUTCTRL_CLK.Port->BRR = DEV_OUTCTRL_CLK.Pin)  
#define Device_OutCtrlClkHigh()                 (DEV_OUTCTRL_CLK.Port->BSRR = DEV_OUTCTRL_CLK.Pin)

extern const PORT_INF DEV_OUTCTRL_EN0;
#define Device_OutCtrlEn0Low()                  (DEV_OUTCTRL_EN0.Port->BRR = DEV_OUTCTRL_EN0.Pin)  
#define Device_OutCtrlEn0High()                 (DEV_OUTCTRL_EN0.Port->BSRR = DEV_OUTCTRL_EN0.Pin)

extern const PORT_INF DEV_OUTCTRL_EN1;
#define Device_OutCtrlEn1Low()                  (DEV_OUTCTRL_EN1.Port->BRR = DEV_OUTCTRL_EN1.Pin)  
#define Device_OutCtrlEn1High()                 (DEV_OUTCTRL_EN1.Port->BSRR = DEV_OUTCTRL_EN1.Pin)

extern const PORT_INF DEV_OUTCTRL_DAT;
#define Device_OutCtrlDatLow()                  (DEV_OUTCTRL_DAT.Port->BRR = DEV_OUTCTRL_DAT.Pin)  
#define Device_OutCtrlDatHigh()                 (DEV_OUTCTRL_DAT.Port->BSRR = DEV_OUTCTRL_DAT.Pin)


extern const PORT_INF DEV_INSEN_CLK;
#define Device_InSensorClkLow()                 (DEV_INSEN_CLK.Port->BRR = DEV_INSEN_CLK.Pin)  
#define Device_InSensorClkHigh()                (DEV_INSEN_CLK.Port->BSRR = DEV_INSEN_CLK.Pin)

extern const PORT_INF DEV_INSEN_LOAD;
#define Device_InSensorLoadLow()                (DEV_INSEN_LOAD.Port->BRR = DEV_INSEN_LOAD.Pin)  
#define Device_InSensorLoadHigh()               (DEV_INSEN_LOAD.Port->BSRR = DEV_INSEN_LOAD.Pin)

extern const PORT_INF DEV_INSEN_DAT;
#define Device_GetInSensor()                     (DEV_INSEN_DAT.Port->IDR & DEV_INSEN_DAT.Pin)

extern const PORT_INF DEV_INSEN_RFID0;
#define Device_GetRfid0()                       (DEV_INSEN_RFID0.Port->IDR & DEV_INSEN_RFID0.Pin)

extern const PORT_INF DEV_INSEN_RFID1;
#define Device_GetRfid1()                       (DEV_INSEN_RFID1.Port->IDR & DEV_INSEN_RFID1.Pin)

#define DEVICE_VERSION_SIZE             50
extern const u8 DEVICE_VERSION[DEVICE_VERSION_SIZE];

#define DEVICE_APP_OK                   0x5555
#define DEVICE_APP_FAIL                 0xAAAA

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

typedef struct deviceOutCtrl{
    u16 rfu: 4;
    u16 batLock1: 1;
    u16 batLock0: 1;
    u16 doorLock1: 1;
    u16 doorLock0: 1;
    u16 ledRed0: 1;
    u16 ledGreen0: 1;
    u16 ledBlue0: 1;
    u16 ledRed1: 1;
    u16 ledGreen1: 1;
    u16 ledBlue1: 1;
    u16 fanCtrl1: 1;
    u16 fanCtrl0: 1;
}DEVICE_OUTCTRL;

#define DEVICE_DOOR_OPEN_VALUE          0       //打开状态是0  关闭状态是1
#define DEVICE_SMOKE_ALARM_VALUE        1       //正常状态是0  报警状态是1
typedef struct deviceInInfo{
    u16 doorState0: 1;
    u16 doorState1: 1;
    u16 fireFB0: 1;
    u16 fireFB1: 1;
    u16 smoke0: 1;
    u16 smoke1: 1;
    u16 waterLv0: 1;
    u16 waterLv1: 1;
    u16 doorRfid0: 1;
    u16 doorRfid1: 1;
    u16 rfu: 6;
}DEVICE_ININFO;


#define DEVICE_SENSOR_NUM       2
typedef struct deviceOpInfo{
    u8 addr;
    WINAVG_INFO tmpr[DEVICE_SENSOR_NUM];           //温度信息
    DEVICE_SENSORINFO sensor[DEVICE_SENSOR_NUM];
    DEVICE_OUTCTRL outCtrl;
    DEVICE_ININFO inInfo;
}DEVICE_OPINFO;
extern DEVICE_OPINFO g_sDeviceOpInfo;

void Device_Delayms(u32 n);

void Device_ReadDeviceParamenter(void);
BOOL Device_WriteDeviceParamenter(void);

void Device_InitAddr(void);
u8 Device_ReadAddr(void);
void Device_InitOutCtrl(void);
void Device_OutCtrl(DEVICE_OUTCTRL *pOutCtrl);

void Device_InitInSensor(void);
void Device_ReadInInfo(DEVICE_ININFO *pInInfo);

u16 Device_ProcessUsrFrame(u8 *pFrame);


#endif
