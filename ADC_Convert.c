#include "stm32f10x.h"                  // Device header
#include "Delay.h"
//ADC单通道转换
uint16_t convertValueArr[3] = {0x00};
void ADC_DMAConfig(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	DMA_InitTypeDef DMA_InitStruct;
	
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)convertValueArr;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_BufferSize = 3;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel1,&DMA_InitStruct);
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
	
}

void ADD_ConvertInit(void){
	//开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//GPIO----ADC_CHO-->PAO
	GPIO_InitTypeDef gpioStruct;
	gpioStruct.GPIO_Mode = GPIO_Mode_AIN;//模拟输入
	gpioStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	gpioStruct.GPIO_Speed = GPIO_Speed_50MHz;//电平转换速度可以达到50MHz
	GPIO_Init(GPIOA,&gpioStruct);
	//ADCCLK配置号--采样+转换的时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	//规则通道
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_13Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_13Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,3,ADC_SampleTime_13Cycles5);
	
	//ADC结构体
	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//只有ADC1参与
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//software--自己写的代码转换
	ADC_InitStruct.ADC_NbrOfChannel = 3;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//EDC转换完成后，是否接着进行下一次转换
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//有多个通道才需要扫描
	ADC_Init(ADC1,&ADC_InitStruct);
	//DMA+使能ADC的DMA的请求功能
	ADC_DMACmd(ADC1,ENABLE);
	//启动ADC
	ADC_Cmd(ADC1,ENABLE);
	delayByUs(1);
	
	//校准
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1) == SET);
	
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) == SET);
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}






