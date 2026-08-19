#include "gpio.h"

GPIO_Status_t gpio_enable_clock(GPIO_TypeDef *GPIOx)
{

	if (GPIOx == GPIOA) RCC->IOPENR |= (1U << 0);
	else if (GPIOx == GPIOB) RCC->IOPENR |= (1U << 1);
	else if (GPIOx == GPIOC) RCC->IOPENR |= (1U << 2);
	else if (GPIOx == GPIOD) RCC->IOPENR |= (1U << 3);
#ifdef GPIOE
	else if (GPIOx == GPIOE) RCC->IOPENR |= (1U << 4);
#endif
	else if (GPIOx == GPIOF) RCC->IOPENR |= (1U << 5);
	else return GPIO_ERR;

	return GPIO_OK;
}

GPIO_Status_t gpio_mode_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Mode_t mode)
{
	if (mode > GPIO_MODE_ANALOG) return GPIO_ERR;

	GPIOx->MODER = (GPIOx->MODER & ~(3U << (2 * pin))) | ((uint32_t)mode << (2 * pin));

	return GPIO_OK;
}

GPIO_Status_t gpio_output_type_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_OType_t otype)
{
	if (otype > GPIO_OPEN_DRAIN) return GPIO_ERR;

	if (otype == GPIO_OPEN_DRAIN) GPIOx->OTYPER |= (1U << pin);
	else GPIOx->OTYPER &= ~(1U << pin);

	return GPIO_OK;
}

GPIO_Status_t gpio_speed_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Speed_t speed)
{
	if (speed > GPIO_SPEED_VERY_HIGH) return GPIO_ERR;

	GPIOx->OSPEEDR = (GPIOx->OSPEEDR & ~(3U << (2 * pin))) | ((uint32_t)speed << (2 * pin));

	return GPIO_OK;
}

GPIO_Status_t gpio_pull_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Pull_t pull)
{
	if (pull > GPIO_PULL_DOWN) return GPIO_ERR;

	GPIOx->PUPDR = (GPIOx->PUPDR & ~(3U << (2 * pin))) | ((uint32_t)pull << (2 * pin));

	return GPIO_OK;
}

GPIO_Status_t gpio_alternate_function_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_AF_t alternate)
{
	if (alternate > GPIO_AF7) return GPIO_ERR;

	if (pin < 8) GPIOx->AFR[0] = (GPIOx->AFR[0] & ~(15U << (4 * pin))) | ((uint32_t)alternate << (4 * pin));
	else
	{
		uint8_t pos = pin - 8;
		GPIOx->AFR[1] = (GPIOx->AFR[1] & ~(15U << (4 * pos))) | ((uint32_t)alternate << (4 * pos));
	}

	return GPIO_OK;
}

GPIO_Status_t gpio_init(GPIO_Config_t *cfg)
{
	GPIO_Status_t status = GPIO_OK;

	status = gpio_enable_clock(cfg->port);
	if (status != GPIO_OK) return status;

	status = gpio_mode_config(cfg->port, cfg->pin, cfg->mode);
	if (status != GPIO_OK) return status;

	if (cfg->mode == GPIO_MODE_AF)
	{
		status = gpio_alternate_function_config(cfg->port, cfg->pin, cfg->alternate);
		if (status != GPIO_OK) return status;
	}

	if (cfg->mode == GPIO_MODE_OUTPUT || cfg->mode == GPIO_MODE_AF)
	{
		status = gpio_output_type_config(cfg->port, cfg->pin, cfg->otype);
		if (status != GPIO_OK) return status;

		status = gpio_speed_config(cfg->port, cfg->pin, cfg->speed);
		if (status != GPIO_OK) return status;
	}

	if (cfg->mode != GPIO_MODE_ANALOG)
	{
		status = gpio_pull_config(cfg->port, cfg->pin, cfg->pull);
		if (status != GPIO_OK) return status;
	}

	return status;
}

GPIO_Status_t gpio_write(GPIO_Pin_t *gpio, GPIO_State_t state)
{
	if (state == GPIO_HIGH) gpio->port->BSRR |= (1U << gpio->pin);
	else if (state == GPIO_LOW) gpio->port->BSRR |= (1U << (gpio->pin + 16));
	else return GPIO_ERR;

	return GPIO_OK;
}

GPIO_Status_t gpio_toggle(GPIO_Pin_t *gpio)
{
	if (gpio->port->ODR & (1U << gpio->pin)) gpio->port->BSRR |= (1U << (gpio->pin + 16));
	else gpio->port->BSRR |= (1U << gpio->pin);

	return GPIO_OK;
}

GPIO_Status_t gpio_toggle_pin(GPIO_Config_t *cfg)
{
	if (cfg->port->ODR & (1U << cfg->pin))
	{
		cfg->port->BSRR |= (1U << (cfg->pin + 16));
	}
	else
	{
		cfg->port->BSRR |= (1U << cfg->pin);
	}

	return GPIO_OK;
}

GPIO_State_t gpio_read(GPIO_Pin_t *gpio)
{
	if (gpio->port->IDR & (1U << gpio->pin))
	{
		return GPIO_HIGH;
	}
	else
	{
		return GPIO_LOW;
	}
}

GPIO_State_t gpio_read_pin(GPIO_Config_t *cfg)
{
	GPIO_Pin_t gpio = {
		.port = cfg->port,
		.pin = cfg->pin
	};

	return gpio_read(&gpio);
}
