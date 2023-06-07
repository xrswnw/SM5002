#ifndef _ANYID_SM5002_BAT_
#define _ANYID_SM5002_BAT_

#include "AnyID_SM5002_Bat_HL.h"
//#include "AnyID_Uart_Receive.h"

#define BAT_TX_FRAME_LEN        16
typedef struct batTxFrame{
    u8 buffer[BAT_TX_FRAME_LEN];
    u16 len;
}BAT_TXFRAME;

#define BAT_RX_FRAME_LEN        64
typedef struct batRxFrame{
    u8 buffer[BAT_RX_FRAME_LEN];
    u16 len;
    u8 state;
}BAT_RXFRAME;


extern BAT_RXFRAME g_sBatRcvFrame;
extern BAT_TXFRAME g_sBatTxFrame;

#define BAT_CMD_GET_VER                 0x01
#define BAT_CMD_GET_SN                  0x02
#define BAT_CMD_GET_STATUS              0x03
#define BAT_CMD_CHAG_HEART              0x04
#define BAT_CMD_GET_ERR                 0x05

typedef struct batStatusInfo{
    u16 volValue;       //电池电压，单位0.1V
    u8 volLev;          //电量，单位1%
    u16 cycleCount;     //循环次数
    u8 chargeStatus;    //充电状态 1-空闲 1-放电 2-充电
    u16 maxChagVol;     //满充电压，单位0.1V
    u16 maxChagCur;     //最大充电电流，单位0.1A
    u16 remainCap;      //剩余容量，单位0.01Ah
    u16 curValue;       //充放电电流，单位0.1A
    u8 tmpr;            //温度
    u8 dltVol;          //单体最大压差，单位1mV
}BAT_STATUSINFO;

typedef struct batErrInfo{
    u8 fullState;
    u8 protectStatus1;
    u8 alarmStatus1;
    u8 alarmStatus2;
    u8 diagStatus;
}BAT_ERRINFO;

#define BAT_SN_LEN                      6
typedef struct batInfo{
    //BOOL bExist;
    u16 verHard;
    u16 verSoft;
    u8 sn[BAT_SN_LEN];
    BAT_STATUSINFO status;
    BAT_ERRINFO err;
}BAT_INFO;

#define BAT_CHK_TIME                    20
#define BAT_OP_NUM                      5
#define BAT_OP_TO                       10
#define BAT_OP_STAT_IDLE                0x00
#define BAT_OP_STAT_TX                  0x01
#define BAT_OP_STAT_WAIT                0x02
#define BAT_OP_STAT_RX                  0x03
#define BAT_OP_STAT_NEXT                0x80
#define BAT_OP_STAT_TO                  0x05

#define BAT_OP_RPT_TIM                  3

#define BAT_MODE_GET_INFO               0   //获取电池信息
#define BAT_MODE_CHARGE                 1   //充电模式
#define BAT_MODE_CHK_STATUS             2   //校验电池状态
#define BAT_MODE_IDLE                   3   //无电池，不需要任何操作
typedef struct batOpInfo{
    u8 mode;
    u8 state;
    u8 index;
    u8 num;
    u8 op[BAT_OP_NUM];
    u32 tick;
    u8 rpt;
}BAT_OPINFO;
extern BAT_OPINFO g_sBatOpInfo;

#define Bat_ResetOpInfo(p)              do{(p)->state = BAT_OP_STAT_IDLE; (p)->num = 0; (p)->index = 0; (p)->rpt = 0; (p)->tick = 0; }while(0)


#define BAT_FRAME_HEAD                  0x7E
#define BAT_FRAME_MIN_LEN               6
#define BAT_FRAME_POS_HEAD              0          
#define BAT_FRAME_POS_LED               1
#define BAT_FRAME_POS_CMD               3
#define BAT_FRAME_POS_DAT               4

void Bat_Init(void);
void Bat_Stop(void);
void Bat_GetOp(BAT_OPINFO *pOpInfo);

u16 Bat_GetCrc(u8 *pFrame, u32 len);
u16 Bat_FormatCmd(u8 *pTxBuf, u8 cmd, u8 *pParams, u16 len);
void Bat_TxCmd(BAT_TXFRAME *pTxFrame, u8 cmd);

BOOL Bat_CheckFrame(BAT_RXFRAME *pRxFrame, u16 *pStartPos);//u8 cmd, BAT_INFO *pBatInfo

u8 Bat_GetNext(BAT_OPINFO *pOpInfo);
#endif
