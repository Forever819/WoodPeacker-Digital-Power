#include "debug.h"
#include "Key_k.h"

Key_t Key_Encoder = {
    .GPIOX = GPIOD,
    .GPIO_Pin = GPIO_Pin_0,
    };

static void Key_Init (Key_t *Key) {
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOD, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_InitStructure.GPIO_Pin = Key->GPIO_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init (Key->GPIOX, &GPIO_InitStructure);

    // 初始化状态
    Key->cnt = 0;
    Key->state = Key_Idle;
    Key->pressed_flag = 0;
}

static void Key_Scan (Key_t *Key) {
    uint8_t key_val = GPIO_ReadInputDataBit (Key->GPIOX, Key->GPIO_Pin);

    switch (Key->state) {
    case Key_Idle:
        if (key_val == Bit_RESET)  // 检测到按下
        {
            Key->cnt++;
            if (Key->cnt > Key_Valid_CNT)  // 去抖：连续按下5次
            {
                Key->state = Key_Pressed;
                Key->cnt = 0;
            }
        } else {
            Key->cnt = 0;
        }
        break;

    case Key_Pressed:
        if (key_val == Bit_SET)  // 松开检测
        {
            Key->cnt++;
            if (Key->cnt > Key_Valid_CNT)  // 去抖
            {
                Key->state = Key_Released;
                Key->cnt = 0;
                Key->pressed_flag = 1;  // 表示按键有效触发
                
            }
        } else {
            Key->cnt = 0;
        }
        break;

    case Key_Released:
        if (key_val == Bit_SET) {
            Key->state = Key_Idle;  // 回到空闲
        }
        break;
    }
}

void BSP_Key_Init(void)
{
    Key_Init(&Key_Encoder);
}

void BSP_Key_Scan(void)
{
    Key_Scan(&Key_Encoder);
}



