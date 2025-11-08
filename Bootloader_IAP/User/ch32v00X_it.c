/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v00X_it.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/11/04
 * Description        : Main Interrupt Service Routines.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include <ch32v00X_it.h>

void NMI_Handler (void) __attribute__ ((interrupt)); 
void HardFault_Handler (void) __attribute__ ((interrupt));
void SW_Handler (void) __attribute__ ((interrupt));

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler (void) {
    while (1) {
    }
}

/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler (void) {
    
    printf("HardFault_Handler\r\n");
    while (1) {
    }
}

void SW_Handler (void) {

    __asm ("li  a2, 0x08002000");
    __asm ("jr  a2");

}
