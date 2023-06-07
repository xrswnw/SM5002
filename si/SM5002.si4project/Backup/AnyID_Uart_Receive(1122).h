#ifndef _ANYID_UART_RECEIVE_
#define _ANYID_UART_RECEIVE_

#include "AnyID_Type.h"
#include "AnyID_Stdlib.h"

#define UART_COM_LED_TIM                6

#define UART_FRAME_CMD_HARD_INFO        0x00
#define UART_FRAME_CMD_IDLE             0x00
#define UART_FRAME_CMD_RST              0x04
//#define UART_FRAME_CMD_SET_CFG          0xF4
//#define UART_FRAME_CMD_GET_CFG          0xF5
#define UART_FRAME_CMD_VER              0xF7
#define UART_FRAME_CMD_GETCPUID         0xF8
#define UART_FRAME_CMD_RSTEE            0xED
#define UART_FRAME_CMD_WEE              0xEE
#define UART_FRAME_CMD_REE              0xEF

#define UART_FRAME_CMD_AUTH_CLR         0xA0
#define UART_FRAME_CMD_AUTH_WRT         0xA1

#define UART_FRAME_CMD_RREGS            0x03 
#define UART_FRAME_CMD_WREGS            0x10

#define UART_FRAME_CMD_AUTO             0xFC
#define UART_FRAME_CMD_MODBUS           0xFD        //MODBUSÖ¸Áî
//#define UART_FRAME_CMD_ESAM             0xFE
//#define UART_FRAME_CMD_PF               0xFF        //±¶¼Ó¸£Ö¸Áî

#define UART_FRAME_POS_HEAD1            0
#define UART_FRAME_POS_HEAD2            1
#define UART_FRAME_POS_LEN              2
#define UART_FRAME_POS_SRCADDR          3
#define UART_FRAME_POS_DESTADDR         5
#define UART_FRAME_POS_CMD              7
#define UART_FRAME_POS_RFU              8
#define UART_FRAME_POS_PAR              9

#define UART_FRAME_BROADCAST_ADDR       0xFFFF
#define UART_FRAME_MIN_LEN              11

#define UART_FRAME_FLAG_HEAD1           0x7E
#define UART_FRAME_FLAG_HEAD2           0x55

#define UART_FRAME_RESPONSE_FLAG        0x1F
#define UART_FRAME_PARAM_RFU            0x00

#define UART_FRAME_FLAG_OK              0x00
#define UART_FRAME_FLAG_FAIL            0x01
#define UART_FRAME_RSP_NOERR            0x00
#define UART_FRAME_RSP_TAGERR           0x01
#define UART_FRAME_RSP_CRCERR           0x02
#define UART_FRAME_RSP_NORSP            0x03
#define UART_FRAME_RSP_PARERR           0x04

#define UART_DATA_MAX_LEN               1024
#define UART_BUFFER_MAX_LEN             (UART_DATA_MAX_LEN + 32) 

#define UART_STAT_END                   0x03
#define UART_STAT_OVR                   0x01
#define UART_STAT_IDLE                  0x40
#define UART_STAT_RCV                   0x80
#define UART_STAT_MSK                   0xC0

typedef struct uartRcvFrame{
    u8 state;
    u16 len;
    u8 buffer[UART_BUFFER_MAX_LEN];
    u32 idleTime;
}UART_RCVFRAME;

#define Uart_IncIdleTime(t, rcvFrame)      do{\
                                                if((rcvFrame).state & UART_STAT_RCV)\
                                                {\
                                                    (rcvFrame).idleTime += (t);\
                                                    if((rcvFrame).idleTime >= (10 * t))\
                                                    {\
                                                        (rcvFrame).state = UART_STAT_IDLE;\
                                                    }\
                                                }\
                                            }while(0)

