#include "stm32f10x.h"
#include "i2c.h"

/**
 * @brief  Initializes I2C peripheral
 * @param  None
 * @retval None
 */
void SW_I2C_Init(void)
{
	// GPIO clock enable
	RCC_APB2PeriphClockCmd(RCC_APB_GPIO, ENABLE);
	// GPIO Setting
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = SCL_PIN | SDA_PIN;
	GPIO_Init(I2C_GPIOX, &GPIO_InitStructure);

	SW_I2C_SCL(Bit_SET);
	SW_I2C_SDA(Bit_SET);
}

/**
 * @brief  I2C start condition
 * @param  None
 * @retval None
 */
void SW_I2C_Start(void)
{
	SW_I2C_SDA(Bit_SET);
	SW_I2C_SCL(Bit_SET);
	SW_I2C_SDA(Bit_RESET);
	SW_I2C_SCL(Bit_RESET);
}

/**
 * @brief  I2C stop condition
 * @param  None
 * @retval None
 */
void SW_I2C_Stop(void)
{
	SW_I2C_SDA(Bit_RESET);
	SW_I2C_SCL(Bit_SET);
	SW_I2C_SDA(Bit_SET);
}

/**
 * @brief  Transmit one byte using I2C, ACK enable.
 * @param  Byte
 * @retval None
 */
uint8_t SW_I2C_TransmitByte(uint8_t Byte)
{
	uint8_t ack;
	for (uint8_t i = 0; i < 8; i++)
	{
		if (Byte & 0x80)
		{
			SW_I2C_SDA(Bit_SET);
		}
		else
		{
			SW_I2C_SDA(Bit_RESET);
		}
		Byte <<= 1;

		SW_I2C_SCL(Bit_SET);
		SW_I2C_SCL(Bit_RESET);
	}

	// receive ACK
	ack = SW_I2C_ReadBit();
	SW_I2C_SCL(Bit_SET);
	SW_I2C_SCL(Bit_RESET);
	return ack;
}

void HW_I2C_Init(void)
{
	// GPIO clock enable
	RCC_APB2PeriphClockCmd(RCC_APB_GPIO, ENABLE);
	// GPIO Setting
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = SCL_PIN | SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_GPIOX, &GPIO_InitStructure);
	// I2C clock enable
	RCC_APB1PeriphClockCmd(RCC_APB_I2C, ENABLE);
	// I2C Setting
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_ClockSpeed = I2C_FAST_SPEED;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2CX, &I2C_InitStructure);

	I2C_Cmd(I2CX, ENABLE);
}
