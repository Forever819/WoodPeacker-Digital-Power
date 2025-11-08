#include "debug.h"
#include "ADC_k.h"
#include <math.h>
#include <stdint.h>

#define ADC_RegularGroup_Data_Size 2
#define ADC_InjectedGroup_Data_Size 3
volatile uint16_t ADC_Regular_Data[ADC_RegularGroup_Data_Size];
volatile uint16_t ADC_Injected_Data[ADC_InjectedGroup_Data_Size];
volatile float VDD_Cal = 3.3f;

void BSP_ADC_Init (void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    ADC_InitTypeDef ADC_InitStructure = {0};
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOA | RCC_PB2Periph_GPIOD, ENABLE);
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_ADC1, ENABLE);
    RCC_HBPeriphClockCmd (RCC_HBPeriph_DMA1, ENABLE);
    RCC_ADCCLKConfig (RCC_PCLK2_Div4);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init (GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_Init (GPIOD, &GPIO_InitStructure);

    ADC_DeInit (ADC1);
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = ADC_RegularGroup_Data_Size;
    // ADC_ITConfig (ADC1, ADC_IT_JEOC, ENABLE);
    // NVIC_InitTypeDef NVIC_InitStructure = {0};
    // NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    // NVIC_Init (&NVIC_InitStructure);


    ADC_Init (ADC1, &ADC_InitStructure);

    ADC_DMACmd (ADC1, ENABLE);
    ADC_BufferCmd (ADC1, ENABLE);  // disable buffer
    ADC_Cmd (ADC1, ENABLE);

    DMA_DeInit (DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->RDATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_Regular_Data;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = ADC_RegularGroup_Data_Size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init (DMA1_Channel1, &DMA_InitStructure);

    DMA_Cmd (DMA1_Channel1, ENABLE);
    // RegularGroup
    ADC_RegularChannelConfig (ADC1, ADC_Channel_1, 1, ADC_SampleTime_CyclesMode3);  // Vin
    ADC_RegularChannelConfig (ADC1, ADC_Channel_0, 2, ADC_SampleTime_CyclesMode3);  // NTC
    // InjectionGroup
    ADC_ExternalTrigInjectedConvConfig (ADC1, ADC_ExternalTrigInjecConv_None);
    ADC_InjectedSequencerLengthConfig (ADC1, 3);
    ADC_InjectedChannelConfig (ADC1, ADC_Channel_6, 1, ADC_SampleTime_CyclesMode7);  // Iout
    ADC_InjectedChannelConfig (ADC1, ADC_Channel_5, 2, ADC_SampleTime_CyclesMode7);  // Vout
    ADC_InjectedChannelConfig (ADC1, ADC_Channel_8, 3, ADC_SampleTime_CyclesMode7);  // Vref

    ADC_SoftwareStartConvCmd (ADC1, ENABLE);
}

void BSP_ADC_Update_Injected_Data (void) {
    ADC_SoftwareStartInjectedConvCmd (ADC1, ENABLE);
    while (ADC_GetFlagStatus (ADC1, ADC_FLAG_JEOC) != SET);
    ADC_Injected_Data[0] = ADC_GetInjectedConversionValue (ADC1, ADC_InjectedChannel_1);
    ADC_Injected_Data[1] = ADC_GetInjectedConversionValue (ADC1, ADC_InjectedChannel_2);
    ADC_Injected_Data[2] = ADC_GetInjectedConversionValue (ADC1, ADC_InjectedChannel_3);
    VDD_Cal = 1.22f * 4095 / ADC_Injected_Data[2];

}

float BSP_ADC_Get_VDD (void) {
    return VDD_Cal;
}

/**
 * @brief  根据 ADC 值计算温度（℃）
 * @param  adc_val       : ADC采样值 (0~4095)
 * @param  adc_max       : ADC最大值 (例如12位ADC为4095)
 * @param  vref          : 参考电压(V)，如3.3
 * @param  r_pullup      : 上拉电阻阻值(Ω)，例如10k
 * @param  r0            : NTC在T0时阻值(Ω)，例如10k
 * @param  t0            : NTC标称温度(K)，通常为298.15K (25℃)
 * @param  b_value       : NTC的B参数 (K)，例如3950
 * @return 温度值（单位：摄氏度）
 */
float NTC_GetTemperature (uint16_t adc_val, uint16_t adc_max,
                          float r_pullup, float r0, float t0, float b_value) {
    if (adc_val == 0 || adc_val >= adc_max)
        return -273.15f;  // 避免除零错误
    // 计算NTC电阻
    float v_adc = (float)adc_val / adc_max * 3.3f;
    float r_ntc = r_pullup * v_adc / (3.3f - v_adc);

    // 根据B值公式计算温度(K)
    float t = 1.0f / (1.0f / t0 + (1.0f / b_value) * logf (r_ntc / r0));

    // 转换为摄氏度
    return t - 273.15f;
}

// void ADC1_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));

// void ADC1_IRQHandler (void) {
//     if (ADC_GetITStatus (ADC1, ADC_IT_JEOC) != RESET) {
//         ADC_ClearITPendingBit (ADC1, ADC_IT_JEOC);


//  }
// }
