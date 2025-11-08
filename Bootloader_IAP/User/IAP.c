#include "debug.h"
#include "IAP.h"
#include "string.h"

#define IAP_BUFFER_SIZE 4096
#define IAP_BIN_MAX_SIZE 27648

#define IAP_INFO_ADDRESS 0x8001F00
#define IAP_ADDRESS_A 0x08002000
#define IAP_ADDRESS_B (0x08002000 + IAP_BIN_MAX_SIZE)

IAP_INFO_u IAP_info_g;
u8 iap_buf_g[IAP_BUFFER_SIZE];

void IAP2APP (void) {
    // NVIC_EnableIRQ(Software_IRQn);
    // NVIC_SetPendingIRQ(Software_IRQn);
    __asm ("li  a5, 0x08002000");
    __asm ("jr  a5");
}

void IAP_Send_Strings (char *s) {
    while (*s) {
        while (USART_GetFlagStatus (USART1, USART_FLAG_TXE) == RESET);
        USART_SendData (USART1, *s);
        s++;
    }
}

void IAP_Key_Init (void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init (GPIOD, &GPIO_InitStructure);
}

void IAP_Init (u32 baudrate) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOC | RCC_PB2Periph_USART1 | RCC_PB2Periph_AFIO, ENABLE);
    RCC_HBPeriphClockCmd (RCC_HBPeriph_DMA1, ENABLE);
    GPIO_PinRemapConfig (GPIO_PartialRemap6_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init (GPIOC, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;


    USART_Init (USART1, &USART_InitStructure);
    USART_Cmd (USART1, ENABLE);

    USART_ITConfig (USART1, USART_IT_RXNE, ENABLE);
    NVIC_EnableIRQ (USART1_IRQn);

    DMA_InitStructure.DMA_BufferSize = IAP_BUFFER_SIZE;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)iap_buf_g;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DATAR;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

    DMA_Init (DMA1_Channel5, &DMA_InitStructure);
    DMA_Cmd (DMA1_Channel5, ENABLE);

    DMA_ITConfig (DMA1_Channel5, DMA_IT_HT, ENABLE);
    DMA_ITConfig (DMA1_Channel5, DMA_IT_TC, ENABLE);
    NVIC_EnableIRQ (DMA1_Channel5_IRQn);

    // get iap info
    IAP_Get_Flash_IAP_INFO();
    // init iap key
    IAP_Key_Init();
    if (GPIO_ReadInputDataBit (GPIOD, GPIO_Pin_0) == Bit_RESET) {
        if (IAP_info_g.OTA_Flag == 0)
            IAP_info_g.OTA_Flag = 1;
    }
}

void USART1_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));
void DMA1_Channel5_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));

void USART1_IRQHandler (void) {
    static u8 cnt = 0;
    IAP_info_g.bin_size_raw[cnt] = USART_ReceiveData (USART1);  // 读操作可以清空rxne it
    if (++cnt >= 4) {
        NVIC_DisableIRQ (USART1_IRQn);
        USART_DMACmd (USART1, USART_DMAReq_Rx, ENABLE);
        IAP_info_g.bin_size = IAP_info_g.bin_size_raw[0] << 24 | IAP_info_g.bin_size_raw[1] << 16 | IAP_info_g.bin_size_raw[2] << 8 | IAP_info_g.bin_size_raw[3];
        printf ("size %d\r\n", IAP_info_g.bin_size);
        // iap start
        // erase start
        if (FLASH_ROM_ERASE (0x8002000, 0x6000) == FLASH_COMPLETE)
            IAP_Send_Strings ("erase complete\r\n");
    }
}

void DMA1_Channel5_IRQHandler (void) {
    static u8 location_flag = 0;
    static FLASH_Status s = FLASH_ERROR_PG;
    if (DMA_GetITStatus (DMA1_IT_TC5) == SET) {
        DMA_ClearITPendingBit (DMA1_IT_TC5);
        s = FLASH_ROM_WRITE (0x8002000 + IAP_info_g.bin_size_rec, (u32 *)&iap_buf_g[IAP_BUFFER_SIZE / 2], IAP_BUFFER_SIZE / 2);
        IAP_info_g.bin_size_rec += IAP_BUFFER_SIZE / 2;
        if (s == FLASH_COMPLETE)
            printf ("tc5 %d\r\n", IAP_info_g.bin_size_rec);
        location_flag = 0;
    } else if (DMA_GetITStatus (DMA1_IT_HT5)) {
        DMA_ClearITPendingBit (DMA1_IT_HT5);
        s = FLASH_ROM_WRITE (0x8002000 + IAP_info_g.bin_size_rec, (u32 *)&iap_buf_g[0], IAP_BUFFER_SIZE / 2);
        IAP_info_g.bin_size_rec += IAP_BUFFER_SIZE / 2;
        if (s == FLASH_COMPLETE)
            printf ("ht5 %d\r\n", IAP_info_g.bin_size_rec);
        location_flag = 1;
    }
    if (IAP_info_g.bin_size - IAP_info_g.bin_size_rec < IAP_BUFFER_SIZE / 2) {

        printf ("left %d\r\n", IAP_info_g.bin_size - IAP_info_g.bin_size_rec);

        // deal with left bin data
        Delay_Ms (1000);
        s = FLASH_ROM_WRITE (0x8002000 + IAP_info_g.bin_size_rec, location_flag ? (u32 *)&iap_buf_g[IAP_BUFFER_SIZE / 2] : (u32 *)&iap_buf_g[0], IAP_BUFFER_SIZE / 2);

        if (s == FLASH_COMPLETE) {
            IAP_Send_Strings ("iap complete,jumping...\r\n");
            IAP2APP();
        } else {
            printf ("%d", s);
        }
    }
}

void IAP_Loop (void) {
    switch (IAP_info_g.OTA_Flag) {
    }
    Delay_Ms (1000);
}


u8 IAP_Get_Flash_IAP_INFO (void) {
    memcpy (IAP_info_g.buf, (u32 *)IAP_INFO_ADDRESS, 256);
    // printf ("OTA_Flag %d\r\n", IAP_info_g.OTA_Flag);
    return 0;
}

u8 IAP_Update_IAP_INFO (void) {
    IAP_info_g.OTA_Flag = 5;
    FLASH_Status s;
    s = FLASH_ROM_ERASE (IAP_INFO_ADDRESS, 256);
    if (s != FLASH_COMPLETE) {
        return 1;
    } else {
        s = FLASH_ROM_WRITE (IAP_INFO_ADDRESS, (u32 *)IAP_info_g.buf, 256);
    }
    if (s != FLASH_COMPLETE) {
        return 2;
    }
    return 0;
}
