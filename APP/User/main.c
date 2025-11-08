/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/01/01
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *Multiprocessor communication mode routine:
 *Master:USART1_Tx(PD5)\USART1_Rx(PD6).
 *This routine demonstrates that USART1 receives the data sent by CH341 and inverts
 *it and sends it (baud rate 115200).
 *
 *Hardware connection:PD5 -- Rx
 *                    PD6 -- Tx
 *
 */

#include "main.h"

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main (void) {
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init (115200);
#endif
    loge ("SystemClk:%d\r\n", SystemCoreClock);
    loge ("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

    BSP_ADC_Init();
    BSP_DAC_Init();
    BSP_Encoder_Init();
    BSP_Key_Init();
    BSP_LED_Init();
    BSP_TIM_Init();
    BSP_Display_Init();
    BSP_Display_CMD ("page page0");
    // BSP_WS2812_Init();

    Delay_Ms (500);
    BSP_LED_Breathe (ENABLE);

    while (1) {

        // BSP_ADC_Update_Injected_Data();
        // float temp = NTC_GetTemperature (
        //     ADC_Regular_Data[1],
        //     4095,      // 12位ADC
        //     10000.0f,  // 上拉电阻10kΩ
        //     10000.0f,  // NTC 25℃时10kΩ
        //     298.15f,   // 25℃
        //     3380.0f    // B值3950
        // );

        // loge ("Temperature: %.2f C\r\n ", temp);
        // loge ("Iout %.2f \r\n", ADC_Injected_Data[0] / 4095.0f * VDD_Cal);
        // loge ("Vout %.2f \r\n", ADC_Injected_Data[1] / 4095.0f * VDD_Cal * 7.98f);
        // loge ("Vin: %.2f\r\n ", ADC_Regular_Data[0] / 4095.0f * VDD_Cal * 16);
        // loge ("VDD %.2f \r\n", VDD_Cal);
        // loge ("cnt %u \r\n", BSP_Encoder_Get_Cnt());

        // BSP_DAC_Set_CCR (BSP_Encoder_Get_Cnt());
        Delay_Ms (100);
    }
}

void BSP_TIM_Update_IQR_Callback (void) {
    BSP_Key_Scan();
    BSP_Key_Task();
}

void BSP_Key_Task (void) {
    if (Key_Encoder.pressed_flag == 1) {
        static u8 state = 1;
        state = !state;
        if (state) {
            TIM1->PSC = 4800 - 1;
            Load_Switch (DISABLE);
        } else {
            TIM1->PSC = 1200 - 1;
            Load_Switch (ENABLE);
        }

        Key_Encoder.pressed_flag = 0;
    }
}

void Load_Switch (FunctionalState state) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;

    if (state) {
        GPIO_SetBits (GPIOC, GPIO_Pin_3);
        GPIO_Init (GPIOC, &GPIO_InitStructure);
    } else {
        GPIO_ResetBits (GPIOC, GPIO_Pin_3);
        GPIO_Init (GPIOC, &GPIO_InitStructure);
    }
}