#include "AnyID_SM5002_AD.h"

#define AD_TMPR_LV_NUM          90   
#define AD_TMPR_MIN             -20  
u16 const g_aADTmprBuf[AD_TMPR_LV_NUM] = {
 179,  189,  200,  212,  224,  236,  249,  263,  277,  292,  
 308,  324,  342,  359,  378,  397,  417,  438,  459,  482,  
 505,  529,  554,  580,  607,  635,  664,  693,  724,  755,  
 788,  821,  856,  891,  927,  965, 1003, 1042, 1083, 1124, 
1166, 1209, 1253, 1298, 1344, 1390, 1438, 1486, 1535, 1585, 
1636, 1687, 1739, 1792, 1845, 1898, 1953, 2007, 2063, 2118, 
2174, 2230, 2287, 2344, 2401, 2458, 2515, 2572, 2630, 2687, 
2744, 2801, 2858, 2915, 2971, 3027, 3083, 3139, 3194, 3249, 
3303, 3357, 3411, 3463, 3515, 3567, 3618, 3669, 3719, 3767, 
};
void Win_CalAvg(WINAVG_INFO *pInfo, int value)
{
    u8 i = 0;
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
    pInfo->t = AD_TMPR_MIN;
    for(i = 0; i < AD_TMPR_LV_NUM; i++)
    {
        if(pInfo->avg < g_aADTmprBuf[i])
        {
            break;
        }
    }
    pInfo->t += i;
    //pInfo->avg = (pInfo->avg * 12 * 3200) >> 12;
}