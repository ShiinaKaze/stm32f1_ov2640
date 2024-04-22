#include "oled.h"
#include "oled_font.h"
#include "delay.h"

// SSD1306 configuration
#define SSD1306_Clock RCC_APB2Periph_GPIOB
#define SSD1306_GPIOx GPIOB
#define SSD1306_SCL_Pin GPIO_Pin_6
#define SSD1306_SDA_Pin GPIO_Pin_7
#define SSD1306_Slave_Address 0x78
#define SSD1306_Control_Command 0x00
#define SSD1306_Control_Data 0x40

/**
 * @brief  	Write commands to the device using I2C
 * 			If you want to use hardware I2C, you can modify this function.
 * @param  	Command
 * @retval 	None
 */
void I2C_WC_SSD1306(uint8_t Command)
{
	SW_I2C_Start();
	SW_I2C_TransmitByte(SSD1306_Slave_Address);	  // slave address
	SW_I2C_TransmitByte(SSD1306_Control_Command); // write command
	SW_I2C_TransmitByte(Command);
	SW_I2C_Stop();
}

/**
 * @brief  	Write data to the device using I2C
 * 			If you want to use hardware I2C, you can modify this function.
 * @param 	Data
 * @retval 	None
 */
void I2C_WD_SSD1306(uint8_t Data)
{
	// If you want to use hardware I2C, you can modify this function.
	SW_I2C_Start();
	SW_I2C_TransmitByte(SSD1306_Slave_Address); // slave address
	SW_I2C_TransmitByte(SSD1306_Control_Data);	// write data
	SW_I2C_TransmitByte(Data);
	SW_I2C_Stop();
}

/**
 * @brief  OLED sets cursor position
 * @param  Y With the upper left corner as the origin, downward direction coordinates, range: 0-7
 * @param  X With the upper left corner as the origin, the coordinates in the right direction, range 0-127
 * @retval None
 */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	I2C_WC_SSD1306(0xB0 | Y);							  // Set Y position
	I2C_WC_SSD1306(0x10 | ((X & 0xF0) >> 4));			  // Set X position 4 bits higher
	I2C_WC_SSD1306(SSD1306_Control_Command | (X & 0x0F)); // Set X position 4 bits lower
}

/**
 * @brief  OLED clear screen
 * @param  None
 * @retval None
 */
void OLED_Clear(void)
{
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for (i = 0; i < 128; i++)
		{
			I2C_WD_SSD1306(SSD1306_Control_Command);
		}
	}
}

/**
 * @brief  OLED displays a character
 * @param  Line Line position, value range 1-4
 * @param  Column Column position, value range 1-16
 * @param  Char A character to display, range: ASCII characters
 * @retval None
 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, uint8_t Char)
{
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8); // Set the cursor position in the top half
	for (i = 0; i < 8; i++)
	{
		I2C_WD_SSD1306(Font8x16[Char - ' '][i]); // Displays the top half of the content
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8); // Set the cursor position in the bottom half
	for (i = 0; i < 8; i++)
	{
		I2C_WD_SSD1306(Font8x16[Char - ' '][i + 8]); // Displays the bottom half of the content
	}
}

/**
 * @brief  OLED display string
 * @param  Line Line position, value range 1-4
 * @param  Column Column position, value range 1-16
 * @param  String String to display, range: ASCII characters
 * @retval None
 */
void OLED_ShowString(uint8_t Line, uint8_t Column, uint8_t *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
 * @brief  OLED power function
 * @retval The return value is equal to X to the Y power
 */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
 * @brief  OLED display numbers (decimal, positive)
 * @param  Line Line position, range 1-4
 * @param  Column Column position, range 1-16
 * @param  Number The number to display
 * @param  Length To display the length of the number, the value ranges from 1 to 10
 * @retval None
 */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i,
					  Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
 * @brief  OLED display number (decimal, signed number)
 * @param  Line Line position, range 1-4
 * @param  Column Column position, range 1-16
 * @param  Number The number to display
 * @param  Length To display the length of the number, the value ranges from 1 to 10
 * @retval None
 */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number,
						uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i + 1,
					  Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
 * @brief  OLED display numbers (hexadecimal, positive)
 * @param  Line Line position, range 1-4
 * @param  Column Column position, range 1-16
 * @param  Number The number to display， value range：0-0xFFFFFFFF
 * @param  Length To display the number length, the value ranges from 1 to 8
 * @retval None
 */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number,
					 uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
 * @brief  OLED display numbers (binary, positive)
 * @param  Line Line position, range 1-4
 * @param  Column Column position, range 1-16
 * @param  Number The number to display，value range ：0-1111 1111 1111 1111
 * @param  Length To display the number length, the value ranges from 1 to 16
 * @retval None
 */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number,
					 uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i,
					  Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
 * @brief  Initializes the OLED
 * @param  None
 * @retval None
 */
void OLED_Init(void)
{

	Delay_s(1);
	SW_I2C_Init();

	// software configuration from SSD1306 Application Note
	I2C_WC_SSD1306(0xA8); // Set MUX Ratio
	I2C_WC_SSD1306(0x3F);

	I2C_WC_SSD1306(0xD3); // Set Display Offset
	I2C_WC_SSD1306(0x00);

	I2C_WC_SSD1306(0x40); // Set Display Start Line

	I2C_WC_SSD1306(0xA1); // Set Segment re-map

	I2C_WC_SSD1306(0xC8); // Set COM Output Scan Direction

	I2C_WC_SSD1306(0xDA); // Set COM Pins hardware configuration
	I2C_WC_SSD1306(0x12);

	I2C_WC_SSD1306(0x81); // Set Contrast Control
	I2C_WC_SSD1306(0xCF);

	I2C_WC_SSD1306(0xA4); // Disable Entire Display On

	I2C_WC_SSD1306(0xA6); // Set Normal Display

	I2C_WC_SSD1306(0xD5); // Set Osc Frequency
	I2C_WC_SSD1306(0x80);

	I2C_WC_SSD1306(0x8D); // Enable charge pump regulator
	I2C_WC_SSD1306(0x14);

	I2C_WC_SSD1306(0xAF); // Display On AFh

	OLED_Clear(); // OLED Clear
}
