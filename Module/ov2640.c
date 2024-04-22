#include "ov2640.h"

void OV2640_HW_Reset(void)
{
    // Reset Camera
    GPIO_ResetBits(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, RESET_PIN);
    Delay_ms(10);
    GPIO_SetBits(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, RESET_PIN);
    Delay_ms(10);
}

void OV2640_SetPowerDownMode(BitAction BitVal)
{
    // Set Device into Normal Mode
    if (BitVal)
    {
        GPIO_SetBits(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, PWDN_PIN);
    }
    else
    {
        GPIO_ResetBits(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, PWDN_PIN);
    }
}

uint16_t OV2640_GetPID(void)
{
    SCCB_Write(0xFF, 0x01);
    uint16_t PID = SCCB_Read(OV2640_SENSOR_PIDH);
    PID <<= 8;
    PID |= SCCB_Read(OV2640_SENSOR_PIDL);
    return PID;
}

uint16_t OV2640_GetMID(void)
{
    SCCB_Write(0xFF, 0x01);
    uint16_t MID = SCCB_Read(OV2640_SENSOR_MIDH);
    MID <<= 8;
    MID |= SCCB_Read(OV2640_SENSOR_MIDL);
    return MID;
}

void OV2640_InitConfig(void)
{
    uint16_t cfg_len = sizeof(atk_mc2640_init_uxga_cfg) / sizeof(atk_mc2640_init_uxga_cfg[0]);
    for (uint16_t i = 0; i < cfg_len; i++)
    {
        SCCB_Write(atk_mc2640_init_uxga_cfg[i][0], atk_mc2640_init_uxga_cfg[i][1]);
    }
}

void OV2640_IOInit(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB_OV2640, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // RESET, output
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = RESET_PIN;
    GPIO_Init(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, &GPIO_InitStructure);
    // HREF, VSYNC, input
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = HREF_PIN | VSYNC_PIN;
    GPIO_Init(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, &GPIO_InitStructure);
    // PCLK, input
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = PCLK_PIN;
    GPIO_Init(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, &GPIO_InitStructure);
    // PWDN, output
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = PWDN_PIN;
    GPIO_Init(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, &GPIO_InitStructure);
    // DATA, input
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = DATA0_PIN | DATA1_PIN | DATA2_PIN | DATA3_PIN | DATA4_PIN | DATA5_PIN | DATA6_PIN | DATA7_PIN;
    GPIO_Init(DATA_GPIO, &GPIO_InitStructure);
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

    SCCB_Write(0xFF, 0x00);
    SCCB_Write(OV2640_DSP_RESET, 0x04);
    SCCB_Write(OV2640_DSP_ZMOW, (uint8_t)(output_width & 0x00FF));
    SCCB_Write(OV2640_DSP_ZMOH, (uint8_t)(output_height & 0x00FF));
    SCCB_Write(OV2640_DSP_ZMHH, ((uint8_t)(output_width >> 8) & 0x03) | ((uint8_t)(output_height >> 6) & 0x04));
    SCCB_Write(OV2640_DSP_RESET, 0x00);

    // uint16_t outh;
    // uint16_t outw;
    // uint8_t temp;
    // if (width % 4)
    // {
    //     return 1;
    // }
    // if (height % 4)
    // {
    //     return 2;
    // }
    // outw = width / 4;
    // outh = height / 4;
    // SCCB_Write(0XFF, 0X00);
    // SCCB_Write(0XE0, 0X04);
    // SCCB_Write(0X5A, outw & 0XFF); // 设置OUTW的低八位
    // SCCB_Write(0X5B, outh & 0XFF); // 设置OUTH的低八位
    // temp = (outw >> 8) & 0X03;
    // temp |= (outh >> 6) & 0X04;
    // SCCB_Write(0X5C, temp); // 设置OUTH/OUTW的高位
    // SCCB_Write(0XE0, 0X00);
}

void OV2640_Init(void)
{
    OV2640_IOInit();
    OV2640_HW_Reset();
    SW_SCCB_Init();
    OV2640_InitConfig();

    SCCB_Write(0XFF, 0x00);
    SCCB_Write(0XD3, 16); // DVP PCLK division
    SCCB_Write(0XFF, 0x01);
    SCCB_Write(0X11, 6); // CLK division

    OV2640_SetOutputJPEG();
    OV2640_SetOutputSize(320, 240);
}

void OV2640_TestCaptureUART(void)
{
    uint8_t jpeg_buffer[10 * 1024] = {0};
    uint32_t buffer_inedex = 0;
    while (OV2640_VSYNC == 0) // wait for new frame VSYNC rising edge
    {
    }
    while (OV2640_VSYNC == 1) // wait for new frame VSYNC falling edge
    {
        while (OV2640_HREF == 1) // when HREF high, read row
        {
            while (OV2640_PCLK == 0) // wait for PCLK rising edge and read data
            {
            }
            jpeg_buffer[buffer_inedex] = OV2640_DATA;
            buffer_inedex++;
            while (OV2640_PCLK == 1) //  wait for PCLK falling edge, update data
            {
            }
        }
    }
    uint32_t jpeg_valid_start = 0;
    for (jpeg_valid_start = 0; jpeg_valid_start < buffer_inedex; jpeg_valid_start++)
    {
        if (jpeg_buffer[jpeg_valid_start] == 0xFF && jpeg_buffer[jpeg_valid_start + 1] == 0xD8)
        {
            uint32_t jpeg_valid_end = 0;
            for (jpeg_valid_end = jpeg_valid_start; jpeg_valid_end < buffer_inedex; jpeg_valid_end++)
            {
                if (jpeg_buffer[jpeg_valid_end] == 0xD9 && jpeg_buffer[jpeg_valid_end - 1] == 0xFF)
                {
                    USART_Transmit(jpeg_buffer + jpeg_valid_start, jpeg_valid_end - jpeg_valid_start + 1);
                    break;
                }
            }
        }
    }
}
