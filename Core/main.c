#include "main.h"
#include "ov2640.h"
#include "oled.h"
#include "delay.h"
#include "usart.h"

int main(void)
{
    OLED_Init();
    USART1_Init(921600);
    OV2640_Init();
    OLED_ShowHexNum(1, 1, OV2640_GetPID(), 4);
    OLED_ShowHexNum(2, 1, OV2640_GetMID(), 4);

    while (1)
    {
        OV2640_TestCaptureUART();
    }
}
