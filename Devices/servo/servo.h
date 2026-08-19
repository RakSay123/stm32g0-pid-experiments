#pragma once

#include "stm32g070xx.h"
#include "gpio/gpio.h"
#include "timer/timer_pwm/timer_pwm.h"

typedef struct {
	GPIO_TypeDef *port;
	uint8_t pin;
	GPIO_AF_t alternate;

	TIM_PWM_Config_t *pwm_cfg;

	uint16_t min_pulse_us;
	uint16_t max_pulse_us;

	int16_t min_angle;
	int16_t max_angle;
} SERVO_t;

void servo_init(SERVO_t *servo);

void servo_set_min_angle(SERVO_t *servo);

void servo_set_zero(SERVO_t *servo);

void servo_set_max_angle(SERVO_t *servo);

void servo_set_angle(SERVO_t *servo, int16_t angle);

void servo_set_pulse(SERVO_t *servo, uint16_t pulse_us);
