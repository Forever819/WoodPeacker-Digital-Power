#include "debug.h"
#include "LED_k.h"

void BSP_LED_Init (void) {
#if (LED_MODE == LED_GPIO_MODE)

    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init (GPIOC, &GPIO_InitStructure);

#elif (LED_MODE == LED_TIMER_MODE)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOC | RCC_PB2Periph_TIM1, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init (GPIOC, &GPIO_InitStructure);

    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV4;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
    TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 4800 - 1;  // 100Hz相当于一个ccr持续10ms
    TIM_TimeBaseInit (TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCStructInit (&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = (100 - 1) / 2;
    TIM_OC4Init (TIM1, &TIM_OCInitStructure);

    TIM_OC4PreloadConfig (TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig (TIM1, ENABLE);

    TIM_Cmd (TIM1, ENABLE);
    TIM_CtrlPWMOutputs (TIM1, ENABLE);
#endif
}

#if (LED_MODE == LED_TIMER_MODE)
//Timer模式下独有函数
uint16_t ccr_value[200];

void BSP_LED_Breathe (FunctionalState state) {
    if (state == ENABLE) {
        for (uint16_t i = 0; i < 100; i++) {
            ccr_value[i] = i;
        }
        for (uint16_t i = 100; i < 200; i++) {
            ccr_value[i] = 199 - i;
        }
        // BSP_LED_Init();
        RCC_HBPeriphClockCmd (RCC_HBPeriph_DMA1, ENABLE);

        DMA_InitTypeDef DMA_InitStructure = {0};
        DMA_DeInit (DMA1_Channel5);

        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&TIM1->CH4CVR;
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ccr_value;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        DMA_InitStructure.DMA_BufferSize = 200;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init (DMA1_Channel5, &DMA_InitStructure);

        // TIM_DMACmd (TIM1, TIM_DMA_CC4, ENABLE);
        TIM_DMACmd (TIM1, TIM_DMA_Update, ENABLE);
        DMA_Cmd (DMA1_Channel5, ENABLE);

    } else {
        // TIM_ForcedOC4Config(TIM1,TIM_ForcedAction_Active);
        DMA_Cmd (DMA1_Channel5, DISABLE);
    }
}

void BSP_LED_SetBrigtness (uint16_t ccr) {
    BSP_LED_Breathe(DISABLE);
    TIM_SetCompare4 (TIM1, ccr);
}

#elif (if (LED_MODE == LED_GPIO_MODE))
//GPIO模式下独有函数
void LED_Toggle (void) {

    if (GPIO_ReadOutputDataBit (GPIOC, GPIO_Pin_4) == Bit_SET) {
        GPIO_ResetBits (GPIOC, GPIO_Pin_4);
    } else {
        GPIO_SetBits (GPIOC, GPIO_Pin_4);
    }
}
#endif