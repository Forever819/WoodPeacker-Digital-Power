#include "debug.h"

void BSP_Display_Init (void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOD | RCC_PB2Periph_USART2 | RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig (GPIO_PartialRemap3_USART2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;

    USART_Init (USART2, &USART_InitStructure);
    USART_Cmd (USART2, ENABLE);
}

void BSP_Display_CMD (char *string) {
    while (*string) {
        while (USART_GetFlagStatus (USART2, USART_FLAG_TXE) == RESET);
        USART_SendData (USART2, *string);
        string++;
    }
    for (uint8_t i = 0; i < 3; i++) {
        while (USART_GetFlagStatus (USART2, USART_FLAG_TXE) == RESET);
        USART_SendData (USART2, 0xff);
    }
}