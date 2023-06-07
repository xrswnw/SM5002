#ifndef _ANYID_SM5002_DEVICE_
#define _ANYID_SM5002_DEVICE_

#include "AnyID_SM5002_Config.h"
#include "AnyID_SM5002_FRam.h"
#include "AnyID_SM5002_SysTick_HL.h"
#include "AnyID_SM5002_WDG_HL.h"
#include "AnyID_SM5002_AD.h"
#include "AnyID_SM5002_R485.h"
#include "AnyID_SM5002_Charge.h"
#include "AnyID_SM5002_Bat.h"
#include "AnyID_SM5002_Bat.h"
 
extern const PORT_INF DEV_ADDR_OUT;
#define Device_GetAddrOut()                     ((DEV_ADDR_PIN.Port->IDR & DEV_ADDR_PIN.Pin) >> 1)

extern const PORT_INF DEV_OUTCTRL_LOCK0; 
#define Device_CtrlLock0Open()                  (DEV_OUTCTRL_LOCK0.Port->BSRR = DEV_OUTCTRL_LOCK0.Pin)  
#define Device_CtrlLock0Close()                 (DEV_OUTCTRL_LOCK0.Port->BRR = DEV_OUTCTRL_LOCK0.Pin)
#define Device_CtrlLock0(b)                     do{if((b)){Device_CtrlLock0Open();}else{Device_CtrlLock0Close();}}while(0)

extern const PORT_INF DEV_OUTCTRL_LOCK1;
#define Device_CtrlLock1Open()                  (DEV_OUTCTRL_LOCK1.Port->BSRR = DEV_OUTCTRL_LOCK1.Pin)  
#define Device_CtrlLock1Close()                 (DEV_OUTCTRL_LOCK1.Port->BRR = DEV_OUTCTRL_LOCK1.Pin)
#define Device_CtrlLock1(b)                     do{if((b)){Device_CtrlLock1Open();}else{Device_CtrlLock1Close();}}while(0)

extern const PORT_INF DEV_OUTCTRL_FAN0;   
#define Device_CtrlFan0Open()                   (DEV_OUTCTRL_FAN0.Port->BSRR = DEV_OUTCTRL_FAN0.Pin)  
#define Device_CtrlFan0Close()                  (DEV_OUTCTRL_FAN0.Port->BRR = DEV_OUTCTRL_FAN0.Pin)
#define Device_CtrlFan0(b)                      do{if((b)){Device_CtrlFan0Open();}else{Device_CtrlFan0Close();}}while(0)

extern const PORT_INF DEV_OUTCTRL_FAN1;
#define Device_CtrlFan1Open()                   (DEV_OUTCTRL_FAN1.Port->BSRR = DEV_OUTCTRL_FAN1.Pin)  
#define Device_CtrlFan1Close()                  (DEV_OUTCTRL_FAN1.Port->BRR = DEV_OUTCTRL_FAN1.Pin)
#define Device_CtrlFan1(b)                      do{if((b)){Device_CtrlFan1Open();}else{Device_CtrlFan1Close();}}while(0)

extern const PORT_INF DEV_OUTCTRL_R0;  
#define Device_CtrlLedR0On()                    (DEV_OUTCTRL_R0.Port->BSRR = DEV_OUTCTRL_R0.Pin)  
#define Device_CtrlLedR0Off()                   (DEV_OUTCTRL_R0.Port->BRR = DEV_OUTCTRL_R0.Pin)

extern const PORT_INF DEV_OUTCTRL_G0;
#define Device_CtrlLedG0On()                    (DEV_OUTCTRL_G0.Port->BSRR = DEV_OUTCTRL_G0.Pin)  
#define Device_CtrlLedG0Off()                   (DEV_OUTCTRL_G0.Port->BRR = DEV_OUTCTRL_G0.Pin)

extern const PORT_INF DEV_OUTCTRL_B0;
#define Device_CtrlLedB0On()                    (DEV_OUTCTRL_B0.Port->BSRR = DEV_OUTCTRL_B0.Pin)  
#define Device_CtrlLedB0Off()                   (DEV_OUTCTRL_B0.Port->BRR = DEV_OUTCTRL_B0.Pin)

