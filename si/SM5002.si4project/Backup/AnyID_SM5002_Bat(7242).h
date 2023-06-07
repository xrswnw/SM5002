#ifndef _ANYID_SM5002_CHAG_
#define _ANYID_SM5002_CHAG_

#include "AnyID_SM5002_Charge_HL.h"
#include "AnyID_Uart_Receive.h"

extern UART_RCVFRAME g_sChagRcvFrame;
extern UART_TXFRAME g_sChagTxFrame;

void Chag_Init(void);
void Chag_Stop(void);

#endif
