#include "main.h"
#include "ov2640.h"
#include "sccb.h"
#include "oled.h"
#include "delay.h"
#include "usart.h"

int main(void)
{
    OLED_Init();
    SW_SCCB_Init();
    USART1_Init(115200);
    
    while (1)
    {
        uint16_t PID = SW_SCCB_Register_Read(OV2640_DEVICE_ADDRESS, 0x0A);
        PID <<= 8;
        PID |= SW_SCCB_Register_Read(OV2640_DEVICE_ADDRESS, 0x0B);
        OLED_ShowHexNum(1, 1, PID, 4);
    }
}