extern const PORT_INF DEV_OUTCTRL_R1;  
#define Device_CtrlLedR1On()                    (DEV_OUTCTRL_R1.Port->BSRR = DEV_OUTCTRL_R1.Pin)  
#define Device_CtrlLedR1Off()                   (DEV_OUTCTRL_R1.Port->BRR = DEV_OUTCTRL_R1.Pin)

extern const PORT_INF DEV_OUTCTRL_G1;
#define Device_CtrlLedG1On()                    (DEV_OUTCTRL_G1.Port->BSRR = DEV_OUTCTRL_G1.Pin)  
#define Device_CtrlLedG1Off()                   (DEV_OUTCTRL_G1.Port->BRR = DEV_OUTCTRL_G1.Pin)

extern const PORT_INF DEV_OUTCTRL_B1;  
#define Device_CtrlLedB1On()                    (DEV_OUTCTRL_B1.Port->BSRR = DEV_OUTCTRL_B1.Pin)  
#define Device_CtrlLedB1Off()                   (DEV_OUTCTRL_B1.Port->BRR = DEV_OUTCTRL_B1.Pin)

#define Device_CtrlLed0(r, g, b)                do{\
                                                    if((r)){Device_CtrlLedR0On();}else{Device_CtrlLedR0Off();}\
                                                    if((g)){Device_CtrlLedG0On();}else{Device_CtrlLedG0Off();}\
                                                    if((b)){Device_CtrlLedB0On();}else{Device_CtrlLedB0Off();}\
                                                 }while(0) 
                                                     
#define Device_CtrlLed1(r, g, b)                do{\
                                                    if((r)){Device_CtrlLedR1On();}else{Device_CtrlLedR1Off();}\
                                                    if((g)){Device_CtrlLedG1On();}else{Device_CtrlLedG1Off();}\
                                                    if((b)){Device_CtrlLedB1On();}else{Device_CtrlLedB1Off();}\
                                                 }while(0) 

extern const PORT_INF DEV_INSEN_LOCK_FB0; 
#define Device_GetLockFB0()                     (DEV_INSEN_LOCK_FB0.Port->IDR & DEV_INSEN_LOCK_FB0.Pin)
extern const PORT_INF DEV_INSEN_LOCK_FB1;
#define Device_GetLockFB1()                     (DEV_INSEN_LOCK_FB1.Port->IDR & DEV_INSEN_LOCK_FB1.Pin)

extern const PORT_INF DEV_INSEN_SMK_FB0;   
#define Device_GetSmokeFB0()                    (DEV_INSEN_SMK_FB0.Port->IDR & DEV_INSEN_SMK_FB0.Pin)
extern const PORT_INF DEV_INSEN_SMK_FB1; 
#define Device_GetSmokeFB1()                    (DEV_INSEN_SMK_FB1.Port->IDR & DEV_INSEN_SMK_FB1.Pin)

extern const PORT_INF DEV_INSEN_LOCK_RFID0; 
#define Device_GetRfidFB0()                     (DEV_INSEN_LOCK_RFID0.Port->IDR & DEV_INSEN_LOCK_RFID0.Pin)
extern const PORT_INF DEV_INSEN_LOCK_RFID1;
#define Device_GetRfidFB1()                     (DEV_INSEN_LOCK_RFID1.Port->IDR & DEV_INSEN_LOCK_RFID1.Pin)


#define Device_CtrlLock0(b)                     do{if((b)){Device_CtrlLock0Open();}else{Device_CtrlLock0Close();}}while(0)
#define Device_ChkLock0Open()                   (DEV_OUTCTRL_LOCK0.Port->ODR & DEV_OUTCTRL_LOCK0.Pin)

#define Device_CtrlLock1(b)                     do{if((b)){Device_CtrlLock1Open();}else{Device_CtrlLock1Close();}}while(0)
#define Device_ChkLock1Open()                   (DEV_OUTCTRL_LOCK1.Port->ODR & DEV_OUTCTRL_LOCK1.Pin)




