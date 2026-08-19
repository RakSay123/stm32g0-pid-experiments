#include "servo.h"

void servo_init(SERVO_t *servo)
{
	GPIO_Config_t cfg = {
		.port = servo->port,
		.pin = servo->pin,
		.mode = GPIO_MODE_AF,
		.otype = GPIO_PUSH_PULL,
		.speed = GPIO_SPEED_HIGH,
		.pull = GPIO_NO_PULL,
		.alternate = servo->alternate
	};

	gpio_init(&cfg);
	timer_pwm_init(servo->pwm_cfg);
}

void servo_set_min_angle(SERVO_t *servo)
{
	servo_set_angle(servo, servo->min_angle);
}

void servo_set_zero(SERVO_t *servo)
{
	servo_set_angle(servo, 0);
}

void servo_set_max_angle(SERVO_t *servo)
{
	servo_set_angle(servo, servo->max_angle);
}

void servo_set_angle(SERVO_t *servo, int16_t angle)
{
	if (angle > servo->max_angle) angle = servo->max_angle;
	if (angle < servo->min_angle) angle = servo->min_angle;

	uint32_t angle_range = servo->max_angle - servo->min_angle;
	uint32_t pulse_range = servo->max_pulse_us - servo->min_pulse_us;

	uint32_t pulse_us = servo->min_pulse_us + ((uint32_t)(angle - servo->min_angle) * pulse_range) / angle_range;
	servo_set_pulse(servo, pulse_us);
}

void servo_set_pulse(SERVO_t *servo, uint16_t pulse_us)
{
	if (pulse_us > servo->max_pulse_us) pulse_us = servo->max_pulse_us;
	if (pulse_us < servo->min_pulse_us) pulse_us = servo->min_pulse_us;

	timer_pwm_set_compare_value(servo->pwm_cfg, pulse_us);
}
