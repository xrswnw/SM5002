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

void Win_CalAvg(WINAVG_INFO *pInfo, int value);
#endif