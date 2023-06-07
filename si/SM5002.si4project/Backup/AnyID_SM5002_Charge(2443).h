#ifndef _ANYID_SM5002_CHAG_
#define _ANYID_SM5002_CHAG_

#include "AnyID_SM5002_Charge_HL.h"
//#include "AnyID_Uart_Receive.h"

typedef struct chargeTxFrame{
    u8 buffer[CHAG_TX_FRAME_LEN];
    u16 len;
}CHAG_TXFRAME;

typedef struct chargeRxFrame{
    u8 buffer[CHAG_RX_FRAME_LEN];
    u16 len;
    u8 state;
    u32 rxTick;
}CHAG_RXFRAME;

void Chag_Init(u8 index);
void Chag_Stop(u8 index);

#define CHAG_DEV_ADDR           0x01                                              //modbus��ַ������
#define CHAG_DEV_NUM            2
#define CHAG_DEV_INDEX0         0
#define CHAG_DEV_INDEX1         1

#define CHAG_STAT_EMPTY         0x00
#define CHAG_STAT_EXIST         0x01        //����
#define CHAG_STAT_CHARGE        0x20        //��س����
#define CHAG_STAT_ERR           0x80        //����

#define CHAG_VENDOR_NAME_LEN    4
#define CHAG_ERR_FLAG_LOW_MSK   0x000F
#define CHAG_ERR_CODE_MIN       0x000A
#define CHAG_ERR_FLAG_HIG_MSK   0xCF10
typedef struct chargeDevInfo{
    u8 state;
    u8 vendorName[CHAG_VENDOR_NAME_LEN];
    u16 softVer;
    u16 hardVer;
    u16 type;
    u16 pwr;
    u16 maxVol;
    u16 maxCur;
    u16 chagVol;
    u16 chagCur;
    u16 errStatusInfo;
}CHAG_INFO;
#define Chag_CheckErr(err)              (((err) & CHAG_ERR_FLAG_LOW_MSK) >= CHAG_ERR_CODE_MIN || ((err) & CHAG_ERR_FLAG_HIG_MSK) > 0)

#define CHAG_OP_OPEN_CHARGE     0
#define CHAG_OP_CFG_PARAMS      1
#define CHAG_OP_GET_INFO        2
#define CHAG_OP_CLOSE_CHARGE    3
#define CHAG_OP_GET_STATUS      4

#define CHAG_OP_NUM             8

//���Բ��д��ڣ����Ƿ����ȼ�
#define CHAG_MODE_INIT          0x01
#define CHAG_MODE_IDLE          0x02
#define CHAG_MODE_CLOSE         0x04
#define CHAG_MODE_OPEN          0x08
#define CHAG_MODE_CHARGE        0x10

#define CHAG_OP_STAT_IDLE       0x00
#define CHAG_OP_STAT_TX         0x01
#define CHAG_OP_STAT_WAIT       0x02
#define CHAG_OP_STAT_RX         0x03
#define CHAG_OP_STAT_NEXT       0x80
#define CHAG_OP_STAT_TO         0x05
#define CHAG_OP_STAT_DLY        0x06

#define CHAG_FRAME_MIN_LEN      8
#define CHAG_FRAME_POS_ADDR     0          
#define CHAG_FRAME_POS_FUN      1
#define CHAG_FRAME_POS_DAT      4

//#define CHAG_CHK_TIME_FAST      20
//#define CHAG_CHK_TIME_SLOW      200//200
#define CHAG_DELAY_TIME         2
#define CHAG_OP_WAIT_TIME       20

#define CHAG_OP_RPT_TIM         3

#define CHAG_VOL_STEP1          720//600
#define CHAG_VOL_STEP2          690
#define CHAG_VOL_STEP3          690


#define CHAG_CUR_STEP1          600
#define CHAG_CUR_STEP2          1000
#define CHAG_CUR_STEP3          700


