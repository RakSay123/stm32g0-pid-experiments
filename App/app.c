/*
 * Application behavior belongs here.
 *
 * This file should describe what the firmware does,
 * not how peripherals are configured.
 */

#include "app.h"

#include <stddef.h>

#include "app_config.h"
#include "device_instances.h"

#include "systick/systick.h"
#include "uart/uart.h"

#include "led/led.h"
#include "motor_driver/motor_driver.h"
#include "dc_motor/dc_motor.h"

static DC_MOTOR_t *dc_motor;
static ROTARY_ENCODER_t *encoder;
static MOTOR_CONTROLLER_t *controller;

static uint32_t previous_led_toggle_ms;
static uint32_t previous_encoder_print_ms;
static uint32_t previous_controller_update_ms;

static void app_print_encoder(uint32_t current_ms)
{
	if (current_ms - previous_encoder_print_ms < APP_ENCODER_PRINT_PERIOD_MS) return;
	previous_encoder_print_ms = current_ms;

	uart_write_float(USART2, 180.00f);
	uart_write_str(USART2, ", ");

	float measured =  rotary_encoder_get_rpm(controller->encoder);
	uart_write_float(USART2, measured);
	uart_write_str(USART2, ", ");

	float error = 180.00f - measured;
	uart_write_float(USART2, error);
	uart_write_str(USART2, ", ");

	uart_write_float(USART2, (float)TIM16->CCR1);

	uart_write_line(USART2, "");
}

static void app_update_status_led(uint32_t current_ms)
{
	if (current_ms - previous_led_toggle_ms < APP_STATUS_LED_PERIOD_MS) return;

	previous_led_toggle_ms = current_ms;
	led_toggle(board_get_status_led());
}

static void app_controller_update(uint32_t current_ms)
{
	if (current_ms - previous_controller_update_ms < 20) return;
	previous_controller_update_ms = current_ms;

	motor_controller_update(controller, current_ms);
}

void app_init(void)
{
	dc_motor = board_get_motor();
	if (dc_motor == NULL) return;

	encoder = board_get_motor_encoder();
	if (encoder == NULL) return;

	controller = board_get_motor_controller();
	if (controller == NULL) return;

	rotary_encoder_set_count_zero(encoder);

	uint32_t current_ms = millis();

	previous_led_toggle_ms = current_ms;
	previous_encoder_print_ms = current_ms;
	previous_controller_update_ms = current_ms;

	motor_controller_set_rpm(controller, 180);

	uart_write_line(USART2, "SUCCESSFUL BOOT");

	uart_write_line(USART2, "\r\ntarget_rpm, measured_rpm, error, pwm");
	systick_delay_s(2);
}

void app_update(void)
{
	uint32_t current_ms = millis();

	app_update_status_led(current_ms);
	app_controller_update(current_ms);
	app_print_encoder(current_ms);
}