#define Uart_ReceiveFrame(byte, rcvFrame) do{\
                                                if((rcvFrame).state & UART_STAT_MSK)\
                                                {\
                                                    (rcvFrame).state |= UART_STAT_RCV;\
                                                    (rcvFrame).buffer[(rcvFrame).len++] = (byte);\
                                                    if((rcvFrame).len == UART_BUFFER_MAX_LEN)\
                                                    {\
                                                        (rcvFrame).state = UART_STAT_OVR;\
                                                    }\
                                                }\
                                             }while(0)

#define Uart_IsRcvFrame(rcvFrame)			((rcvFrame).state == UART_STAT_TO || (rcvFrame).state == UART_STAT_OVR)
#define Uart_ResetFrame(rcvFrame)           ((rcvFrame).state = UART_STAT_IDLE, (rcvFrame).len = 0)

extern UART_RCVFRAME g_sUartTempRcvFrame;
BOOL Uart_AnalyseFrame(UART_RCVFRAME *pRcvFrame, u16 *pstartIndex, u16 *pFrameLen);

#define UART_MODBUS_UNITFLAG_ADDR       0xFF
#define UART_MODBUS_BROADCAST_ADDR      0
#define UART_MODBUS_TCP_MIN_LEN         (7 + 5) //HEAD-7 FUN-1 REGNUN-2 REGADDR-2
#define UART_MODBUS_TCP_RTU_MIN_LEN     (5)
#define UART_MODBUS_RTU_MIN_LEN         (8)
#define UART_MODBUS_REG_LEN             256
#define UART_MODBUS_TCP_MBAP_LEN        6

typedef struct uartModbusInfo{
    BOOL bAsciiFrame;
    BOOL bMbap;
    u16 head;
    u16 protocol;
    u16 len;

    u8 addr;
    u8 fun;
    u16 regAddr;
    u16 regNum;
    u16 reg;
    u8 params[UART_MODBUS_REG_LEN << 1];
    u16 paramsLen;
}UART_MODBUS;
extern UART_MODBUS g_sUartModbusInfo;
BOOL Uart_AnalyseModbus(UART_RCVFRAME *pRcvFrame, u8 devAddr, UART_MODBUS *pModbusInfo, BOOL bMbap);
void Uart_ModbusByte2Reg(u8 *pByte, u16 *pReg, u16 regAddr, u16 regNum);
void Uart_ModbusReg2Byte(u8 *pByte, u16 *pReg, u16 regAddr, u16 regNum);
u16 Uart_ModbusCalCrc(u8 *pBuffer, u8 len);

#define Uart_ModbusGetData(frame, pos, dat)         (dat) = (frame)[(pos)++]; (dat) <<= 8; (dat) |= (frame)[(pos)++]
#define UART_MODBUS_RW_REGS_NUM_MIN                 0x0001
#define UART_MODBUS_R_REGS_NUM_MAX                  0x007D
#define UART_MODBUS_W_REGS_NUM_MAX                  0x007B

#define UART_MODBUS_READ_REG_PARAMSLEN              4

#define UART_MODBUS_COL_ON                           0xFF00
#define UART_MODBUS_COL_OFF                          0x0000
#define UART_MODBUS_FUN_IDLE                         0x00
#define UART_MODBUS_FUN_READ_COIL                    0x01
#define UART_MODBUS_FUN_WRITE_COIL                   0x05
#define UART_MODBUS_FUN_WRITE_COILS                  0x0F
#define UART_MODBUS_FUN_READ_INREGS                  0x04
#define UART_MODBUS_FUN_READ_HOLDREGS                0x03
#define UART_MODBUS_FUN_WRITE_HOLDREG                0x06
#define UART_MODBUS_FUN_WRITE_HOLDREGS               0x10

#define UART_MODBUS_FUN_ERR_FLAG                     0x80
#define UART_MODBUS_ERR_OK                           0x00
#define UART_MODBUS_ERR_FUN                          0x01
#define UART_MODBUS_ERR_DATADDR                      0x02
#define UART_MODBUS_ERR_DAT                          0x03
#define UART_MODBUS_ERR_DEV                          0x04
#define UART_MODBUS_ERR_BSY                          0x06
#define UART_MODBUS_ERR_FRAME                        0xFF
#define UART_MODBUS_ERR_WAIT                         0xFE

