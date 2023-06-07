#include "AnyID_SM5002_AD.h"

#define AD_TMPR_LV_NUM          64   
#define AD_TMPR_MIN             -50  
u16 const g_aADTmprBuf[AD_TMPR_LV_NUM] = {

};
void Win_CalAvg(WINAVG_INFO *pInfo, int value)
{
    //16´°¿ÚÆ½»¬ÂË²¨
    pInfo->index++;
    pInfo->sum -= pInfo->buffer[pInfo->index & 0x0F];
    if(pInfo->index == 0)
    {
        pInfo->index = WIN_SAMPLE_NUM;
    }
    pInfo->buffer[pInfo->index & 0x0F] = value;
    pInfo->sum += value;
    if(pInfo->index >= WIN_SAMPLE_NUM)
    {
        pInfo->avg = pInfo->sum >> 4;
    }
    else
    {
        pInfo->avg = pInfo->sum / pInfo->index;
    }
    
    pInfo->avg = (pInfo->avg * 12 * 3200) >> 12;
}