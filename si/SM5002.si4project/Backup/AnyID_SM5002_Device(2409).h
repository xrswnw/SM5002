#ifndef _ANYID_SM5002_DEVICE_
#define _ANYID_SM5002_DEVICE_

#include "AnyID_SM5002_Config.h"
#include "AnyID_SM5002_FRam.h"
#include "AnyID_SM5002_SysTick_HL.h"
#include "AnyID_SM5002_WDG_HL.h"
#include "AnyID_Uart_Receive.h"




#define READER_VERSION_SIZE             50
extern const u8 READER_VERSION[READER_VERSION_SIZE];

#define READER_APP_OK                   0x5555
#define READER_APP_FAIL                 0xAAAA


void Reader_Delayms(u32 n);

void Reader_ReadDeviceParamenter(void);
BOOL Reader_WriteDeviceParamenter(void);

#endif
