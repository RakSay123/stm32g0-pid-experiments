#include "i2c.h"
#define I2C1_EN (1U << 21)
#define I2C2_EN (1U << 22)

#define RD_WRN_WRITE (0U << 10)
#define RD_WRN_READ (1U << 10)
#define START (1U << 13)
#define STOP (1U << 14)
#define AUTOEND (1U << 25)
#define NACKF (1U << 4)
#define NACKCF (1U << 4)
#define STOPF (1U << 5)
#define STOPCF (1U << 5)
#define TXIS (1U << 1)
#define RXNE (1U << 2)
#define BUSY (1U << 15)
#define TC (1U << 6)


static void i2c_enable_clock(I2C_TypeDef *I2Cx)
{
	if (I2Cx == I2C1)
	{
		RCC->APBENR1 |= I2C1_EN;
	}
	else if (I2Cx == I2C2)
	{
		RCC->APBENR1 |= I2C2_EN;
	}
}

//static uint32_t i2c_compute_timing(uint32_t pclk, uint32_t speed)
//{
//	uint32_t presc = 0;
//	uint32_t total_cycles = pclk / speed;
//	uint32_t scll_cycles = (2 * total_cycles) / 3;
//	uint32_t sclh_cycles = total_cycles - scll_cycles;
//	uint32_t scll = scll_cycles - 1;
//	uint32_t sclh = sclh_cycles - 1;
//
//	uint32_t setup_cycles = (pclk * 250U) / 1000000000U;
//	uint32_t scldel = 0;
//	if (setup_cycles > 0)
//	{
//		scldel = (setup_cycles / (presc + 1)) - 1;
//	}
//	uint32_t sdadel = 0;
//
//	uint32_t timing = (presc << 28) | (scldel << 20) | (sdadel << 16) | (sclh << 8) | (scll << 0);
//	return timing;
//}

void i2c_init(I2C_TypeDef *I2Cx, const I2C_Config_t *cfg)
{
	i2c_enable_clock(I2Cx);
	I2Cx->CR1 &= ~1;
	I2Cx->TIMINGR = 0x00503D58; /*i2c_compute_timing(cfg->pclk, cfg->speed)*/
	I2Cx->CR1 |= 1;
}

static void i2c_start(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t nbytes, I2C_Direction_t direction, I2C_Repeated_Status_t repeated)
{
//	if (I2Cx->ISR & BUSY)
//	{
//	    i2c_reset(I2Cx);
//	}

	if (repeated == I2C_NOT_REPEATED) while (I2Cx->ISR & BUSY); // wait for BUSY = 0
	if (I2Cx->ISR & STOPF) I2Cx->ICR |= STOPCF;
	if (I2Cx->ISR & NACKF) I2Cx->ICR |= NACKCF;

	I2Cx->CR2 &= ~((0x3FF << 0) | RD_WRN_READ  | (0xFF << 16) | AUTOEND);
	I2Cx->CR2 |= (addr << 1); // SADD

	if (direction == I2C_READ)
	{
		I2Cx->CR2 |= RD_WRN_READ;
		if (repeated == I2C_REPEATED) I2Cx->CR2 |= AUTOEND;
	}
	else {
		I2Cx->CR2 |= RD_WRN_WRITE;
	}

	I2Cx->CR2 |= ((uint32_t)nbytes << 16); // number of bytes
	I2Cx->CR2 |= START;
}

static I2C_Status_t i2c_stop(I2C_TypeDef *I2Cx)
{
	uint32_t timeout = 100000;
	I2Cx->CR2 |= STOP; // request a stop
	while (!(I2Cx->ISR & STOPF)) // While stop not confirmed, do nothing, confirms we stopped
	{
		if (--timeout == 0)
		{
			return I2C_ERR_TIMEOUT;
		}
	}

	I2Cx->ICR |= STOPCF; // clear the stop flag via the interrupt clear register (ICR)
	return I2C_OK;
}

