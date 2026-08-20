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
static ROTARY_ENCODER_t *encoder;

static APP_Demo_State_t demo_state;

static uint32_t previous_led_toggle_ms;
static uint32_t previous_encoder_update_ms;
static uint32_t previous_encoder_print_ms;
static uint32_t previous_demo_state_ms;

static bool rotary_encoder_update_success;
static uint8_t stop_snapshots_printed;

static void app_print_encoder_position(USART_TypeDef *USARTx, ROTARY_ENCODER_t *rotary_encoder)
{
	uart_write_str(USARTx, "[ROTARY_ENCODER] Direction: ");
	if (rotary_encoder_get_direction(rotary_encoder) == TIM_ENCODER_DIRECTION_UP) uart_write_str(USARTx, "UP | ");
	else uart_write_str(USARTx, "DOWN | ");

	uart_write_str(USARTx, "Count: ");
	uart_write_int(USARTx, rotary_encoder_get_total_count(rotary_encoder));
	uart_write_str(USARTx, " | ");

	uart_write_str(USARTx, "Revolutions: ");
	uart_write_float(USARTx, rotary_encoder_get_revolutions(rotary_encoder));
	uart_write_str(USARTx, " | ");

	uart_write_str(USARTx, "Cumulative angle: ");
	uart_write_float(USARTx, rotary_encoder_get_cumulative_angle_degrees(rotary_encoder));
	uart_write_str(USARTx, " deg | ");

	uart_write_str(USARTx, "Normalized angle: ");
	uart_write_float(USARTx, rotary_encoder_get_normalized_angle_degrees(rotary_encoder));
	uart_write_line(USARTx, " deg");
}

static void app_print_encoder_distance(USART_TypeDef *USARTx, ROTARY_ENCODER_t *rotary_encoder)
{
	uart_write_str(USARTx, "[ROTARY_ENCODER] Displacement: ");
	uart_write_float(USARTx, rotary_encoder_get_displacement_mm(rotary_encoder));
	uart_write_str(USARTx, " mm | ");

	uart_write_str(USARTx, "Distance: ");
	uart_write_float(USARTx, rotary_encoder_get_total_distance_mm(rotary_encoder));
	uart_write_line(USARTx, " mm");
}

static void app_print_encoder_velocity(USART_TypeDef *USARTx, ROTARY_ENCODER_t *rotary_encoder)
{
	uart_write_str(USARTx, "[ROTARY_ENCODER] Rev/s: ");
	uart_write_float(USARTx, rotary_encoder_get_revolutions_per_second(rotary_encoder));
	uart_write_str(USARTx, " | ");

	uart_write_str(USARTx, "RPM: ");
	uart_write_float(USARTx, rotary_encoder_get_rpm(rotary_encoder));
	uart_write_str(USARTx, " | ");

	uart_write_str(USARTx, "Deg/s: ");
	uart_write_float(USARTx, rotary_encoder_get_degrees_per_second(rotary_encoder));
	uart_write_str(USARTx, " | ");

	uart_write_str(USARTx, "Velocity: ");
	uart_write_float(USARTx, rotary_encoder_get_linear_velocity_mm_per_second(rotary_encoder));
	uart_write_str(USARTx, " mm/s | ");

	uart_write_str(USARTx, "Top speed: ");
	uart_write_float(USARTx, rotary_encoder_get_top_speed_mm_per_second(rotary_encoder));
	uart_write_str(USARTx, " mm/s | ");

	uart_write_str(USARTx, "Motion: ");
	if (rotary_encoder->motion == ROTARY_ENCODER_UP) uart_write_line(USARTx, "UP");
	else if (rotary_encoder->motion == ROTARY_ENCODER_DOWN) uart_write_line(USARTx, "DOWN");
	else if (rotary_encoder->motion == ROTARY_ENCODER_STOPPED) uart_write_line(USARTx, "STOPPED");
	else uart_write_line(USARTx, "UNKNOWN");
}

static void app_print_encoder_all(USART_TypeDef *USARTx, ROTARY_ENCODER_t *rotary_encoder)
{
	app_print_encoder_position(USARTx, rotary_encoder);
	app_print_encoder_distance(USARTx, rotary_encoder);
	app_print_encoder_velocity(USARTx, rotary_encoder);
	uart_write_line(USARTx, "");
}

