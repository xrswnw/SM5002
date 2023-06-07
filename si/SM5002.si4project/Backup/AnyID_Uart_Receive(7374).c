#include "AnyID_Uart_Receive.h"


UART_RCVFRAME g_sUartTempRcvFrame = {0};
UART_MODBUS g_sUartModbusInfo = {0};

const u8 g_aDatatoAsic[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

const u8 g_aAsictoData[256] =
{
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

void Uart_ModbusByte2Reg(u8 *pByte, u16 *pReg, u16 regAddr, u16 regNum)
{
    u16 i = 0;
    u16 t = 0;
    for(i = 0; i < (regNum << 1); i += 2)
    {
        t = pByte[i + 0];
        t <<= 8;
        t |= pByte[i + 1];
        pReg[regAddr + (i >> 1)] = t;
    }
}

void Uart_ModbusReg2Byte(u8 *pByte, u16 *pReg, u16 regAddr, u16 regNum)
{
    u16 i = 0;
    u16 t = 0;
    for(i = 0; i < regNum; i++)
    {
        t = pReg[regAddr + i];
        pByte[(i << 1) + 0] = (t >> 8) & 0xFF;
        pByte[(i << 1) + 1] = (t >> 0) & 0xFF;
    }
}

u16 Uart_ModbusReadRegsRspFrame(u8 regNum, u16 *pRegs, u8 *pFrame, UART_MODBUS *pModbusInfo)
{
    u16 pos = 0;
    u16 i = 0;

    if(pModbusInfo->bMbap)
    {
        pModbusInfo->len = 3 + (regNum << 1);
        pFrame[pos++] = (pModbusInfo->head >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->head >> 0) & 0xFF;
        pFrame[pos++] = (pModbusInfo->protocol >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->protocol >> 0) & 0xFF;
        pFrame[pos++] = (pModbusInfo->len >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->len >> 0) & 0xFF;
    }
    
    if(pModbusInfo->bAsciiFrame == FALSE)
    {
        pFrame[pos++] = pModbusInfo->addr;
        pFrame[pos++] = pModbusInfo->fun;

        pFrame[pos++] = regNum << 1;
        for(i = 0; i < regNum; i++)
        {
            pFrame[pos++] = (pRegs[i] >> 8) & 0xFF;
            pFrame[pos++] = (pRegs[i] >> 0) & 0xFF;
        }

        if(!pModbusInfo->bMbap)
        {
            u16 crc = 0;

            crc = Uart_ModbusCalCrc(pFrame, pos);
            pFrame[pos++] = (crc >> 0) & 0xFF;
            pFrame[pos++] = (crc >> 8) & 0xFF;
        }
    }
    else
    {
        u8 lrc = 0, t = 0;
        
        pFrame[pos++] = ':';
        
        Uart_GetAsicFromData(pFrame + pos, pModbusInfo->addr); pos += 2; lrc += pModbusInfo->addr;
        Uart_GetAsicFromData(pFrame + pos, pModbusInfo->fun); pos += 2; lrc += pModbusInfo->fun;
        
        t = (regNum << 1); Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;
        for(i = 0; i < regNum; i++)
        {
            t = (pRegs[i] >> 8) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;
            t = (pRegs[i] >> 0) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;
        }

        lrc = ~lrc; lrc += 1;
        Uart_GetAsicFromData(pFrame + pos, lrc); pos += 2;
        
        pFrame[pos++] = '\r'; pFrame[pos++] = '\n';
    }
    return pos;
}

u16 Uart_ModbusWriteRegsRspFrame(u16 regAddr, u16 regNum, u8 *pFrame, UART_MODBUS *pModbusInfo)
{
    u16 pos = 0;

    if(pModbusInfo->bMbap)
    {
        pModbusInfo->len = 6;
        pFrame[pos++] = (pModbusInfo->head >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->head >> 0) & 0xFF;
        pFrame[pos++] = (pModbusInfo->protocol >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->protocol >> 0) & 0xFF;
        pFrame[pos++] = (pModbusInfo->len >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->len >> 0) & 0xFF;
    }
    if(pModbusInfo->bAsciiFrame == FALSE)
    {
        pFrame[pos++] = pModbusInfo->addr;
        pFrame[pos++] = pModbusInfo->fun;

        pFrame[pos++] = (regAddr >> 8) & 0xFF;
        pFrame[pos++] = (regAddr >> 0) & 0xFF;
        pFrame[pos++] = (regNum >> 8) & 0xFF;
        pFrame[pos++] = (regNum >> 0) & 0xFF;

        if(!pModbusInfo->bMbap)
        {
            u16 crc = 0;

            crc = Uart_ModbusCalCrc(pFrame, pos);
            pFrame[pos++] = (crc >> 0) & 0xFF;
            pFrame[pos++] = (crc >> 8) & 0xFF;
        }
    }
    else
    {
        u8 lrc = 0, t = 0;
        
        pFrame[pos++] = ':';
        
        Uart_GetAsicFromData(pFrame + pos, pModbusInfo->addr); pos += 2; lrc += pModbusInfo->addr;
        Uart_GetAsicFromData(pFrame + pos, pModbusInfo->fun); pos += 2; lrc += pModbusInfo->fun;
        
        t = (regAddr >> 8) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;
        t = (regAddr >> 0) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;
        t = (regNum >> 8) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;
        t = (regNum >> 0) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;

        lrc = ~lrc; lrc += 1;
        Uart_GetAsicFromData(pFrame + pos, lrc); pos += 2;
        
        pFrame[pos++] = '\r'; pFrame[pos++] = '\n';
    }
    return pos;
}

u16 Uart_ModbusWriteRegRspFrame(u16 regAddr, u16 reg, u8 *pFrame, UART_MODBUS *pModbusInfo)
{
    u16 pos = 0;

    if(pModbusInfo->bMbap)
    {
        pModbusInfo->len = 6;
        pFrame[pos++] = (pModbusInfo->head >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->head >> 0) & 0xFF;
        pFrame[pos++] = (pModbusInfo->protocol >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->protocol >> 0) & 0xFF;
        pFrame[pos++] = (pModbusInfo->len >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->len >> 0) & 0xFF;
    }
    
    if(pModbusInfo->bAsciiFrame == FALSE)
    {
        pFrame[pos++] = pModbusInfo->addr;
        pFrame[pos++] = pModbusInfo->fun;

        pFrame[pos++] = (regAddr >> 8) & 0xFF;
        pFrame[pos++] = (regAddr >> 0) & 0xFF;
        pFrame[pos++] = (reg >> 8) & 0xFF;
        pFrame[pos++] = (reg >> 0) & 0xFF;

        if(!pModbusInfo->bMbap)
        {
            u16 crc = 0;

            crc = Uart_ModbusCalCrc(pFrame, pos);
            pFrame[pos++] = (crc >> 0) & 0xFF;
            pFrame[pos++] = (crc >> 8) & 0xFF;
        }
    }
    else 
    {
        u8 lrc = 0, t = 0;
        
        pFrame[pos++] = ':';
        
        Uart_GetAsicFromData(pFrame + pos, pModbusInfo->addr); pos += 2; lrc += pModbusInfo->addr;
        Uart_GetAsicFromData(pFrame + pos, pModbusInfo->fun); pos += 2; lrc += pModbusInfo->fun;
        
        t = (regAddr >> 8) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;
        t = (regAddr >> 0) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;
        t = (reg >> 8) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;
        t = (reg >> 0) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;

        lrc = ~lrc; lrc += 1;
        Uart_GetAsicFromData(pFrame + pos, lrc); pos += 2;
        
        pFrame[pos++] = '\r'; pFrame[pos++] = '\n';
    }
    return pos;
}

u16 Uart_ModbusWriteColRspFrame(u16 regAddr, u16 reg, u8 *pFrame, UART_MODBUS *pModbusInfo)
{
    u16 pos = 0;

    if(pModbusInfo->bMbap)
    {
        pModbusInfo->len = 6;
        pFrame[pos++] = (pModbusInfo->head >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->head >> 0) & 0xFF;
        pFrame[pos++] = (pModbusInfo->protocol >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->protocol >> 0) & 0xFF;
        pFrame[pos++] = (pModbusInfo->len >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->len >> 0) & 0xFF;
    }
    
    if(pModbusInfo->bAsciiFrame == FALSE)
    {
        pFrame[pos++] = pModbusInfo->addr;
        pFrame[pos++] = pModbusInfo->fun;

        pFrame[pos++] = (regAddr >> 8) & 0xFF;
        pFrame[pos++] = (regAddr >> 0) & 0xFF;
        pFrame[pos++] = (reg >> 8) & 0xFF;
        pFrame[pos++] = (reg >> 0) & 0xFF;

        if(!pModbusInfo->bMbap)
        {
            u16 crc = 0;

            crc = Uart_ModbusCalCrc(pFrame, pos);
            pFrame[pos++] = (crc >> 0) & 0xFF;
            pFrame[pos++] = (crc >> 8) & 0xFF;
        }
    }
    else
    {
        u8 lrc = 0, t = 0;
        
        pFrame[pos++] = ':';
        
        Uart_GetAsicFromData(pFrame + pos, pModbusInfo->addr); pos += 2; lrc += pModbusInfo->addr;
        Uart_GetAsicFromData(pFrame + pos, pModbusInfo->fun); pos += 2; lrc += pModbusInfo->fun;
        
        t = (regAddr >> 8) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;
        t = (regAddr >> 0) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;
        t = (reg >> 8) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;
        t = (reg >> 0) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;

        lrc = ~lrc; lrc += 1;
        Uart_GetAsicFromData(pFrame + pos, lrc); pos += 2;
        
        pFrame[pos++] = '\r'; pFrame[pos++] = '\n';
    }
    return pos;
}

u16 Uart_ModbusWriteColsRspFrame(u16 regAddr, u16 regNum, u8 *pFrame, UART_MODBUS *pModbusInfo)
{
    u16 pos = 0;

    if(pModbusInfo->bMbap)
    {
        pModbusInfo->len = 6;
        pFrame[pos++] = (pModbusInfo->head >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->head >> 0) & 0xFF;
        pFrame[pos++] = (pModbusInfo->protocol >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->protocol >> 0) & 0xFF;
        pFrame[pos++] = (pModbusInfo->len >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->len >> 0) & 0xFF;
    }
    
    if(pModbusInfo->bAsciiFrame == FALSE)
    {
        pFrame[pos++] = pModbusInfo->addr;
        pFrame[pos++] = pModbusInfo->fun;

        pFrame[pos++] = (regAddr >> 8) & 0xFF;
        pFrame[pos++] = (regAddr >> 0) & 0xFF;
        pFrame[pos++] = (regNum >> 8) & 0xFF;
        pFrame[pos++] = (regNum >> 0) & 0xFF;

        if(!pModbusInfo->bMbap)
        {
            u16 crc = 0;

            crc = Uart_ModbusCalCrc(pFrame, pos);
            pFrame[pos++] = (crc >> 0) & 0xFF;
            pFrame[pos++] = (crc >> 8) & 0xFF;
        }
    }
    else
    {
        u8 lrc = 0, t = 0;
        
        pFrame[pos++] = ':';
        
        Uart_GetAsicFromData(pFrame + pos, pModbusInfo->addr); pos += 2; lrc += pModbusInfo->addr;
        Uart_GetAsicFromData(pFrame + pos, pModbusInfo->fun); pos += 2; lrc += pModbusInfo->fun;
        
        t = (regAddr >> 8) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;
        t = (regAddr >> 0) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;
        t = (regNum >> 8) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;
        t = (regNum >> 0) & 0xFF; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;

        lrc = ~lrc; lrc += 1;
        Uart_GetAsicFromData(pFrame + pos, lrc); pos += 2;
        
        pFrame[pos++] = '\r'; pFrame[pos++] = '\n';
    }
    return pos;
}

u16 Uart_ModbusErrorFrame(u8 err, u8 *pFrame, UART_MODBUS *pModbusInfo)
{
    u16 pos = 0;

    if(pModbusInfo->bMbap)
    {
        pModbusInfo->len = 3;
        pFrame[pos++] = (pModbusInfo->head >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->head >> 0) & 0xFF;
        pFrame[pos++] = (pModbusInfo->protocol >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->protocol >> 0) & 0xFF;
        pFrame[pos++] = (pModbusInfo->len >> 8) & 0xFF;
        pFrame[pos++] = (pModbusInfo->len >> 0) & 0xFF;
    }
    
    if(pModbusInfo->bAsciiFrame == FALSE)
    {
        pFrame[pos++] = pModbusInfo->addr;
        pFrame[pos++] = pModbusInfo->fun | UART_MODBUS_FUN_ERR_FLAG;

        pFrame[pos++] = err;

        if(!pModbusInfo->bMbap)
        {
            u16 crc = 0;

            crc = Uart_ModbusCalCrc(pFrame, pos);
            pFrame[pos++] = (crc >> 0) & 0xFF;
            pFrame[pos++] = (crc >> 8) & 0xFF;
        }
    }
    else
    {
        u8 lrc = 0, t = 0;
        
        pFrame[pos++] = ':';
        
        Uart_GetAsicFromData(pFrame + pos, pModbusInfo->addr); pos += 2; lrc += pModbusInfo->addr;
        t = pModbusInfo->fun | UART_MODBUS_FUN_ERR_FLAG; Uart_GetAsicFromData(pFrame + pos, t); pos += 2; lrc += t;
        
        Uart_GetAsicFromData(pFrame + pos, err); pos += 2; lrc += err;
        
        lrc = ~lrc; lrc += 1;
        Uart_GetAsicFromData(pFrame + pos, lrc); pos += 2;
        
        pFrame[pos++] = '\r'; pFrame[pos++] = '\n';
    }
    return pos;
}

#define POLYNOMIAL                      0xA001   //x^16 + x^12 + x^5 + 1
#define PRESET_VALUE                    0xFFFF
u16 Uart_ModbusCalCrc(u8 *pBuffer, u8 len)
{
    u16 i = 0;
    u16 crc = 0;
    u8 j = 0;

    crc = PRESET_VALUE;
    for(i = 0; i < len; i++)
    {
        crc = crc ^ pBuffer[i];
        for(j = 0; j < 8; j++)
        {
            if(crc & 0x0001)
            {
                crc = (crc >> 1) ^ POLYNOMIAL;
            }
            else
            {
                crc = (crc >> 1);
            }
        }
    }

    return crc;
}

BOOL Uart_AnalyseModbus(UART_RCVFRAME *pRcvFrame, u8 devAddr, UART_MODBUS *pModbusInfo, BOOL bMbap)
{
    BOOL bOk = FALSE;
    u16 pos = 0;
    
    if(bMbap)
    {
        u16 head = 0, protocol = 0, len = 0, addr;
        if(pRcvFrame->len >= UART_MODBUS_TCP_MIN_LEN)
        {
            while(pos + UART_MODBUS_TCP_MIN_LEN <= pRcvFrame->len)
            {
                head = pRcvFrame->buffer[pos++]; head <<= 8; head |= pRcvFrame->buffer[pos++];
                protocol = pRcvFrame->buffer[pos++]; protocol <<= 8; protocol |= pRcvFrame->buffer[pos++];
                len = pRcvFrame->buffer[pos++]; len <<= 8; len |= pRcvFrame->buffer[pos++];
                addr = pRcvFrame->buffer[pos++];
                if(addr == devAddr || addr == UART_MODBUS_BROADCAST_ADDR || addr == UART_MODBUS_UNITFLAG_ADDR)
                {
                    if(len + UART_MODBUS_TCP_MBAP_LEN <= pRcvFrame->len && len >= UART_MODBUS_TCP_RTU_MIN_LEN)
                    {
                        memset(pModbusInfo, 0, sizeof(UART_MODBUS));        //清空
                        
                        pModbusInfo->head = head;
                        pModbusInfo->protocol = protocol;
                        pModbusInfo->len = len;
                        pModbusInfo->addr = addr;
                        pModbusInfo->bMbap = TRUE;      //TCP协议
                        
                        pModbusInfo->fun = pRcvFrame->buffer[pos++];
                        if(pModbusInfo->fun == UART_MODBUS_FUN_WRITE_COILS ||
                           pModbusInfo->fun == UART_MODBUS_FUN_WRITE_COIL ||
                           pModbusInfo->fun == UART_MODBUS_FUN_WRITE_HOLDREG ||
                           pModbusInfo->fun == UART_MODBUS_FUN_WRITE_HOLDREGS ||
                           pModbusInfo->fun == UART_MODBUS_FUN_READ_HOLDREGS ||
                           pModbusInfo->fun == UART_MODBUS_FUN_READ_INREGS)
                        {
                            pModbusInfo->paramsLen = pModbusInfo->len - 2;
                            memcpy(pModbusInfo->params, pRcvFrame->buffer + pos, pModbusInfo->paramsLen);
                            bOk = TRUE;
                            
                            break;
                        }
                    }
                }
                pos++;
            }
        }
    }
    else
    {
        u16 frameLen = 0;
        u8 addr = 0, fun = 0;
        while(pRcvFrame->len >= UART_MODBUS_RTU_MIN_LEN + pos)
        {
            if(pRcvFrame->buffer[pos] == devAddr || pRcvFrame->buffer[pos] == UART_MODBUS_BROADCAST_ADDR)
            {
                addr = pRcvFrame->buffer[pos + 0];
                fun = pRcvFrame->buffer[pos + 1];
                
                frameLen = 0;
                if(fun == UART_MODBUS_FUN_WRITE_COILS)
                {
                    frameLen = UART_MODBUS_REQLEN_WCOILS + pRcvFrame->buffer[pos + 6];
                }
                else if(fun == UART_MODBUS_FUN_WRITE_COIL)
                {
                    frameLen = UART_MODBUS_REQLEN_WCOIL;
                }
                else if(fun == UART_MODBUS_FUN_WRITE_HOLDREG)              //写单个寄存器
                {
                    frameLen = UART_MODBUS_REQLEN_WREG;
                }
                else if(fun == UART_MODBUS_FUN_WRITE_HOLDREGS)        //写多个寄存器
                {
                    frameLen = UART_MODBUS_REQLEN_WREGS_MIN + pRcvFrame->buffer[pos + 6];
                }
                else if(fun == UART_MODBUS_FUN_READ_HOLDREGS)         //读多个寄存器
                {
                    frameLen = UART_MODBUS_REQLEN_RREGS;
                }
                else if(fun == UART_MODBUS_FUN_READ_INREGS)           //读输入寄存器
                {
                    frameLen = UART_MODBUS_REQLEN_RINREG;
                }
            
                if(frameLen > 0 && pos + frameLen <= pRcvFrame->len)
                {
                    u16 crc1 = 0, crc2 = 0;

                    crc1 = *((u16 *)(pRcvFrame->buffer + pos + frameLen - 2));
                    crc2 = Uart_ModbusCalCrc(pRcvFrame->buffer + pos, frameLen - 2);
                    if(crc1 == crc2)
                    {
                        memset(pModbusInfo, 0, sizeof(UART_MODBUS));
                        pModbusInfo->addr = addr;
                        pModbusInfo->fun = fun;
                        pModbusInfo->paramsLen = frameLen - 4; //addr fun ----- crc1 crc2
                        memcpy(pModbusInfo->params, pRcvFrame->buffer + pos + 2, pModbusInfo->paramsLen);
                        pModbusInfo->bAsciiFrame = FALSE;
                        bOk = TRUE;
                        break;
                    }
                }
            }
            pos++;
        }
    }
    return bOk;
}
//asc不支持TCP
BOOL Uart_AnalyseASCIIModbus(UART_RCVFRAME *pRcvFrame, u8 devAddr, UART_MODBUS *pModbusInfo)
{
    BOOL bOk = FALSE;
    u16 pos = 0;
    u16 frameLen = 0, i = 0;
    u8 t = 0, lrc = 0;
    u8 addr = 0, fun = 0;
    
    while(pRcvFrame->len >= UART_MODBUS_ASCII_MIN_LEN + pos)
    {
        if(pRcvFrame->buffer[pos + 0] == UART_MODBUS_ASCII_HEAD)
        {
            Uart_GetDataFromAsic(pRcvFrame->buffer + pos + 1, addr); 
            if(addr == devAddr || addr == UART_MODBUS_BROADCAST_ADDR)
            {
                Uart_GetDataFromAsic(pRcvFrame->buffer + pos + 3, fun); 

                frameLen = 0;
                if(fun == UART_MODBUS_FUN_WRITE_COILS)
                {
                    Uart_GetDataFromAsic(pRcvFrame->buffer + pos + 13, t);
                    frameLen = UART_MODBUS_REQLEN_WCOILS + t;
                }
                else if(fun == UART_MODBUS_FUN_WRITE_COIL)
                {
                    frameLen = UART_MODBUS_REQLEN_WCOIL;
                }
                else if(fun == UART_MODBUS_FUN_WRITE_HOLDREG)              //写单个寄存器
                {
                    frameLen = UART_MODBUS_REQLEN_WREG;
                }
                else if(fun == UART_MODBUS_FUN_WRITE_HOLDREGS)        //写多个寄存器
                {
                    Uart_GetDataFromAsic(pRcvFrame->buffer + pos + 13, t);
                    frameLen = UART_MODBUS_REQLEN_WREGS_MIN + t;
                }
                else if(fun == UART_MODBUS_FUN_READ_HOLDREGS)         //读多个寄存器
                {
                    frameLen = UART_MODBUS_REQLEN_RREGS;
                }
                else if(fun == UART_MODBUS_FUN_READ_INREGS)           //读输入寄存器
                {
                    frameLen = UART_MODBUS_REQLEN_RINREG;
                }

                if(frameLen > 0)    //这里计算的是RTU格式下面的长度
                {
                    frameLen -= 2;      //去掉CRC两个字节
                    frameLen <<= 1;     //hex转成ASC，两倍长度
                    frameLen += 5;      //加上头、尾巴和LRC
                    //长度合法，结束符合法
                    if(frameLen >= UART_MODBUS_ASCII_MIN_LEN && pRcvFrame->buffer[pos + frameLen - 2] == '\r' && pRcvFrame->buffer[pos + frameLen - 1] == '\n')
                    {
                        memset(pModbusInfo, 0, sizeof(UART_MODBUS));        //初步认为是正确的请求帧了
                        
                        pModbusInfo->addr = addr;
                        pModbusInfo->fun = fun;
                        
                        lrc = 0;
                        i = 0;
                        pModbusInfo->paramsLen = 0;
                        lrc += pModbusInfo->addr;
                        lrc += pModbusInfo->fun;
                        for(i = 5; i < frameLen - 2; i += 2)    //addr和fun已经计算，直接跳过，结束符不参与计算
                        {
                            Uart_GetDataFromAsic(pRcvFrame->buffer + pos + i, t);
                            pModbusInfo->params[pModbusInfo->paramsLen] = t;
                            pModbusInfo->paramsLen++;
                            lrc += t;
                        }
                        pModbusInfo->paramsLen--;   //最后一个LRC不需要的
                        if(lrc == 0x00)
                        {
                            pModbusInfo->bAsciiFrame = TRUE;
                            bOk = TRUE;
                            break;
                        }
                    }
                }
            }
        }
        pos++;
    }
    return bOk;
}
       
u16 Uart_UsrResponseFrame(u8 *pParam, u8 len, u8 cmd, u16 addr, u8 *pBuffer)
{
    u8 pos = 0;
    u16 crc = 0;

    pBuffer[pos++] = UART_FRAME_FLAG_HEAD1;     // frame head first byte
    pBuffer[pos++] = UART_FRAME_FLAG_HEAD2;     // frame haed second byte
    pBuffer[pos++] = 0x00;                      // length
    pBuffer[pos++] = (addr >> 0) & 0xFF; 
    pBuffer[pos++] = (addr >> 8) & 0xFF;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = UART_FRAME_RESPONSE_FLAG;
    pBuffer[pos++] = cmd;                       // cmd
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;      // RFU

    memcpy(pBuffer + pos, pParam, len);
    pos += len;

    pBuffer[pos++] = UART_FRAME_FLAG_OK;
    pBuffer[pos++] = UART_FRAME_RSP_NOERR;

    pBuffer[2] = pos - 3 + 2; //减去帧头7E 55 LEN 的三个字节，加上CRC的两个字节

    crc = a_GetCrc(pBuffer + UART_FRAME_POS_LEN, pos - 2); //从LEN开始计算crc
    pBuffer[pos++] = (crc >> 0) & 0xFF;
    pBuffer[pos++] = (crc >> 8) & 0xFF;

    return pos;
}

u16 Uart_UsrRspErrorFrame(u8 errCode, u8 cmd, u16 addr, u8 *pBuffer)
{
    u8 pos = 0;
    u16 crc = 0;

    pBuffer[pos++] = UART_FRAME_FLAG_HEAD1;     // frame head first byte
    pBuffer[pos++] = UART_FRAME_FLAG_HEAD2;     // frame haed second byte
    pBuffer[pos++] = 0x00;                      // length
    pBuffer[pos++] = (addr >> 0) & 0xFF; 
    pBuffer[pos++] = (addr >> 8) & 0xFF;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = UART_FRAME_RESPONSE_FLAG;
    pBuffer[pos++] = cmd;                       // cmd
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;      // RFU

    pBuffer[pos++] = UART_FRAME_FLAG_FAIL;
    pBuffer[pos++] = errCode;

    pBuffer[2] = pos - 3 + 2; //减去帧头7E 55 LEN 的三个字节，加上CRC的两个字节

    crc = a_GetCrc(pBuffer + UART_FRAME_POS_LEN, pos - 2); //从LEN开始计算crc
    pBuffer[pos++] = (crc >> 0) & 0xFF;
    pBuffer[pos++] = (crc >> 8) & 0xFF;

    return pos;
}

u16 Uart_UsrOpTagErrorFrame(u8 errCode, u8 tagMemLen, u8 cmd, u16 addr, u8 *pBuffer)
{
    u8 pos = 0;
    u16 crc = 0;

    pBuffer[pos++] = UART_FRAME_FLAG_HEAD1;     // frame head first byte
    pBuffer[pos++] = UART_FRAME_FLAG_HEAD2;     // frame haed second byte
    pBuffer[pos++] = 0x00;                      // length
    pBuffer[pos++] = (addr >> 0) & 0xFF; 
    pBuffer[pos++] = (addr >> 8) & 0xFF;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = UART_FRAME_RESPONSE_FLAG;
    pBuffer[pos++] = cmd;                       // cmd
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;      // RFU

    memset(pBuffer, 0, tagMemLen);
    pos += tagMemLen;
    
    pBuffer[pos++] = UART_FRAME_FLAG_FAIL;
    pBuffer[pos++] = errCode;

    pBuffer[2] = pos - 3 + 2; //减去帧头7E 55 LEN 的三个字节，加上CRC的两个字节

    crc = a_GetCrc(pBuffer + UART_FRAME_POS_LEN, pos - 2); //从LEN开始计算crc
    pBuffer[pos++] = (crc >> 0) & 0xFF;
    pBuffer[pos++] = (crc >> 8) & 0xFF;

    return pos;
}

u16 Uart_UsrRspLongFrame(u16 *pParams, u16 regNum, u8 errCode, u8 cmd, u16 addr, u8 *pBuffer)
{
    u16 pos = 0;
    u16 crc = 0;
    u16 i = 0;
    
    pBuffer[pos++] = UART_FRAME_FLAG_HEAD1;     // frame head first byte
    pBuffer[pos++] = UART_FRAME_FLAG_HEAD2;     // frame haed second byte
    pBuffer[pos++] = 0x00;                      // length
    pBuffer[pos++] = (addr >> 0) & 0xFF; 
    pBuffer[pos++] = (addr >> 8) & 0xFF;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = UART_FRAME_RESPONSE_FLAG;
    pBuffer[pos++] = cmd;                       // cmd
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;      // RFU

    pBuffer[pos++] = 0x00; 
    pBuffer[pos++] = 0x00; 
    for(i = 0; i < regNum; i++)
    {
        pBuffer[pos++] = (pParams[i] >> 8) & 0xFF; 
        pBuffer[pos++] = (pParams[i] >> 0) & 0xFF; 
    }
    if(errCode == UART_FRAME_RSP_NOERR)
    {
        pBuffer[pos++] = UART_FRAME_FLAG_OK;
        pBuffer[pos++] = errCode;
    }
    else
    {
        pBuffer[pos++] = UART_FRAME_FLAG_FAIL;
        pBuffer[pos++] = errCode;
    }

    pBuffer[UART_FRAME_POS_PAR + 1 + 0] = ((pos - 12) >> 0) & 0xFF;
    pBuffer[UART_FRAME_POS_PAR + 1 + 1] = ((pos - 12) >> 8) & 0xFF;

    crc = a_GetCrc(pBuffer + UART_FRAME_POS_LEN, pos - 2); //从LEN开始计算crc
    
    pBuffer[pos++] = (crc >> 0) & 0xFF;
    pBuffer[pos++] = (crc >> 8) & 0xFF;

    return pos;
}

BOOL Uart_UsrCheckFrame(UART_RCVFRAME *pRcvFrame, u16 *pStartPos, u16 addr)
{
    BOOL b = FALSE;
    u16 pos = 0;

    if(pRcvFrame->len >= UART_FRAME_MIN_LEN)
    {
        while(pos + UART_FRAME_MIN_LEN <= pRcvFrame->len)
        {
            if(pRcvFrame->buffer[pos + 0] == UART_FRAME_FLAG_HEAD1 && pRcvFrame->buffer[pos + 1] == UART_FRAME_FLAG_HEAD2)
            {
                u16 frameLen = 0;
                u16 crc1 = 0, crc2 = 0;
                if(pRcvFrame->buffer[pos + UART_FRAME_POS_LEN])
                {
                    frameLen = pRcvFrame->buffer[pos + 2] + 3;
                }
                else
                {
                    frameLen = pRcvFrame->buffer[pos + UART_FRAME_POS_PAR + 1];
                    frameLen <<= 8;
                    frameLen |= pRcvFrame->buffer[pos + UART_FRAME_POS_PAR + 0];

                    frameLen += UART_FRAME_MIN_LEN + 2;
                }
                if(frameLen >= UART_FRAME_MIN_LEN)
                {
                    crc1 = a_GetCrc(pRcvFrame->buffer + pos + 2, frameLen - 4);
                    crc2 = pRcvFrame->buffer[pos + frameLen - 1];
                    crc2 <<= 8;
                    crc2 |= pRcvFrame->buffer[pos + frameLen - 2];
                    if(crc1 == crc2)
                    {
                        u16 destAddr = 0;
                        
                        destAddr = *((u16 *)(pRcvFrame->buffer + pos + UART_FRAME_POS_DESTADDR));
                        
                        if(destAddr == UART_USR_ADDR_BROADCAST || destAddr == addr)
                        {
                            *pStartPos = pos;
                            b = TRUE;
                            break;
                        }
                    }
                }
            }
            pos++;
        }
    }

    return b;
}