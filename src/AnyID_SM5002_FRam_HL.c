#include "AnyID_SM5002_FRam_HL.h"

const PORT_INF FRAM_SDA_PORT = {GPIOB, GPIO_Pin_7};
const PORT_INF FRAM_SCL_PORT = {GPIOB, GPIO_Pin_6};
void FRam_InitInterface(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;

    GPIO_InitStructure.GPIO_Pin = FRAM_SDA_PORT.Pin;
    GPIO_Init(FRAM_SDA_PORT.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = FRAM_SCL_PORT.Pin;
    GPIO_Init(FRAM_SCL_PORT.Port, &GPIO_InitStructure);
    
    FRam_I2CStop();         //����һ��STOPָ��
}

void FRam_Delayms(u32 n)             //ϵͳ��ʱn����
{
    n *= 0x6000;
    n++;
    while(n--);
}

void FRam_I2CDelay(void)
{
    u32 i = 0;

    i = 100;
    while(i--);
}
BOOL FRam_I2CStart(void)
{
    FRAM_I2C_SDA_H;
    FRAM_I2C_SCL_H;
    FRam_I2CDelay();
    if(!FRam_I2CGetSda())return FALSE;    //SDA��Ϊ�͵�ƽ������æ,�˳�
    FRAM_I2C_SDA_L;
    FRam_I2CDelay();
    if(FRam_I2CGetSda()) return FALSE;    //SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
    FRAM_I2C_SDA_L;
    FRam_I2CDelay();
    return TRUE;
}

void FRam_I2CStop(void)
{
    FRAM_I2C_SCL_L;
    FRam_I2CDelay();
    FRAM_I2C_SDA_L;
    FRam_I2CDelay();
    FRAM_I2C_SCL_H;
    FRam_I2CDelay();
    FRAM_I2C_SDA_H;
    FRam_I2CDelay();
}

void FRam_I2CAck(void)
{
    FRAM_I2C_SCL_L;
    FRam_I2CDelay();
    FRAM_I2C_SDA_L;
    FRam_I2CDelay();
    FRAM_I2C_SCL_H;
    FRam_I2CDelay();
    FRAM_I2C_SCL_L;
    FRam_I2CDelay();
}

void FRam_I2CNoAck(void)
{
    FRAM_I2C_SCL_L;
    FRam_I2CDelay();
    FRAM_I2C_SDA_H;
    FRam_I2CDelay();
    FRAM_I2C_SCL_H;
    FRam_I2CDelay();
    FRAM_I2C_SCL_L;
    FRam_I2CDelay();
}

BOOL FRam_I2CWaitAck(void)      //����Ϊ:=1��ACK,=0��ACK
{
    FRAM_I2C_SCL_L;
    FRam_I2CDelay();
    FRAM_I2C_SDA_H;
    FRam_I2CDelay();
    FRAM_I2C_SCL_H;
    FRam_I2CDelay();
    if(FRam_I2CGetSda())
    {
        FRAM_I2C_SCL_L;
        return FALSE;
    }
    FRAM_I2C_SCL_L;
    return TRUE;
}

void FRam_I2CSendByte(u8 SendByte) //���ݴӸ�λ����λ//
{
    u8 i = 8;
    while(i--)
    {
        FRAM_I2C_SCL_L;
        if(SendByte&0x80)
        {
            FRAM_I2C_SDA_H;
        }
        else
        {
            FRAM_I2C_SDA_L;
        }
        FRam_I2CDelay();
        
        FRAM_I2C_SCL_H;
        FRam_I2CDelay();
        
        SendByte <<= 1;
    }
    FRAM_I2C_SCL_L;
}

u8 FRam_I2CReceiveByte(void)  //���ݴӸ�λ����λ
{
    u8 i = 8;
    u8 ReceiveByte = 0;

    FRAM_I2C_SDA_H;
    while(i--)
    {
        ReceiveByte <<= 1;
        
        FRAM_I2C_SCL_L;
        FRam_I2CDelay();
        FRAM_I2C_SCL_H;
        FRam_I2CDelay();
        if(FRam_I2CGetSda())
        {
            ReceiveByte |= 0x01;
        }
    }
    FRAM_I2C_SCL_L;
    return ReceiveByte;
}