#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "stm32g070xx.h"

typedef struct {
	USART_TypeDef *USARTx;
	uint32_t fclk;
	uint32_t baud_rate;
} UART_Config_t;

void uart_enable_clock(USART_TypeDef *USARTx);

void uart_init(const UART_Config_t *cfg);

void uart_write(USART_TypeDef *USARTx, char c);

void uart_write_str(USART_TypeDef *USARTx, char* str);

void uart_write_line(USART_TypeDef *USARTx, char* str);

void uart_write_buf(USART_TypeDef *USARTx, uint8_t *buf, uint16_t len);

void uart_write_hex(USART_TypeDef *USARTx, uint8_t value);

void uart_write_int(USART_TypeDef *USARTx, int32_t num);

void uart_write_float(USART_TypeDef *USARTx, float value);

char uart_read(USART_TypeDef *USARTx);

void uart_read_str(USART_TypeDef *USARTx, char *buffer, int max_len);

void uart_flush(USART_TypeDef *USARTx);

#endif /* UART_H_ */
