#include "led.h"

void led_init(LED_t *led)
{
	if (led->mode == LED_MODE_GPIO)
	{
		GPIO_Config_t cfg = {
			.port = led->port,
			.pin = led->pin,
			.mode = GPIO_MODE_OUTPUT,
			.otype = GPIO_PUSH_PULL,
			.speed = GPIO_SPEED_HIGH,
			.pull = GPIO_NO_PULL,
			.alternate = GPIO_AF0
		};
		gpio_init(&cfg);
	}
	else if (led->mode == LED_MODE_PWM)
	{
		GPIO_Config_t cfg = {
			.port = led->port,
			.pin = led->pin,
			.mode = GPIO_MODE_AF,
			.otype = GPIO_PUSH_PULL,
			.speed = GPIO_SPEED_HIGH,
			.pull = GPIO_NO_PULL,
			.alternate = led->alternate
		};

		gpio_init(&cfg);
		timer_pwm_init(led->pwm_cfg);
	}
	else
	{
		// invalid mode
	}

}

void led_toggle(LED_t *led)
{
	if (led->mode != LED_MODE_GPIO) return;

	gpio_toggle(led->port, led->pin);
}

void led_on(LED_t *led)
{
	if (led->mode != LED_MODE_GPIO) return;

	gpio_write(led->port, led->pin, GPIO_HIGH);
}

void led_off(LED_t *led)
{
	if (led->mode != LED_MODE_GPIO) return;

	gpio_write(led->port, led->pin, GPIO_LOW);
}

void led_set_brightness(LED_t *led, uint32_t brightness)
{
	if (led->mode != LED_MODE_PWM) return;

	timer_pwm_set_duty_cycle(led->pwm_cfg, brightness);
}
