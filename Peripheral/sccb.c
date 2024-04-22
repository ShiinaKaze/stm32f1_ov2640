#include "sccb.h"

void SW_SCCB_Init(void)
{

	RCC_APB2PeriphClockCmd(RCC_APB_SCCB_GPIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = SIO_C_PIN | SIO_D_PIN;
	GPIO_Init(SCCB_GPIO, &GPIO_InitStructure);
}

void SW_SCCB_SIO_D_SetInput(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = SIO_D_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SCCB_GPIO, &GPIO_InitStructure);
}

void SW_SCCB_SIO_D_SetOutput(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = SIO_D_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SCCB_GPIO, &GPIO_InitStructure);
}

void SW_SCCB_Start(void)
{
	// make sure SIO_C SIO_D high
	SW_SIO_C_Write(Bit_SET);
	SW_SIO_D_Write(Bit_SET);
	SW_SCCB_Delay();
	// SCCB Start condition
	SW_SIO_D_Write(Bit_RESET);
	SW_SCCB_Delay();
	SW_SIO_C_Write(Bit_RESET);
	SW_SCCB_Delay();
}

void SW_SCCB_Stop(void)
{
	// make sure SIO_D high
	SW_SIO_D_Write(Bit_RESET);
	SW_SCCB_Delay();
	// SCCB Stop condition
	SW_SIO_C_Write(Bit_SET);
	SW_SCCB_Delay();
	SW_SIO_D_Write(Bit_SET);
	SW_SCCB_Delay();
}

uint8_t SW_SCCB_Write_Byte(uint8_t byte)
{
	uint8_t x_bit;
	for (uint8_t i = 0; i < 8; i++)
	{
		if (byte & 0x80)
		{
			SW_SIO_D_Write(Bit_SET);
		}
		else
		{
			SW_SIO_D_Write(Bit_RESET);
		}
		byte <<= 1;
		SW_SCCB_Delay();
		SW_SIO_C_Write(Bit_SET);
		SW_SCCB_Delay();
		SW_SIO_C_Write(Bit_RESET);
		SW_SCCB_Delay();
	}
	SW_SCCB_SIO_D_SetInput();
	SW_SCCB_Delay();
	SW_SIO_C_Write(Bit_SET);
	x_bit = SW_SIO_D_Read(); // X/Don't care bit, typical value is 0
	SW_SCCB_Delay();
	SW_SIO_C_Write(Bit_RESET);
	SW_SCCB_Delay();
	SW_SCCB_SIO_D_SetOutput();

	// check sccb data
	if (x_bit != 0)
	{
		return SCCB_ERROR;
	}
	return x_bit;
}

uint8_t SW_SCCB_Read_Byte(void)
{
	uint8_t NA_bit;
	SW_SCCB_SIO_D_SetInput();
	uint8_t byte = 0x00;
	for (uint8_t i = 0; i < 8; i++)
	{
		SW_SCCB_Delay();
		SW_SIO_C_Write(Bit_SET);
		if (SW_SIO_D_Read())
		{
			byte |= (0x80 >> i);
		}
		SW_SCCB_Delay();
		SW_SIO_C_Write(Bit_RESET);
		SW_SCCB_Delay();
	}
	SW_SCCB_Delay();
	SW_SIO_C_Write(Bit_SET);
	NA_bit = SW_SIO_D_Read(); // NA bit, typical value is 1
	SW_SCCB_Delay();
	SW_SIO_C_Write(Bit_RESET);
	SW_SCCB_Delay();
	SW_SCCB_SIO_D_SetOutput();

	// check sccb data
	if (NA_bit != 1)
	{
		return SCCB_ERROR;
	}
	return byte;
}

void SW_SCCB_Register_Write(uint8_t id_address, uint8_t sub_address, uint8_t data)
{
	// 3-Phase Write
	SW_SCCB_Start();
	SW_SCCB_Write_Byte(id_address);
	SW_SCCB_Write_Byte(sub_address);
	SW_SCCB_Write_Byte(data);
	SW_SCCB_Stop();
}

uint8_t SW_SCCB_Register_Read(uint8_t id_address, uint8_t sub_address)
{
	uint8_t data;
	// 2-Phase Write
	SW_SCCB_Start();
	SW_SCCB_Write_Byte(id_address);
	SW_SCCB_Write_Byte(sub_address);
	SW_SCCB_Stop();
	// 2-Phase Read
	SW_SCCB_Start();
	SW_SCCB_Write_Byte(id_address | 0x01);
	data = SW_SCCB_Read_Byte();
	SW_SCCB_Stop();

	return data;
}

void SCCB_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB_SCCB_GPIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = SIO_C_PIN | SIO_D_PIN;
	GPIO_Init(SCCB_GPIO, &GPIO_InitStructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;								  // I2C模式
	I2C_InitStructure.I2C_ClockSpeed = 400000;								  // 传输速度，最大不能超过400KHZ
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;						  // 时钟占空比
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;								  // 主机是否设置应答
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // 主机地址位数  7位或10位
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;								  // 主机地址（当STM32位从机时）
	I2C_Init(I2C2, &I2C_InitStructure);

	I2C_Cmd(I2C2, ENABLE);
}

void SCCB_WaitEvent(I2C_TypeDef *I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 10000;
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)
	{
		Timeout--;
		if (Timeout == 0)
		{
			break;
		}
	}
}

void SCCB_WriteReg(uint8_t id_address, uint8_t sub_address, uint8_t data)
{
	I2C_GenerateSTART(I2C2, ENABLE);
	SCCB_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT); // 检测EV5

	I2C_Send7bitAddress(I2C2, id_address, I2C_Direction_Transmitter); // 发送从机地址
	SCCB_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED); // 检测EV6（发送模式）

	I2C_SendData(I2C2, sub_address);						  // 发送从机寄存器地址
	SCCB_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING); // 检测EV8

	I2C_SendData(I2C2, data);								 // 发送数据
	SCCB_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED); // 检测EV8_2

	I2C_GenerateSTOP(I2C2, ENABLE); // 停止发送
}

uint8_t SCCB_ReadReg(uint8_t id_address, uint8_t sub_address)
{
	uint8_t data;

	I2C_GenerateSTART(I2C2, ENABLE);
	SCCB_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);

	I2C_Send7bitAddress(I2C2, id_address, I2C_Direction_Transmitter);
	SCCB_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

	I2C_SendData(I2C2, sub_address);
	SCCB_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED); // 此上与发送数据相同
	I2C_GenerateSTOP(I2C2, ENABLE);

	I2C_GenerateSTART(I2C2, ENABLE);
	SCCB_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);

	I2C_Send7bitAddress(I2C2, id_address, I2C_Direction_Receiver);
	SCCB_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED); // 检测EV6（接收模式）

	I2C_AcknowledgeConfig(I2C2, DISABLE); // 主机给从机应答位A
	I2C_GenerateSTOP(I2C2, ENABLE);		  // 接收数据时需要提前发送终止位

	SCCB_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED); // 检测EV7
	data = I2C_ReceiveData(I2C2);

	I2C_AcknowledgeConfig(I2C2, ENABLE);

	return data;
}