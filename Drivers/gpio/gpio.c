#include "gpio.h"

void gpio_enable_clock(GPIO_TypeDef *GPIOx)
{

	if (GPIOx == GPIOA)
	{
		RCC->IOPENR |= (1U << 0);
	}
	else if (GPIOx == GPIOB)
	{
		RCC->IOPENR |= (1U << 1);
	}
	else if (GPIOx == GPIOC)
	{
		RCC->IOPENR |= (1U << 2);
	}
	else if (GPIOx == GPIOD)
	{
		RCC->IOPENR |= (1U << 3);
	}
#ifdef GPIOE
	else if (GPIOx == GPIOE)
	{
		RCC->IOPENR |= (1U << 4);
	}
#endif
	else if (GPIOx == GPIOF)
	{
		RCC->IOPENR |= (1U << 5);
	}
	else
	{
		// Error handling to be implemented here in the future
	}
}

void gpio_mode_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Mode_t mode)
{
	GPIOx->MODER = (GPIOx->MODER & ~(3U << (2 * pin))) | ((uint32_t)mode << (2 * pin));
}

void gpio_output_type_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_OType_t otype)
{
	if (otype == GPIO_OPEN_DRAIN)
	{
		GPIOx->OTYPER |= (1U << pin);
	}
	else
	{
		GPIOx->OTYPER &= ~(1U << pin);
	}
}

void gpio_speed_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Speed_t speed)
{
	GPIOx->OSPEEDR = (GPIOx->OSPEEDR & ~(3U << (2 * pin))) | ((uint32_t)speed << (2 * pin));
}

void gpio_pull_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Pull_t pull)
{
	GPIOx->PUPDR = (GPIOx->PUPDR & ~(3U << (2 * pin))) | ((uint32_t)pull << (2 * pin));
}

void gpio_alternate_function_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_AF_t alternate)
{
	if (pin < 8)
	{
		GPIOx->AFR[0] = (GPIOx->AFR[0] & ~(15U << (4 * pin))) | ((uint32_t)alternate << (4 * pin));
	}
	else
	{
		uint8_t pos = pin - 8;
		GPIOx->AFR[1] = (GPIOx->AFR[1] & ~(15U << (4 * pos))) | ((uint32_t)alternate << (4 * pos));
	}
}

void gpio_init(GPIO_Config_t *cfg)
{
	gpio_enable_clock(cfg->port);
	gpio_mode_config(cfg->port, cfg->pin, cfg->mode);

	if (cfg->mode == GPIO_MODE_AF)
	{
		gpio_alternate_function_config(cfg->port, cfg->pin, cfg->alternate);
	}

	if (cfg->mode == GPIO_MODE_OUTPUT || cfg->mode == GPIO_MODE_AF)
	{
		gpio_output_type_config(cfg->port, cfg->pin, cfg->otype);
		gpio_speed_config(cfg->port, cfg->pin, cfg->speed);
	}

	if (cfg->mode != GPIO_MODE_ANALOG)
	{
		gpio_pull_config(cfg->port, cfg->pin, cfg->pull);
	}
}

void gpio_write(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_State_t state)
{
	if (state == GPIO_HIGH)
	{
		GPIOx->BSRR |= (1U << pin);
	}
	else if (state == GPIO_LOW)
	{
		GPIOx->BSRR |= (1U << (pin + 16));
	}
}

void gpio_toggle(GPIO_TypeDef *GPIOx, uint8_t pin)
{
	if (GPIOx->ODR & (1U << pin))
	{
		GPIOx->BSRR |= (1U << (pin + 16));
	}
	else
	{
		GPIOx->BSRR |= (1U << pin);
	}
}

void gpio_toggle_pin(GPIO_Config_t *GPIO)
{
	if (GPIO->port->ODR & (1U << GPIO->pin))
	{
		GPIO->port->BSRR |= (1U << (GPIO->pin + 16));
	}
	else
	{
		GPIO->port->BSRR |= (1U << GPIO->pin);
	}
}

GPIO_State_t gpio_read(GPIO_TypeDef *GPIOx, uint8_t pin)
{
	if (GPIOx->IDR & (1U << pin))
	{
		return GPIO_HIGH;
	}
	else
	{
		return GPIO_LOW;
	}
}
