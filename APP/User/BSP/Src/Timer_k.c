#include "debug.h"
#include "Timer_k.h"


void BSP_TIM_Init (void) {
    
    //¸´ÓÃLEDµÄTIM1
    TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
    loge("Using led timer(tim1) isr\r\n");
    loge("Running at freq %d Hz\r\n",SystemCoreClock/((TIM1->ATRLR+1)*(TIM1->PSC+1)));
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void TIM1_UP_IRQHandler(void)
{
    TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
    BSP_TIM_Update_IQR_Callback();
}