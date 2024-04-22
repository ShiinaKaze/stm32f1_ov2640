#include "main.h"

#define RCC_APB_USART_GPIO RCC_APB2Periph_GPIOA
#define RCC_APB_USART RCC_APB2Periph_USART1
#define UART_GPIO GPIOA
#define USART_TX_PIN GPIO_Pin_9
#define USART_RX_PIN GPIO_Pin_10
#define USART USART1

void USART1_Init(uint32_t baudrate);
void USART_Transmit(uint8_t *TxBuffer, uint16_t TxLength);
