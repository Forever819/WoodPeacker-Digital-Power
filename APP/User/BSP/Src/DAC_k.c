#include "debug.h"
#include "DAC_k.h"

void BSP_DAC_Init (void) {
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOC, ENABLE);
    RCC_PB1PeriphClockCmd (RCC_PB1Periph_TIM2, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init (GPIOC, &GPIO_InitStructure);
    GPIO_PinRemapConfig(GPIO_PartialRemap3_TIM2,ENABLE);

    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
    TIM_TimeBaseInitStructure.TIM_Period = 4096 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;
    TIM_TimeBaseInit (TIM2, &TIM_TimeBaseInitStructure);

    TIM_OCStructInit (&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 1024 - 1;
    TIM_OC2Init (TIM2, &TIM_OCInitStructure);

    TIM_OC2PreloadConfig (TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig (TIM2, ENABLE);

    TIM_Cmd (TIM2, ENABLE);
    TIM_CtrlPWMOutputs (TIM2, ENABLE);
}

void BSP_DAC_Set_CCR(uint16_t ccr)
{
    TIM_SetCompare2(TIM2,ccr);
}
