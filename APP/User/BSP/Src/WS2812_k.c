#include "debug.h"
#include "WS2812_k.h"

void BSP_WS2812_Init (void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef SPI_InitStructure = {0};

    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOC, ENABLE);
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_SPI1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init (GPIOC, &GPIO_InitStructure);
    GPIO_PinRemapConfig (GPIO_PartialRemap3_SPI1, ENABLE);

    // ws2812要求单位时间0.25us+-0.15 2.5M~10Mhz
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;  // 6mhz 一个位0.16us
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_Init (SPI1, &SPI_InitStructure);
    SPI_Cmd (SPI1, ENABLE);

    // RESET
    for (uint16_t i = 0; i < 5000; i++) {
        while (SPI_I2S_GetFlagStatus (SPI1, SPI_I2S_FLAG_TXE) == RESET);
        SPI_I2S_SendData (SPI1, 0x00);
    }
   
}
#define SPI_BITS_PER_CODE   7        // 每个 WS2812 bit 使用 7 个 SPI bit
#define SPI_HIGH_FOR_1      6        // 1码：高5位
#define SPI_HIGH_FOR_0      1        // 0码：高2位

uint32_t Data_proc(uint32_t *color_data, uint16_t color_data_size, uint8_t *data2send) {
    uint32_t data2send_size = 0;
    uint8_t temp2send = 0;
    uint8_t cnt = 0;

    for (uint16_t i = 0; i < color_data_size; i++) {
        uint32_t color = *color_data++;  // GRB顺序：G(31:24), R(23:16), B(15:8)

        for (uint8_t bit = 0; bit < 24; bit++) {
            uint8_t high_bits = (color & 0x800000UL) ? SPI_HIGH_FOR_1 : SPI_HIGH_FOR_0;
            uint8_t total_bits = SPI_BITS_PER_CODE;

            for (uint8_t k = 0; k < total_bits; k++) {
                if (k < high_bits) {
                    temp2send |= 0x01;   // 高电平
                }
                // else: 低电平 (保持0)

                temp2send <<= 1;
                cnt++;

                if (cnt == 8) {
                    *data2send++ = temp2send;
                    data2send_size++;
                    temp2send = 0;
                    cnt = 0;
                }
            }
            color <<= 1;
        }
    }

    // 处理剩余未满8位的字节（补0）
    if (cnt > 0) {
        temp2send <<= (8 - cnt);
        *data2send++ = temp2send;
        data2send_size++;
    }

    return data2send_size;
}

