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

#include "led/led.h"
#include "motor_driver/motor_driver.h"
#include "dc_motor/dc_motor.h"

typedef enum {
	APP_DEMO_LOW_SPEED_CW,
	APP_DEMO_FULL_SPEED_CW,
	APP_DEMO_COAST,
	APP_DEMO_LOW_SPEED_CCW,
	APP_DEMO_FULL_SPEED_CCW,
	APP_DEMO_BRAKE,
	APP_DEMO_COMPLETE
} APP_Demo_State_t;

static DC_MOTOR_t *dc_motor;

static APP_Demo_State_t demo_state;

static uint32_t previous_led_toggle_ms;
static uint32_t previous_demo_state_ms;

static void app_update_status_led(uint32_t current_ms)
{
	if (current_ms - previous_led_toggle_ms < APP_STATUS_LED_PERIOD_MS) return;

	previous_led_toggle_ms = current_ms;
	led_toggle(board_get_status_led());
}

static void app_demo_enter_state(APP_Demo_State_t state)
{
	switch (state)
	{
		case APP_DEMO_LOW_SPEED_CW:
			dc_motor_set_speed_and_direction(dc_motor, MOTOR_DRIVER_FORWARD, APP_MOTOR_LOW_SPEED);
			break;

		case APP_DEMO_FULL_SPEED_CW:
			dc_motor_set_speed_and_direction(dc_motor, MOTOR_DRIVER_FORWARD, APP_MOTOR_FULL_SPEED);
			break;

		case APP_DEMO_COAST:
			dc_motor_coast(dc_motor);
			break;

		case APP_DEMO_LOW_SPEED_CCW:
			dc_motor_set_speed_and_direction(dc_motor, MOTOR_DRIVER_REVERSE, APP_MOTOR_LOW_SPEED);
			break;

		case APP_DEMO_FULL_SPEED_CCW:
			dc_motor_set_speed_and_direction(dc_motor, MOTOR_DRIVER_REVERSE, APP_MOTOR_FULL_SPEED);
			break;

		case APP_DEMO_BRAKE:
			dc_motor_brake(dc_motor);
			break;

		case APP_DEMO_COMPLETE:
			dc_motor_coast(dc_motor);
			break;

		default:
			break;
	}
}

static APP_Demo_State_t app_demo_switch_state(APP_Demo_State_t state)
{
	switch (state)
	{
		case APP_DEMO_LOW_SPEED_CW:
			return APP_DEMO_FULL_SPEED_CW;

		case APP_DEMO_FULL_SPEED_CW:
			return APP_DEMO_COAST;

		case APP_DEMO_COAST:
			return APP_DEMO_LOW_SPEED_CCW;

		case APP_DEMO_LOW_SPEED_CCW:
			return APP_DEMO_FULL_SPEED_CCW;

		case APP_DEMO_FULL_SPEED_CCW:
			return APP_DEMO_BRAKE;

		case APP_DEMO_BRAKE:
			return APP_DEMO_COMPLETE;

		case APP_DEMO_COMPLETE:
		default:
			return APP_DEMO_LOW_SPEED_CW;
	}

	return state;
}

static void app_update_demo(uint32_t current_ms)
{
	if (demo_state == APP_DEMO_COMPLETE) return;
	if (current_ms - previous_demo_state_ms < APP_DEMO_STATE_DURATION_MS) return;

	previous_demo_state_ms = current_ms;

	demo_state = app_demo_switch_state(demo_state);
	app_demo_enter_state(demo_state);
}

void app_init(void)
{
	dc_motor = board_get_motor();
	if (dc_motor == NULL) return;

	demo_state = APP_DEMO_LOW_SPEED_CW;

	uint32_t current_ms = millis();

	previous_led_toggle_ms = current_ms;
	previous_demo_state_ms = current_ms;

	app_demo_enter_state(demo_state);
}

void app_update(void)
{
	uint32_t current_ms = millis();

	app_update_status_led(current_ms);
	app_update_demo(current_ms);
}
