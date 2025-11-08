#ifndef ADC_K_H
float NTC_GetTemperature (uint16_t adc_val, uint16_t adc_max, float r_pullup, float r0, float t0, float b_value);
void BSP_ADC_Init (void);
float BSP_ADC_Get_VDD (void);
void BSP_ADC_Update_Injected_Data(void);

extern volatile uint16_t ADC_Regular_Data[];
extern volatile uint16_t ADC_Injected_Data[];
extern volatile float VDD_Cal;

#define ADC_K_H
#endif