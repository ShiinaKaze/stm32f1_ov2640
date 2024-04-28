#include "main.h"
#include "ov2640.h"
#include "oled.h"
#include "delay.h"
#include "uart.h"

#define JPEG_BUFFER_LENGTH 16 * 1024
#define OUTPUT_IMAGE_WIDTH 320
#define OUTPUT_IMAGE_HEIGHT 240
#define OV2640_CLOCK 4
#define OV2640_DVP_CLOCK 48
int main(void)
{
    // Clock
    RCC_ClocksTypeDef Clock;
    RCC_GetClocksFreq(&Clock);

    // Initialization
    OLED_Init();
    UART1_Init(460800);
    OV2640_Init();
    // OV2640 Setting
    uint8_t jpeg_buffer[JPEG_BUFFER_LENGTH] = {0};
    OV2640_SetClockDivision(OV2640_CLOCK, OV2640_DVP_CLOCK);
    OV2640_SetOutputJPEG();
    OV2640_SetOutputSize(OUTPUT_IMAGE_WIDTH, OUTPUT_IMAGE_HEIGHT);

    // OLED_ShowString(1, 1, "SYSCLK:");
    // OLED_ShowString(2, 1, "PCLK2:");
    OLED_ShowString(1, 1, "CAM PID:");
    // OLED_ShowNum(1, 8, Clock.SYSCLK_Frequency, 8);
    // OLED_ShowNum(2, 7, Clock.PCLK2_Frequency, 8);
    OLED_ShowHexNum(1, 10, OV2640_GetPID(), 4);
    OV2640_StartFrameStream_DMA_IT_UART(jpeg_buffer, JPEG_BUFFER_LENGTH);
    while (1)
    {
        // OV2640_TransmitFrame_JPEG_UART(jpeg_buffer, JPEG_BUFFER_LENGTH);
        while (OV2640_VSYNC == 0) // wait for new frame VSYNC rising edge
        {
        }
        while (OV2640_VSYNC == 1)
        {
            if (OV2640_HREF == 1 && (TIM3->DIER & TIM_DMA_CC3) == 0)
            {
                TIM3->DIER |= TIM_DMA_CC3;
            }
            if (OV2640_HREF == 0 && (TIM3->DIER & TIM_DMA_CC3) != 0)
            {
                TIM3->DIER &= ~TIM_DMA_CC3;
            }
        }
        uint32_t jpeg_valid_start, jpeg_valid_end = 0;
        for (jpeg_valid_start = 0; jpeg_valid_start < JPEG_BUFFER_LENGTH; jpeg_valid_start++)
        {
            if (jpeg_buffer[jpeg_valid_start] == 0xD8 && jpeg_buffer[jpeg_valid_start - 1] == 0xFF)
            {
                break;
            }
        }
        for (jpeg_valid_end = jpeg_valid_start; jpeg_valid_end < JPEG_BUFFER_LENGTH; jpeg_valid_end++)
        {
            if (jpeg_buffer[jpeg_valid_end] == 0xD9 && jpeg_buffer[jpeg_valid_end - 1] == 0xFF)
            {
                UART1_Transmit(jpeg_buffer + jpeg_valid_start - 1, jpeg_valid_end - jpeg_valid_start + 2);
                break;
            }
        }
        DMA1_Channel2->CCR &= ~DMA_CCR2_EN;
        DMA1_Channel2->CNDTR = JPEG_BUFFER_LENGTH;
        DMA1_Channel2->CCR |= DMA_CCR2_EN;
    }
}
