#ifndef __OLED_H
#define __OLED_H

#include "main.h"
#include "i2c.h"

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, uint8_t Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, uint8_t *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number,
		uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number,
		uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number,
		uint8_t Length);

#endif
