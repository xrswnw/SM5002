#ifndef _ANYID_SM5002_BAT_
#define _ANYID_SM5002_BAT_

#include "AnyID_SM5002_Bat_HL.h"
//#include "AnyID_Uart_Receive.h"

typedef struct batTxFrame{
    u8 buffer[BAT_TX_FRAME_LEN];
    u16 len;
}BAT_TXFRAME;

typedef struct batRxFrame{
    u8 buffer[BAT_RX_FRAME_LEN];
    u16 len;
    u8 state;
}BAT_RXFRAME;


//extern BAT_RXFRAME g_sBatRcvFrame[BAT_CH_NUM];
//extern BAT_TXFRAME g_sBatTxFrame[BAT_CH_NUM];

#define BAT_CMD_GET_VER                 0x01
#define BAT_CMD_GET_SN                  0x02
#define BAT_CMD_GET_STATUS              0x03
#define BAT_CMD_CHAG_HEART              0x04
#define BAT_CMD_GET_ERR                 0x05
#define BAT_CMD_GET_UNITVOL             0x07

//#pragma pack(1)
typedef struct batStatusInfo{
    u16 volValue;       //��ص�ѹ����λ0.1V
    u8 volLev;          //��������λ1%
    u16 cycleCount;     //ѭ������
    u8 chargeStatus;    //���״̬ 1-���� 1-�ŵ� 2-���
    u16 maxChagVol;     //�����ѹ����λ0.1V
    u16 maxChagCur;     //������������λ0.1A
    u16 remainCap;      //ʣ����������λ0.01Ah
    u16 curValue;       //��ŵ��������λ0.1A
    u8 tmpr;            //�¶�
    u8 dltVol;          //�������ѹ���λ1mV
}BAT_STATUSINFO;

#define BAT_ERR1_MSK                    0xFF
#define BAT_ERR2_MSK                    0xF8//0xFA
#define BAT_ERR3_MSK                    0xFF
#define BAT_ERR4_MSK                    0xFF
typedef struct batErrInfo{
    u8 fullState;
    u8 protectStatus1;
    u8 alarmStatus1;
    u8 alarmStatus2;
    u8 diagStatus;
}BAT_ERRINFO;

#define BAT_SN_LEN                      6
#define BAT_STAT_EMPTY                  0x00        //��-�޵��
#define BAT_STAT_EXIST                  0x01        //���ڵ��
#define BAT_STAT_CHARGE                 0x20        //��س����
#define BAT_STAT_FULL                   0x40        //�������
#define BAT_STAT_ERR                    0x80        //��ع���
#define BAT_UNIT_NUM                    20
extern const u8 g_nBatBoardcastSn[BAT_SN_LEN];
typedef struct batInfo{
    u8 state;
    u16 verHard;
    u16 verSoft;
    u8 sn[BAT_SN_LEN];
    BAT_STATUSINFO status;
    BAT_ERRINFO err;
    u16 unitVol[BAT_UNIT_NUM];
}BAT_INFO;
#define Bat_CheckErr(err)               ((((err).protectStatus1 & BAT_ERR1_MSK) != 0) || \
                                          (((err).alarmStatus1 & BAT_ERR2_MSK) != 0) || \
                                          (((err).alarmStatus2  & BAT_ERR3_MSK) != 0) || \
                                          (((err).diagStatus  & BAT_ERR4_MSK) != 0))
       

//#define BAT_CHK_TIME_FAST               20
//#define BAT_CHK_TIME_SLOW               2000//200
//#define BAT_CHK_TIME_MID                400

#define BAT_DELAY_TIME                  4
#define BAT_OP_WAIT_TIME                100

#define BAT_OP_NUM                      5

#define BAT_OP_STAT_IDLE                0x00
#define BAT_OP_STAT_TX                  0x01
#define BAT_OP_STAT_WAIT                0x02
#define BAT_OP_STAT_RX                  0x03
#define BAT_OP_STAT_NEXT                0x80
#define BAT_OP_STAT_TO                  0x05
#define BAT_OP_STAT_DLY                 0x06

#define BAT_OP_RPT_TIM                  3

#define BAT_MODE_GET_INFO               0   //��ȡ�����Ϣ
#define BAT_MODE_CHARGE                 1   //���ģʽ
#define BAT_MODE_CHK_STATUS             2   //У����״̬
typedef struct batOpInfo{
    u8 mode;
    u8 state;
    u8 index;
    u8 num;
    u8 op[BAT_OP_NUM];
    u32 tick;
    u8 rpt;
    //u16 to;
    BAT_TXFRAME txFrame;
    BAT_RXFRAME rxFrame;
    BAT_INFO batInfo;
}BAT_OPINFO;
extern BAT_OPINFO g_sBatOpInfo[BAT_CH_NUM];
#define Bat_IsExist(p)                  (((p)->batInfo.state & BAT_STAT_EXIST) == BAT_STAT_EXIST)
#define Bat_IsIdle(p)                   ((p)->state == BAT_OP_STAT_IDLE)
#define Bat_IsErr(p)                    (((p)->batInfo.state & BAT_STAT_ERR) == BAT_STAT_ERR)
#define Bat_IsFull(p)                   (((p)->batInfo.state & BAT_STAT_FULL) == BAT_STAT_FULL)

#define Bat_ResetOpInfo(p)              do{(p)->state = BAT_OP_STAT_IDLE; (p)->num = 0; (p)->index = 0; (p)->rpt = 0; (p)->tick = 0; }while(0)


#define BAT_FRAME_HEAD                  0x7E
#define BAT_FRAME_MIN_LEN               6
#define BAT_FRAME_POS_HEAD              0          
#define BAT_FRAME_POS_LEN               1
#define BAT_FRAME_POS_CMD               3
#define BAT_FRAME_POS_DAT               4

void Bat_Init(u8 index);
void Bat_Stop(u8 index);
void Bat_GetOp(BAT_OPINFO *pOpInfo);

u16 Bat_GetCrc(u8 *pFrame, u32 len);
u16 Bat_FormatCmd(u8 *pTxBuf, u8 cmd, u8 *pParams, u16 len);

BOOL Bat_CheckFrame(BAT_RXFRAME *pRxFrame, u16 *pStartPos, u8 cmd);//u8 cmd, BAT_INFO *pBatInfo
BOOL Bat_FormatRxInfo(u8 *pFrame, BAT_INFO *pBatInfo, u8 index);

BOOL Bat_GetNext(BAT_OPINFO *pOpInfo);
#endif