static I2C_Status_t i2c_write_byte(I2C_TypeDef *I2Cx, uint8_t data)
{
	uint32_t timeout = 100000;
	while (!(I2Cx->ISR & TXIS)) // wait until ready
	{
		if (I2Cx->ISR & NACKF)
		{
			I2Cx->ICR |= NACKCF;
			return I2C_ERR_NACK;
		}
		if (--timeout == 0)
		{
			return I2C_ERR_TIMEOUT;
		}
	}

	I2Cx->TXDR = data; // write
	return I2C_OK;
}

static I2C_Status_t i2c_wait_tc(I2C_TypeDef *I2Cx)
{
	uint32_t timeout = 100000;

	while (!(I2Cx->ISR & TC))
	{
		if (I2Cx->ISR & NACKF)
		{
			I2Cx->ICR |= NACKCF;
			return I2C_ERR_NACK;
		}

		if (--timeout == 0)
		{
			return I2C_ERR_TIMEOUT;
		}
	}

	return I2C_OK;
}

I2C_Status_t i2c_write_register(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len)
{
	i2c_start(I2Cx, addr, len + 1, I2C_WRITE, I2C_NOT_REPEATED);

	I2C_Status_t a = i2c_write_byte(I2Cx, reg);
	if (a != I2C_OK)
	{
		i2c_stop(I2Cx);
		return a;
	}

	for (uint8_t i = 0; i < len; i++)
	{
		I2C_Status_t b = i2c_write_byte(I2Cx, data[i]);
		if (b != I2C_OK)
		{
			i2c_stop(I2Cx);
			return b;
		}
	}

	I2C_Status_t tc = i2c_wait_tc(I2Cx);
	if (tc != I2C_OK)
	{
		i2c_stop(I2Cx);
		return tc;
	}

	return i2c_stop(I2Cx);
}

static I2C_Status_t i2c_read_byte(I2C_TypeDef *I2Cx, uint8_t *data)
{
	uint32_t timeout = 100000;
	while (!(I2Cx->ISR & RXNE)) // wait until ready to read
	{
		if (I2Cx->ISR & NACKF)
		{
			I2Cx->ICR |= NACKCF;
			return I2C_ERR_NACK;
		}

		if (--timeout == 0)
		{
			return I2C_ERR_TIMEOUT;
		}
	}

	*data = (uint8_t)(I2Cx->RXDR);
	return I2C_OK;
}

I2C_Status_t i2c_read_register(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len)
{
	i2c_start(I2Cx, addr, 1, I2C_WRITE, I2C_NOT_REPEATED); // 1 byte for the register

	I2C_Status_t a = i2c_write_byte(I2Cx, reg);
	if (a != I2C_OK)
	{
		i2c_stop(I2Cx);
		return a;
	}

	I2C_Status_t tc = i2c_wait_tc(I2Cx);
	if (tc != I2C_OK)
	{
		i2c_stop(I2Cx);
		return tc;
	}

	i2c_start(I2Cx, addr, len, I2C_READ, I2C_REPEATED); // IMPORTANT: restart for read

	for (uint8_t i = 0; i < len; i++) // read everything from RXDR and store it in data
	{
		I2C_Status_t b = i2c_read_byte(I2Cx, &data[i]);
		if (b != I2C_OK)
		{
			i2c_stop(I2Cx);
			return b;
		}
	}

	uint32_t timeout = 100000;
	while (!(I2Cx->ISR & STOPF))
	{
		if (--timeout == 0) return I2C_ERR_TIMEOUT;
	}
	I2Cx->ICR |= STOPCF;

	return I2C_OK;
}

//static void i2c_reset(I2C_TypeDef *I2Cx)
//{
//    I2Cx->CR1 &= ~I2C_CR1_PE;  // disable
//    I2Cx->CR1 |= I2C_CR1_PE;   // re-enable
//}