#define DEVICE_VERSION_SIZE             50
extern const u8 DEVICE_VERSION[DEVICE_VERSION_SIZE];

#define DEVICE_APP_OK                   0x5555
#define DEVICE_APP_FAIL                 0xAAAA

#define DEVICE_TMPR_ALARM_DFT           30
#define DEVICE_LED_LOWVOL_DFT           80

#define DEVICE_GATE_NUM                 2
#define DEVICE_GATE_INDEX0              0
#define DEVICE_GATE_INDEX1              1
#define DEVICE_GATE_ALL                 DEVICE_GATE_NUM

#define DEVICE_GATE_PARAMS_LEN          24


typedef struct deviceParams{
    u8 planeBat[DEVICE_GATE_NUM];       //预约电池

    u16 rfu0;                     
    u16 ledLowVolLev;                   //低电压，这是状态指示用的
    u16 alarmTmpr;                      //报警温度阈值
    CHAG_PARAMS chagParams;             //充电机工作参数
    u32 crc;
}DEVICE_PARAMS;
extern DEVICE_PARAMS g_sDeviceParams;

#define DEVICE_DOOR_OPEN                1
#define DEVICE_DOOR_CLOSE               0
#define DEVICE_RFID_OPEN                1
#define DEVICE_RFID_CLOSE               0
#define DEVICE_SMOKE_ALARM              1
#define DEVICE_SMOKE_NORM               0
typedef struct deviceStateInfo{
    u8 door: 1;     // 1-开门      0-保持
    u8 rfid: 1;     // 1-开门      0-保持
    u8 smoke: 1;    // 1-报警      0-正常
    u8 fan: 1;      // 1-开启      0-关闭
    u8 rfu: 4;
}DEVICE_STATINFO;

typedef struct deviceSensorInfo{
    u16 flag;
    u8 addr;
    u8 tmpr;            //温度
    DEVICE_STATINFO sensorState;
    BAT_INFO batInfo;
    CHAG_INFO chagInfo;
}DEVICE_SENSORINFO;

#define DEVICE_INFO_BUF_NUM             64
#define DEVICE_INFO_RCD_SIZE            256
#define DEVICE_INFO_IDLE_TICK_CHAG      (200 * 10) //(200 * 60)          //充电状态，1分钟一条记录
#define DEVICE_INFO_IDLE_TICK_NORMAL    (200 * 10) //(200 * 60 * 10)     //正常状态，10分钟一条记录
typedef struct deviceInfoBuf{
    u8 e;
    u8 s;
    DEVICE_SENSORINFO buf[DEVICE_INFO_BUF_NUM];
    DEVICE_SENSORINFO tmp;
}DEVICE_INFOBUF;
extern DEVICE_INFOBUF g_sDeviceInfoBuf;
#define Device_ClearRcdBuf()                    do{g_sDeviceInfoBuf.e = 0; g_sDeviceInfoBuf.s = 0;}while(0)

#define DEVICE_OUT_CTRL_POS_FAN         0x01
#define DEVICE_OUT_CTRL_POS_DOOR        0x02
typedef struct deviceOutCtrl{
    u16 rfu: 4;
    u16 batLock1: 1;
    u16 batLock0: 1;
    u16 doorLock0: 1;                   //测试模式------------------------------------------------------交换了位置，后续改硬件
    u16 doorLock1: 1;
    u16 ledRed0: 1;
    u16 ledGreen0: 1;
    u16 ledBlue0: 1;
    u16 ledRed1: 1;
    u16 ledGreen1: 1;
    u16 ledBlue1: 1;
    u16 fanCtrl1: 1;
    u16 fanCtrl0: 1;
}DEVICE_OUTCTRL;
#define DEVICE_DOOR_CLOSE_VALUE         1       //打开状态是0  关闭状态是1
#define DEVICE_RFID_CLOSE_VALUE         1