static void app_update_status_led(uint32_t current_ms)
{
	if (current_ms - previous_led_toggle_ms < APP_STATUS_LED_PERIOD_MS) return;

	previous_led_toggle_ms = current_ms;
	led_toggle(board_get_status_led());
}

static void app_update_encoder(uint32_t current_ms)
{
	rotary_encoder_update_distance(encoder);

	if (current_ms - previous_encoder_update_ms < APP_ENCODER_UPDATE_PERIOD_MS) return;

	previous_encoder_update_ms = current_ms;
	rotary_encoder_update_success = rotary_encoder_update_velocity(encoder, current_ms) == ROTARY_ENCODER_OK;
}

static void app_print_encoder(uint32_t current_ms)
{
	if (current_ms - previous_encoder_print_ms < APP_ENCODER_PRINT_PERIOD_MS) return;

	previous_encoder_print_ms = current_ms;

	if (!rotary_encoder_update_success)
	{
		uart_write_line(USART2, "[ROTARY_ENCODER] Update failed");
		return;
	}

	ROTARY_ENCODER_Motion_t motion = rotary_encoder_get_motion(encoder);

	if (motion == ROTARY_ENCODER_STOPPED)
	{
		if (stop_snapshots_printed >= APP_STOPPED_SNAPSHOT_LIMIT) return;
		stop_snapshots_printed++;
	}
	else
	{
		stop_snapshots_printed = 0U;
	}

	app_print_encoder_all(USART2, encoder);
}

static void app_demo_enter_state(APP_Demo_State_t state)
{
	switch (state)
	{
		case APP_DEMO_LOW_SPEED_CW:
			uart_write_line(USART2, "\r\n=== LOW SPEED CW ===");
			dc_motor_set_speed_and_direction(dc_motor, MOTOR_DRIVER_FORWARD, APP_MOTOR_LOW_SPEED);
			break;

		case APP_DEMO_FULL_SPEED_CW:
			uart_write_line(USART2, "\r\n=== FULL LOW SPEED CW ===");
			dc_motor_set_speed_and_direction(dc_motor, MOTOR_DRIVER_FORWARD, APP_MOTOR_FULL_SPEED);
			break;

		case APP_DEMO_COAST:
			uart_write_line(USART2, "\r\n=== COAST ===");
			dc_motor_coast(dc_motor);
			break;

		case APP_DEMO_LOW_SPEED_CCW:
			uart_write_line(USART2, "\r\n=== LOW SPEED CCW ===");
			dc_motor_set_speed_and_direction(dc_motor, MOTOR_DRIVER_REVERSE, APP_MOTOR_LOW_SPEED);
			break;

		case APP_DEMO_FULL_SPEED_CCW:
			uart_write_line(USART2, "\r\n=== FULL SPEED CCW ===");
			dc_motor_set_speed_and_direction(dc_motor, MOTOR_DRIVER_REVERSE, APP_MOTOR_FULL_SPEED);
			break;

		case APP_DEMO_BRAKE:
			uart_write_line(USART2, "\r\n=== BRAKE ===");
			dc_motor_brake(dc_motor);
			break;

		case APP_DEMO_COMPLETE:
			uart_write_line(USART2, "\r\n=== COMPLETE ===");
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

	encoder = board_get_motor_encoder();
	if (encoder == NULL) return;

	rotary_encoder_update_success = rotary_encoder_set_count_zero(encoder) == ROTARY_ENCODER_OK;
	demo_state = APP_DEMO_LOW_SPEED_CW;

	uint32_t current_ms = millis();

	previous_led_toggle_ms = current_ms;
	previous_encoder_update_ms = current_ms;
	previous_encoder_print_ms = current_ms;
	previous_demo_state_ms = current_ms;


	stop_snapshots_printed = 0U;

	uart_write_line(USART2, "SUCCESSFUL BOOT");
	app_demo_enter_state(demo_state);
}

void app_update(void)
{
	uint32_t current_ms = millis();

	app_update_status_led(current_ms);
	app_update_encoder(current_ms);
	app_print_encoder(current_ms);
	app_update_demo(current_ms);
}
