#include "uart.h"

#define USART1_EN (1U << 14)
#define USART2_EN (1U << 17)
#define USART3_EN (1U << 18)
#define USART4_EN (1U << 19)

#define TE_EN (1U << 3)
#define RE_EN (1U << 2)
#define UE_EN (1U << 0)

#define TXFNF (1U << 7)
#define RXFNE (1U << 5)

void uart_enable_clock(USART_TypeDef *USARTx)
{
	if (USARTx == USART1)
	{
		RCC->APBENR2 |= USART1_EN;
	}
	else if(USARTx == USART2)
	{
		RCC->APBENR1 |= USART2_EN;
	}
	else if (USARTx == USART3)
	{
		RCC->APBENR1 |= USART3_EN;
	}
	else if (USARTx == USART4)
	{
		RCC->APBENR1 |= USART4_EN;
	}
}

void uart_init(const UART_Config_t *cfg)
{
	uart_enable_clock(cfg->USARTx);
	uint32_t brr = (cfg->fclk + (cfg->baud_rate / 2U)) / cfg->baud_rate;
	cfg->USARTx->BRR = brr;
	cfg->USARTx->CR1 = 0;
	cfg->USARTx->CR1 |= TE_EN | RE_EN;
	cfg->USARTx->CR1 |= UE_EN;
}

void uart_write(USART_TypeDef *USARTx, char c)
{
	while (!(USARTx->ISR & TXFNF));
	USARTx->TDR = (c & 0xFF);
}

void uart_write_str(USART_TypeDef *USARTx, char* str)
{
	for (int i = 0; str[i] != '\0'; i++) {
		uart_write(USARTx, str[i]);
	}
}

void uart_write_line(USART_TypeDef *USARTx, char* str)
{
	uart_write_str(USARTx, str);
	uart_write_str(USARTx, "\r");
	uart_write_str(USARTx, "\n");
}

void uart_write_buf(USART_TypeDef *USARTx, uint8_t *buf, uint16_t len)
{
	for (uint16_t i = 0; i < len; i++)
	{
		uart_write(USARTx, buf[i]);
	}
}

void uart_write_hex(USART_TypeDef *USARTx, uint8_t value)
{
	char hex_chars[] = "0123456789ABCDEF";

	uart_write(USARTx, '0');
	uart_write(USARTx, 'x');

	uart_write(USARTx, hex_chars[(value >> 4) & 0xF]);
	uart_write(USARTx, hex_chars[value & 0xF]);
}

void uart_write_int(USART_TypeDef *USARTx, int32_t num)
{
    char buffer[12];
    int i = 0;

    if (num == 0)
    {
        uart_write(USARTx, '0');
        return;
    }

    if (num < 0)
    {
        uart_write(USARTx, '-');
        num = -num;
    }

    while (num > 0)
    {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }

    // reverse
    while (i > 0)
    {
        uart_write(USARTx, buffer[--i]);
    }
}

void uart_write_float(USART_TypeDef *USARTx, float value)
{
    int32_t int_part = (int32_t)value;
    int32_t frac_part = (int32_t)((value - int_part) * 100); // 2 decimal places

    // Handle negative numbers
    if (value < 0)
    {
        uart_write(USARTx, '-');
        int_part = -int_part;
        frac_part = -frac_part;
    }

    // Print integer part
    uart_write_int(USARTx, int_part);

    uart_write(USARTx, '.');

    // Ensure leading zero for fraction (e.g., 0.05)
    if (frac_part < 10)
    {
        uart_write(USARTx, '0');
    }

    uart_write_int(USARTx, frac_part);
}

char uart_read(USART_TypeDef *USARTx)
{
	while (!(USARTx->ISR & RXFNE));
	return (char)(USARTx->RDR & 0xFF);
}

void uart_read_str(USART_TypeDef *USARTx, char *buffer, int max_len)
{
	int i = 0;
	char c;

	do
	{
		c = uart_read(USARTx);
	} while (c == '\r' || c == '\n');

	while (i < max_len - 1) {
		if (c == '\r' || c == '\n') // ENTER
		{
			uart_write(USARTx, '\r');
			uart_write(USARTx, '\n');
			break;
		}
		else if (c == '\b' || c == 0x7F) // BACKSPACE
		{
			if (i > 0)
			{
				i--;
				uart_write(USARTx, '\b');
				uart_write(USARTx, ' ');
				uart_write(USARTx, '\b');
			}

			continue;
		}
		else // NORMAL CHAR
		{
			buffer[i++] = c;
			uart_write(USARTx, c);
		}

		c = uart_read(USARTx);
	}

	buffer[i] = '\0';

}

void uart_flush(USART_TypeDef *USARTx)
{
	while (USARTx->ISR & RXFNE)
	{
		volatile char dummy = (USARTx->RDR & 0xFF);
		(void)dummy;
	}
}