#define DEVICE_IN_SESNOR_NUM            3
#define DEVICE_IN_CHECK_TIM             10
#define DEVICE_IN_CHK_FLAG_DOOR         0x01
typedef struct deviceCheckInInfo{
    u8 checkTick[DEVICE_IN_SESNOR_NUM];
    DEVICE_STATINFO curInState;
    u16 inValue;
    u16 changeFlag;
}DEVICE_INCHECK;
extern DEVICE_INCHECK g_sDeviceInCheck[DEVICE_GATE_NUM];

#define DEVICE_CTRL_DOOR_TICK           80
typedef struct deviceCtrlDoor{
    BOOL bOpen;
    u32 tick;
}DEVICE_CTRLDOOR;

#define DEVICE_GATE_MODE_STARTUP        0   //开机模式：上电检测，异常()：报警
#define DEVICE_GATE_MODE_CHARGE         1   //充电模式
#define DEVICE_GATE_MODE_IDLE           2
#define DEVICE_GATE_MODE_RTNBAT         3   //归还电池
#define DEVICE_GATE_MODE_BRWBAT         4   //借出电池

#define DEVICE_IVT_TIME_FAST            50
#define DEVICE_IVT_TIME_SLOW            (20 * 200)//200
#define DEVICE_IVT_TIME_MID             400

#define DEVICE_OPENDOR_TIM              200    //开门需要500ms

#define DEVICE_RTNBAT_STEP_OVER         0x00
#define DEVICE_RTNBAT_STEP_OPENDOR      0x01
#define DEVICE_RTNBAT_STEP_CHKBAT       0x02

#define DEVICE_RTNBAT_ERR_OK            0
#define DEVICE_RTNBAT_ERR_OPENDOR       1
#define DEVICE_RTNBAT_ERR_BATNO         2
#define DEVICE_RTNBAT_ERR_PARAMS        3
//#define DEVICE_RTNBAT_ERR_BATSN         4

#define DEVICE_RTN_CHKBAT_ERR_BATEXIT   1   //电池插入
#define DEVICE_RTN_CHKBAT_ERR_BATNO     2   //无电池
#define DEVICE_RTN_CHKBAT_ERR_BATSN     4   //非法电池

#define DEVICE_RTNBAT_ERR_WAIT          0xFF



#define DEVICE_BRWBAT_STEP_OVER         0x00
#define DEVICE_BRWBAT_STEP_OPENDOR      0x01
#define DEVICE_BRWBAT_STEP_CHKBAT       0x02

#define DEVICE_PLANE_BAT_OK             0x00
#define DEVICE_PLANE_BAT_FAIL           0x03

#define DEVICE_BRWBAT_ERR_OK            0
#define DEVICE_BRWBAT_ERR_OPENDOR       1
#define DEVICE_BRWBAT_ERR_BATEXIST      2
#define DEVICE_BRWBAT_ERR_PARAMS        3

#define DEVICE_BRW_CHKBAT_ERR_BATNO     1   //电池取走
#define DEVICE_BRW_CHKBAT_ERR_BATSN     2   //非法电池

#define DEVICE_BRWBAT_ERR_WAIT          0xFF

#define DEVICE_RCD_FLAG_SENSOR          0x01
#define DEVICE_RTN_BAT_CHK_RPT          3
typedef struct deviceGateInfo{
    u8 mode;
    u8 rtnBatStep;                      //还电池步骤
    u32 rtnTick;
    u8 rtnOpenDoorRpt;                  //不能识别电池，非法次数

    u8 brwBatStep;                      //借电池步骤
    u32 brwTick;
    
    WINAVG_INFO tmpr;                   //温度信息
    DEVICE_CTRLDOOR ctrlDoor;           //开门控制逻辑
    DEVICE_STATINFO sensorState;        //传感器信息
    CHAG_OPINFO *pChagOpInfo;           //充电机操作信息
    BAT_OPINFO *pBatOpInfo;             //电池操作信息
    u16 interVal;                       //间隔时间
    u8 ledColor;                        //LED灯颜色
    u32 tick;
    u32 rcdIdleTick;                    //空闲记录时间，长时间没有状态变化得记录，系统也需要产生一条记录
    u32 rcdIdleDelay;
    u8 rcdFlag;                         //记录标志1-需要打包记录
}DEVICE_GATEINFO;
extern DEVICE_GATEINFO g_sDeviceGateInfo[DEVICE_GATE_NUM];

