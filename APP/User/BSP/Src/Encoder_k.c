#include "debug.h"
#include "Encoder_k.h"

static volatile int16_t encoder_cnt = 2048;

void BSP_Encoder_Init() {
#if (ENCODER_MODE == ENCODER_EXTI_MODE)

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_PB2PeriphClockCmd (RCC_PB2Periph_AFIO | RCC_PB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init (GPIOC, &GPIO_InitStructure);

    GPIO_EXTILineConfig (GPIO_PortSourceGPIOC, GPIO_PinSource1);  // gpio组通过afio选择line0-7所以要使能afio
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_Init (&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init (&NVIC_InitStructure);

#elif (ENCODER_MODE == ENCODER_TIMER_MODE)

#endif
}

#if (ENCODER_MODE == ENCODER_EXTI_MODE)
void EXTI7_0_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));

void EXTI7_0_IRQHandler (void) {
    if (EXTI_GetITStatus (EXTI_Line1) != RESET) {
        EXTI_ClearITPendingBit (EXTI_Line1);
        if (GPIO_ReadInputDataBit (GPIOC, GPIO_Pin_2) == Bit_SET)
            encoder_cnt++;
        else
            encoder_cnt--;

        if (encoder_cnt > 4096)
            encoder_cnt = 4096;
        else if (encoder_cnt < 0)
            encoder_cnt = 0;
    }
}

int16_t BSP_Encoder_Get_Cnt (void) {
    return encoder_cnt;
}

#elif (ENCODER_MODE == ENCODER_TIMER_MODE)

#endif