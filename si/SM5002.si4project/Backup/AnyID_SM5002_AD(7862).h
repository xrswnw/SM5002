#ifndef _ANYID_SM5002_AD_
#define _ANYID_SM5002_AD_

#include "AnyID_SM5002_AD_HL.h"

#define WIN_SAMPLE_NUM                  16
typedef struct temperatureInfo{
    int sum;
    int avg;
    u32 index;
    int buffer[WIN_SAMPLE_NUM];
    int t;
}WINAVG_INFO;

typedef struct adTempInfo{
    WINAVG_INFO t1;
    WINAVG_INFO t2;
}AD_TEMPINFO;
extern AD_TEMPINFO g_sTemperInfo;

void Win_CalAvg(WINAVG_INFO *pInfo, int value);
#endif