typedef struct deviceOpInfo{
    u8 addr;
    DEVICE_OUTCTRL outCtrl;
    DEVICE_OUTCTRL usrCtrl;
    u8 rtnBatSn[BAT_SN_LEN];
    u8 brwBatSn[BAT_SN_LEN];
}DEVICE_OPINFO;
extern DEVICE_OPINFO g_sDeviceOpInfo;

#define DEVICE_LED_COLOR_RED            0x04
#define DEVICE_LED_COLOR_GREEN          0x02
#define DEVICE_LED_COLOR_BLUE           0x01
#define DEVICE_LED_COLOR_WHITE          (DEVICE_LED_COLOR_RED | DEVICE_LED_COLOR_GREEN | DEVICE_LED_COLOR_BLUE)
#define DEVICE_LED_COLOR_PAPER          (DEVICE_LED_COLOR_RED | DEVICE_LED_COLOR_BLUE)
#define DEVICE_LED_COLOR_YELLOW         (DEVICE_LED_COLOR_RED | DEVICE_LED_COLOR_GREEN)
#define DEVICE_LED_COLOR_CHING          (DEVICE_LED_COLOR_BLUE | DEVICE_LED_COLOR_GREEN)

void Device_Delayms(u32 n);
void Device_Init(u8 index);
void Device_ReadDeviceParamenter(void);

BOOL Device_CheckLockOpen(u8 index);
BOOL Device_WriteDeviceParamenter(void);

void Device_InitAddr(void);
u8 Device_ReadAddr(void);
void Device_InitOutCtrl(void);
void Device_CtrlDoor(BOOL bOpen, u8 index);
void Device_CtrlFan(BOOL bOpen, u8 index);
void Device_CtrlLed(u8 index, u8 color);
u8 Device_CheckUsrFan(DEVICE_OUTCTRL *pOutCtrl, u8 index);

void Device_InitInSensor(void);
void Device_ReadInInfo(DEVICE_STATINFO *pStateInfo, u8 index);
u32 Device_CheckInSensor(DEVICE_INCHECK *pInCheck, u8 *pUsrState);

void Device_FomatGateRecord(u8 index, DEVICE_GATEINFO *pGateInfo, DEVICE_INFOBUF *pRcdBuf);
void Device_AddRecordInfo(DEVICE_INFOBUF *pRcdBuf, DEVICE_SENSORINFO *pSensorInfo);

u16 Device_GetInInfoRspFrame(DEVICE_INFOBUF *pInfoBuf, u16 addr, u8 *pBuffer);
u16 Device_ProcessUsrFrame(u8 *pFrame);

void Device_StartCharge(DEVICE_GATEINFO *pGateInfo, u8 index);
void Device_Charge(BAT_OPINFO *pBatOpInfo, CHAG_OPINFO *pChagOpInfo, u8 index);
void Device_StopCharge(DEVICE_GATEINFO *pGateInfo, u8 index);
void Device_CheckCharge(BAT_OPINFO *pBatOpInfo, CHAG_OPINFO *pChagOpInfo, u8 index);



void Device_OpBatRspFrame(u8 cmd, u8 step, u8 err);
void Device_RtnWaitBat(BAT_OPINFO *pBatOpInfo, CHAG_OPINFO *pChagOpInfo, u8 index);
void Device_BrwWaitBat(BAT_OPINFO *pBatOpInfo, CHAG_OPINFO *pChagOpInfo, u8 index);


void Device_GateRefreshLed(DEVICE_GATEINFO *pGateInfo, u8 index);
void Device_GateStartup(DEVICE_GATEINFO *pGateInfo, u8 index);
void Device_GateCharge(DEVICE_GATEINFO *pGateInfo, u8 index);
void Device_GateReturnBat(DEVICE_GATEINFO *pGateInfo, u8 index);
void Device_GateBorrowBat(DEVICE_GATEINFO *pGateInfo, u8 index);

#endif
