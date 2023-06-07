#include "AnyID_SM5002_AD_HL.h"

u16 g_aAdBuffer[AD_CHANNEL_NUM] = {0};

const PORT_INF AD_TEMP1_COM = {GPIOA, GPIO_Pin_0};
const PORT_INF AD_TEMP2_COM = {GPIOA, GPIO_Pin_1};
void AD_InitInterface(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = AD_TEMP1_COM.Pin;
    GPIO_Init(AD_TEMP1_COM.Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = AD_TEMP2_COM.Pin;
    GPIO_Init(AD_TEMP2_COM.Port, &GPIO_InitStructure);
}

void AD_Init(void)
{
    //һ��5ms�ɼ�һ�μ���
    ADC_InitTypeDef ADC_InitStructure;

    ADC_DeInit(AD_PORT);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                      //��������ģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;                           //ɨ�跽ʽ
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                     //����ת��
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;     //�ⲿ������ֹ
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                  //�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = AD_CHANNEL_NUM;                    //����ת����ͨ����
    ADC_Init(AD_PORT, &ADC_InitStructure);

    ADC_RegularChannelConfig(AD_PORT, AD_TEMP1_CHANNEL, 1, AD_TIME);
    ADC_RegularChannelConfig(AD_PORT, AD_TEMP2_CHANNEL, 2, AD_TIME);
     
    ADC_Cmd(AD_PORT, ENABLE);
    
    ADC_DMACmd(AD_PORT, ENABLE);

    //У׼
    ADC_ResetCalibration(AD_PORT);
    while(ADC_GetResetCalibrationStatus(AD_PORT));
    ADC_StartCalibration(AD_PORT);
    while(ADC_GetCalibrationStatus(AD_PORT));
    
    DMA_Cmd(AD_DMA_CHANNEL, ENABLE);
    ADC_Cmd(AD_PORT, ENABLE);
    AD_Start(); 
}

void AD_DmaInit(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit(AD_DMA_CHANNEL);												//��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(AD_PORT->DR));		//DMA����ADC����ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)g_aAdBuffer;		        //DMA�ڴ����ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;						//�ڴ���Ϊ���ݴ����Ŀ�ĵ�
    DMA_InitStructure.DMA_BufferSize = AD_CHANNEL_NUM;				        //DMAͨ����DMA����Ĵ�С
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//�����ַ����
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				    //�ڴ��ַ����
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//DMAͨ����DMA����Ĵ�С
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//DMAͨ����DMA����Ĵ�С
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		                        //������ѭ������ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;	                        //DMAͨ�� xӵ�и����ȼ�
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;			                    //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
    
    DMA_Init(AD_DMA_CHANNEL, &DMA_InitStructure);			                    //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��
    DMA_Cmd(AD_DMA_CHANNEL, ENABLE);
}


