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

static LED_t *led;
static DC_MOTOR_t *dc_motor;
static ROTARY_ENCODER_t *encoder;
static MOTOR_CONTROLLER_t *controller;

static uint32_t previous_led_toggle_ms;
static uint32_t previous_encoder_print_ms;
static uint32_t previous_controller_update_ms;

static void app_update_status_led(uint32_t current_ms)
{
	if (current_ms - previous_led_toggle_ms < APP_STATUS_LED_PERIOD_MS) return;

	previous_led_toggle_ms = current_ms;
	led_toggle(led);
}

static void app_controller_update(uint32_t current_ms)
{
	uint32_t elapsed_ms = current_ms - previous_controller_update_ms;

	if (elapsed_ms < 20) return;

	previous_controller_update_ms = current_ms;

	float dt_seconds = (float)elapsed_ms / 1000.0f;

	motor_controller_update(controller, current_ms, dt_seconds);
}

void app_init(void)
{
	led = board_get_status_led();
	if (led == NULL) return;

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

//	uart_write_line(USART2, "SUCCESSFUL BOOT");

	uart_write_line(USART2, "\r\ntime_ms,target_rpm,measured_rpm,error,pwm");
//	systick_delay_s(2);
}

void app_update(void)
{
	uint32_t current_ms = millis();

	app_update_status_led(current_ms);
	app_controller_update(current_ms);

	while (current_ms >= 10000)
	{
		dc_motor_brake(dc_motor);
		led_off(led);
	}
}
