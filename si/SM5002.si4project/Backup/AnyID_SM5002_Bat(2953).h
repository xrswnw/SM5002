#ifndef _ANYID_SM5002_BAT_
#define _ANYID_SM5002_BAT_

#include "AnyID_SM5002_Bat_HL.h"
#include "AnyID_Uart_Receive.h"

extern UART_RCVFRAME g_sBatRcvFrame;
extern UART_TXFRAME g_sBatTxFrame;

#define BAT_CMD_GET_VER                 0x01
#define BAT_CMD_GET_SN                  0x02
#define BAT_CMD_GET_STATUS              0x03
#define BAT_CMD_CHAG_HEART              0x04



void Bat_Init(void);
void Bat_Stop(void);

#endif