#define UART_MODBUS_REQLEN_WREG                     8           //addr fun addrh addrl valueh valuel crc1 crc2
#define UART_MODBUS_REQLEN_WREGS_MIN                9           //addr fun addrh addrl regnumh regnuml bytnum regssss crc1 crc2
#define UART_MODBUS_REQLEN_RREGS                    8           //addr fun addrh addrl regnumh regnuml crc1 crc2
#define UART_MODBUS_REQLEN_RINREG                   8           //addr fun addrh addrl regnumh regnuml crc1 crc2
#define UART_MODBUS_REQLEN_WCOILS                   9           //addr fun addrh addrl regnumh regnuml bytnum regsss crc1 crc2
#define UART_MODBUS_REQLEN_WCOIL                    8           //addr fun addrh addrl valueh valuel crc1 crc2

extern const u8 g_aAsictoData[256];
#define UART_MODBUS_ASCII_HEAD                      0x3A        //':'
#define UART_MODBUS_ASCII_TAIL1                     0x0D
#define UART_MODBUS_ASCII_TAIL2                     0x0A
#define UART_MODBUS_ASCII_MIN_LEN                   17          //head  addr-2b fun-2b regaddr-4b value-4b lrc-2b 0a-1b 0a-1b

#define Uart_GetDataFromAsic(p, byt)                do{(byt) = g_aAsictoData[(p)[0]]; (byt) <<= 4; (byt) |= g_aAsictoData[(p)[1]];}while(0)
BOOL Uart_AnalyseASCIIModbus(UART_RCVFRAME *pRcvFrame, u8 devAddr, UART_MODBUS *pModbusInfo);


extern const u8 g_aDatatoAsic[16];
#define Uart_GetAsicFromData(p, d)                  ((p)[0] = g_aDatatoAsic[((d) >> 4) & 0x0F], (p)[1] = g_aDatatoAsic[((d) >> 0) & 0x0F])


u16 Uart_ModbusReadRegsRspFrame(u8 regNum, u16 *pRegs, u8 *pFrame, UART_MODBUS *pModbusInfo);
u16 Uart_ModbusWriteRegsRspFrame(u16 regAddr, u16 regNum, u8 *pFrame, UART_MODBUS *pModbusInfo);
u16 Uart_ModbusWriteRegRspFrame(u16 regAddr, u16 reg, u8 *pFrame, UART_MODBUS *pModbusInfo);
u16 Uart_ModbusWriteColRspFrame(u16 regAddr, u16 reg, u8 *pFrame, UART_MODBUS *pModbusInfo);
u16 Uart_ModbusWriteColsRspFrame(u16 regAddr, u16 regNum, u8 *pFrame, UART_MODBUS *pModbusInfo);
u16 Uart_ModbusErrorFrame(u8 err, u8 *pFrame, UART_MODBUS *pModbusInfo);


#define UART_USR_ADDR_BROADCAST                     0xFFFF
BOOL Uart_UsrCheckFrame(UART_RCVFRAME *pRcvFrame, u16 *pStartPos, u16 addr);
u16 Uart_UsrResponseFrame(u8 *pParam, u8 len, u8 cmd, u16 addr, u8 *pBuffer);
u16 Uart_UsrOpTagErrorFrame(u8 errCode, u8 tagMemLen, u8 cmd, u16 addr, u8 *pBuffer);
u16 Uart_UsrRspErrorFrame(u8 errCode, u8 cmd, u16 addr, u8 *pBuffer);
u16 Uart_UsrRspLongFrame(u16 *pParams, u16 regNum, u8 errCode, u8 cmd, u16 addr, u8 *pBuffer);

#endif