#define CHAG_LOW_VOL_MIN_TIM    2000    //���ٳ��10s 
#define CHAG_VOL_LEV_LOW        10      //
#define CHAG_VOL_LEV_HIG        90      //
#define CHAG_VOL_LEV_FUL        96      //
#define CHAG_STEP_NUM           3       //������粽�裺�����硢��ѹ��硢��ѹ���
#define CHAG_STEP_IDX0          0
#define CHAG_STEP_IDX1          1
#define CHAG_STEP_IDX2          2
typedef struct chagParams{
    u16 fulVolLev;              //������ֵ
    u16 lowVolLev;              //�͵�����ֵ
    u16 higVolLev;              //�ߵ�����ֵ
    struct chagVolCur{
        u16 vol;
        u16 cur;
    }stepParams[CHAG_STEP_NUM];
}CHAG_PARAMS;

typedef struct chargeOpInfo{
    u8 mode;                    //����ģʽ
    u8 state;                   //����״̬
    u8 index;                   //��ǰ��������
    u8 num;                     //����������Ŀ
    u8 op[CHAG_OP_NUM];         //��������
    u32 tick;                   //����ʱ��
    u8 rpt;                     //ͨ���ظ�����
    CHAG_TXFRAME txFrame;
    CHAG_RXFRAME rxFrame;
    CHAG_INFO chagInfo;         //������Ϣ
    //u8 addr;                    //�����豸��ַ
    u8 fun;                     //��������������
    u8 chagStep;                //��粽��
    u16 chagVol;                //��ǰ���õĳ���ѹ
    u16 chagCur;                //��ǰ���õĳ�����
    u32 chagTime;               //��翪ʼʱ��
    u16 batVolLv;               //��ǰ�ĵ�ص���
    CHAG_PARAMS *pChagParams;
}CHAG_OPINFO;
extern CHAG_OPINFO g_sChagOpInfo[CHAG_DEV_NUM];
//extern CHAG_OPINFO *g_pChagOpInfo;
#define Chag_IsIdle(p)                  ((p)->state == CHAG_OP_STAT_IDLE)
#define Chag_IsExist(p)                 (((p)->chagInfo.state & CHAG_STAT_EXIST) == CHAG_STAT_EXIST)
#define Chag_IsErr(p)                   (((p)->chagInfo.state & CHAG_STAT_ERR) == CHAG_STAT_ERR)

//#define CHAG_OP_BUF_SIZE        3       //���֮������ͬʱ����
//typedef struct chargeOpBuffer{
//    u8 s;
//    u8 e;
//    u8 curIndex;
//    u8 devIndex[CHAG_OP_BUF_SIZE];
//}CHAG_OPBUF;
//extern CHAG_OPBUF g_sChagOpBuf;
//#define Chag_OpBufEmpty()               (g_sChagOpBuf.s == g_sChagOpBuf.e)

u8 Chag_GetCurIndex(CHAG_OPINFO *pOpInfo);

#define Chag_ResetOpInfo(p)     do{(p)->state = CHAG_OP_STAT_IDLE; (p)->num = 0; (p)->index = 0; (p)->rpt = 0; (p)->tick = 0; }while(0)


u16 Chag_CalCrc(u8 *pBuffer, u8 len);
u16 Chag_FormatOpFrame(CHAG_OPINFO *pOpInfo, u8 *pTxBuf);
BOOL Chag_CheckFrame(CHAG_RXFRAME *pRxFrame, u16 *pStartPos, u8 fun);
BOOL Chag_GetNext(CHAG_OPINFO *pOpInfo);
void Chag_GetOp(CHAG_OPINFO *pOpInfo);

//void Chag_AddOpDev(CHAG_OPBUF *pOpBuf, u8 index);
//u8 Chag_GetCurDev(CHAG_OPBUF *pOpBuf);
void Chag_FormatRxInfo(u8 *pRxBuf, CHAG_OPINFO *pOpInfo);

#endif
