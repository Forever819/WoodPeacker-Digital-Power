#include "debug.h"
#include <core_riscv.h>
#include "SerialTFT_k.h"
#include "IAP.h"
// 要求7kb以内


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
    BSP_Display_Init();
    IAP_Init(115200);
    printf("bootloader iap\r\n");
    // BSP_Display_CMD("page page1");
    // BSP_Display_CMD("p0.pic=5");//ready
    // BSP_Display_CMD ("t1.txt=\"1.1.0\"");
    // BSP_Display_CMD("p0.pic=4");//failed
    // BSP_Display_CMD("p0.pic=3");//downloading
    // BSP_Display_CMD("p0.pic=2");//compelete

    // printf ("BL:BootLoader Complete! Run to APP...\r\n\r\n");
    // u32 mtvec = __get_MTVEC();
    // printf("mtvec in bootloader %x\r\n",mtvec);
    // IAP2APP();
    
    while (1)
    {
        IAP_Loop();
    }
}



