#pragma once

#include "gpio/gpio.h"
#include "timer/timer.h"
#include "timer/timer_pwm/timer_pwm.h"

typedef enum {
	LED_MODE_GPIO,
	LED_MODE_PWM
} LED_Mode_t;

typedef struct {
	GPIO_TypeDef *port;
	uint8_t pin;
	LED_Mode_t mode;

	GPIO_AF_t alternate;
	TIM_PWM_Config_t *pwm_cfg;
} LED_t;

void led_init(LED_t *led);

void led_toggle(LED_t *led);

void led_on(LED_t *led);

void led_off(LED_t *led);

void led_set_brightness(LED_t *led, uint32_t brightness);
