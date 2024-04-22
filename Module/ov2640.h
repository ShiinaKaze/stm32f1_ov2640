#ifndef _OV2640_H
#define _OV2640_H

#include "main.h"
#include "delay.h"
#include "sccb.h"
#include "dma.h"
#include "usart.h"
#include "ov2640_cfg.h"

#define OV2640_DEVICE_ADDRESS 0x60

#define RCC_APB_OV2640 (RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC)

// Control Pin
// SIO_C and SIO_D are defined in sccb.h

#define PCLK_PWDN_HREF_RESET_VSYNC_GPIO GPIOB
#define PCLK_PIN GPIO_Pin_0
#define PWDN_PIN GPIO_Pin_1
#define RESET_PIN GPIO_Pin_3
#define HREF_PIN GPIO_Pin_4
#define VSYNC_PIN GPIO_Pin_5

// Data Pin
#define DATA_GPIO GPIOA
#define DATA0_PIN GPIO_Pin_0
#define DATA1_PIN GPIO_Pin_1
#define DATA2_PIN GPIO_Pin_2
#define DATA3_PIN GPIO_Pin_3
#define DATA4_PIN GPIO_Pin_4
#define DATA5_PIN GPIO_Pin_5
#define DATA6_PIN GPIO_Pin_6
#define DATA7_PIN GPIO_Pin_7

#define OV2640_DATA GPIOA->IDR & 0x00FF
#define OV2640_PCLK GPIO_ReadInputDataBit(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, PCLK_PIN)
#define OV2640_HREF GPIO_ReadInputDataBit(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, HREF_PIN)
#define OV2640_VSYNC GPIO_ReadInputDataBit(PCLK_PWDN_HREF_RESET_VSYNC_GPIO, VSYNC_PIN)

#define SCCB_Write(sub_address, data) SW_SCCB_Register_Write(OV2640_DEVICE_ADDRESS, sub_address, data)
#define SCCB_Read(sub_address) SW_SCCB_Register_Read(OV2640_DEVICE_ADDRESS, sub_address)

void OV2640_HW_Reset(void);
uint16_t OV2640_GetPID(void);
uint16_t OV2640_GetMID(void);
void OV2640_InitConfig(void);
void OV2640_Init(void);
void OV2640_InitConfig(void);
void OV2640_SetOutputSize(uint16_t width, uint16_t height);
void OV2640_TestCaptureUART(void);

#endif // _OV2640_H
