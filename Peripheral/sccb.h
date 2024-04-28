#ifndef _SCCB_H
#define _SCCB_H

#include "main.h"
#include "delay.h"

#define RCC_APB_SCCB_GPIO RCC_APB2Periph_GPIOB
#define SCCB_GPIO GPIOB

#define SIO_C_PIN GPIO_Pin_10
#define SIO_D_PIN GPIO_Pin_11
#define SCCB_ERROR 0xFF

#define SW_SIO_C_Write(BIT_VALUE) GPIO_WriteBit(SCCB_GPIO, SIO_C_PIN, BIT_VALUE)
#define SW_SIO_D_Write(BIT_VALUE) GPIO_WriteBit(SCCB_GPIO, SIO_D_PIN, BIT_VALUE)
#define SW_SIO_D_Read() GPIO_ReadInputDataBit(SCCB_GPIO, SIO_D_PIN)
#define SW_SCCB_Delay() Delay_us(5)

void SW_SCCB_Init(void);
void SW_SCCB_Register_Write(uint8_t id_address, uint8_t sub_address, uint8_t data);
uint8_t SW_SCCB_Register_Read(uint8_t id_address, uint8_t sub_address);

void SCCB_Init(void);
void SCCB_WriteReg(uint8_t id_address, uint8_t sub_address, uint8_t data);
uint8_t SCCB_ReadReg(uint8_t id_address, uint8_t sub_address);
#endif // _SCCB_H
