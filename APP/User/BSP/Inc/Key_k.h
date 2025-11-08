#define Key_Valid_CNT 5

typedef enum {
    Key_Idle = 0,
    Key_Pressed,
    Key_Released
} Key_State;

typedef struct {
    GPIO_TypeDef *GPIOX;
    uint16_t GPIO_Pin;

    uint16_t cnt;
    Key_State state;
    uint8_t pressed_flag;
} Key_t;

void BSP_Key_Init (void);
void BSP_Key_Scan (void);
void BSP_Key_Task (void);

extern Key_t Key_Encoder;
