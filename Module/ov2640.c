#include "ov2640.h"

void OV2640_HW_Reset(void)
{
    // Reset Camera
    GPIO_ResetBits(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, RESET_PIN);
    Delay_ms(10);
    GPIO_SetBits(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, RESET_PIN);
    Delay_ms(10);
}

void OV2640_SetPowerDownMode(uint8_t state)
{
    if (state)
    {
        GPIO_SetBits(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, PWDN_PIN);
    }
    else
    {
        // Set Device into Normal Mode
        GPIO_ResetBits(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, PWDN_PIN);
    }
}

uint16_t OV2640_GetPID(void)
{
    SCCB_Write(0xFF, 0x01);
    uint16_t PID = SCCB_Read(OV2640_REG_SENSOR_PIDH);
    PID <<= 8;
    PID |= SCCB_Read(OV2640_REG_SENSOR_PIDL);
    return PID;
}

uint16_t OV2640_GetMID(void)
{
    SCCB_Write(0xFF, 0x01);
    uint16_t MID = SCCB_Read(OV2640_REG_SENSOR_MIDH);
    MID <<= 8;
    MID |= SCCB_Read(OV2640_REG_SENSOR_MIDL);
    return MID;
}

void OV2640_HW_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB_OV2640, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // RESET, output
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = RESET_PIN;
    GPIO_Init(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, &GPIO_InitStructure);
    // HREF, VSYNC, input
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = HREF_PIN | VSYNC_PIN;
    GPIO_Init(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, &GPIO_InitStructure);
    // PCLK, input
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = PCLK_PIN;
    GPIO_Init(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, &GPIO_InitStructure);
    // PWDN, output
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = PWDN_PIN;
    GPIO_Init(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, &GPIO_InitStructure);
    // DATA, input
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = DATA0_PIN | DATA1_PIN | DATA2_PIN | DATA3_PIN | DATA4_PIN | DATA5_PIN | DATA6_PIN | DATA7_PIN;
    GPIO_Init(DATA_GPIO, &GPIO_InitStructure);
}
void OV2640_InitConfig(void)
{
    uint16_t cfg_len = sizeof(atk_mc2640_init_uxga_cfg) / sizeof(atk_mc2640_init_uxga_cfg[0]);
    for (uint16_t i = 0; i < cfg_len; i++)
    {
        SCCB_Write(atk_mc2640_init_uxga_cfg[i][0], atk_mc2640_init_uxga_cfg[i][1]);
    }
}

void OV2640_SetClockDivision(uint8_t Clock, uint8_t DVP_Clock)
{
    // Clock division
    SCCB_Write(OV2640_REG_BANK, 0x00);
    SCCB_Write(OV2640_REG_DSP_R_DVP_SP, DVP_Clock); // DVP PCLK division, DVP PCLK = sysclk (48)/[6:0] (YUV0),sysclk (48)/(2*[6:0]) (RAW)
    SCCB_Write(OV2640_REG_BANK, 0x01);
    SCCB_Write(OV2640_REG_SENSOR_CLKRC, Clock); // CLK division, CLK = XVCLK/(decimal value of CLKRC[5:0] + 1)
}

void OV2640_SetOutputJPEG(void)
{
    uint16_t cfg_len = sizeof(atk_mc2640_set_yuv422_cfg) / sizeof(atk_mc2640_set_yuv422_cfg[0]);
    for (uint16_t cfg_index = 0; cfg_index < cfg_len; cfg_index++)
    {
        SCCB_Write(atk_mc2640_set_yuv422_cfg[cfg_index][0], atk_mc2640_set_yuv422_cfg[cfg_index][1]);
    }
    cfg_len = sizeof(atk_mc2640_set_jpeg_cfg) / sizeof(atk_mc2640_set_jpeg_cfg[0]);
    for (uint16_t cfg_index = 0; cfg_index < cfg_len; cfg_index++)
    {
        SCCB_Write(atk_mc2640_set_jpeg_cfg[cfg_index][0], atk_mc2640_set_jpeg_cfg[cfg_index][1]);
    }
}

void OV2640_SetOutputSize(uint16_t width, uint16_t height)
{
    uint16_t output_width;
    uint16_t output_height;

    output_width = width >> 2;
    output_height = height >> 2;

    SCCB_Write(OV2640_REG_BANK, 0x00);
    SCCB_Write(OV2640_REG_DSP_RESET, 0x04);
    SCCB_Write(OV2640_REG_DSP_ZMOW, (uint8_t)(output_width & 0x00FF));
    SCCB_Write(OV2640_REG_DSP_ZMOH, (uint8_t)(output_height & 0x00FF));
    SCCB_Write(OV2640_REG_DSP_ZMHH, ((uint8_t)(output_width >> 8) & 0x03) | ((uint8_t)(output_height >> 6) & 0x04));
    SCCB_Write(OV2640_REG_DSP_RESET, 0x00);
}

void OV2640_Init(void)
{
    SW_SCCB_Init();
    OV2640_HW_Init();
    OV2640_HW_Reset();
    OV2640_SetPowerDownMode(0);

    // OV2640 Init
    OV2640_InitConfig();
}

void OV2640_TransmitFrame_JPEG_UART(uint8_t *jpeg_buffer, uint16_t jpeg_buffer_length)
{
    uint32_t buffer_inedex = 0;
    // After testing, the actual OV2640 timing may differ from those given in the datasheet
    while (OV2640_VSYNC == 0) // wait for new frame VSYNC rising edge
    {
    }
    while (OV2640_VSYNC == 1)
    {
        while (OV2640_HREF == 1) // when HREF high, read row
        {
            while (OV2640_PCLK == 0) // wait for PCLK rising edge and read data
            {
            }
            jpeg_buffer[buffer_inedex] = OV2640_DATA;
            buffer_inedex++;
            while (OV2640_PCLK == 1)
            {
            }
        }
    }
    uint32_t jpeg_valid_start, jpeg_valid_end = 0;
    for (jpeg_valid_start = 0; jpeg_valid_start < jpeg_buffer_length; jpeg_valid_start++)
    {
        if (jpeg_buffer[jpeg_valid_start] == 0xD8 && jpeg_buffer[jpeg_valid_start - 1] == 0xFF)
        {
            for (jpeg_valid_end = jpeg_valid_start + (320 * 240 / 10 / 2); jpeg_valid_end < jpeg_buffer_length; jpeg_valid_end++)
            {
                if (jpeg_buffer[jpeg_valid_end] == 0xD9 && jpeg_buffer[jpeg_valid_end - 1] == 0xFF)
                {
                    UART1_Transmit(jpeg_buffer + jpeg_valid_start - 1, jpeg_valid_end - jpeg_valid_start + 2);
                    break;
                }
            }
        }
    }
}

void OV2640_StartFrameStream_DMA_IT_UART(uint8_t *jpeg_buffer, uint16_t buffer_length)
{
    // DMA config
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DATA_GPIO->IDR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)jpeg_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = buffer_length;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel2, ENABLE);
    // TIM config
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    TIM_Cmd(TIM3, ENABLE);
    // TIM_DMACmd(TIM3, TIM_DMA_CC3, ENABLE);

    // EXTI_InitTypeDef EXTI_InitStructure;
    // NVIC_InitTypeDef NVIC_InitStructure;

    // // HREF EXTI
    // GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);

    // EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    // EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    // EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    // EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    // EXTI_Init(&EXTI_InitStructure);

    // NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);

    // // VSYNC EXTI
    // GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);

    // EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    // EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    // EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    // EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    // EXTI_Init(&EXTI_InitStructure);

    // NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);
}
