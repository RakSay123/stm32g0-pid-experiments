/*
 * Application behavior belongs here.
 *
 * PWM vs RPM characterization experiment.
 *
 * Sweep:
 *   0% -> 100% PWM in 1% increments
 *   100% -> 0% PWM in 1% increments
 *
 * Each duty cycle is held for 5 seconds.
 * RPM samples from the final 1 second are averaged and
 * printed as a CSV row.
 */

#include "app.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "app_config.h"
#include "device_instances.h"

#include "systick/systick.h"
#include "uart/uart.h"

#include "led/led.h"
#include "motor_driver/motor_driver.h"
#include "dc_motor/dc_motor.h"


#define APP_PWM_ARR_VALUE              1000U
#define APP_PWM_STEP                   10U        /* 1% of ARR = 10 */
#define APP_PWM_MIN                    0U
#define APP_PWM_MAX                    1000U

#define APP_PWM_DWELL_TIME_MS          5000U
#define APP_PWM_SAMPLE_START_MS        4000U      /* Average final 1 second */


typedef enum
{
	APP_SWEEP_UP,
	APP_SWEEP_DOWN,
	APP_SWEEP_COMPLETE
} APP_Sweep_State_t;


static DC_MOTOR_t *dc_motor;
static ROTARY_ENCODER_t *encoder;

static APP_Sweep_State_t sweep_state;

static uint32_t current_pwm;
static uint32_t sweep_step_start_ms;

static uint32_t previous_led_toggle_ms;
static uint32_t previous_encoder_update_ms;

static float rpm_sum;
static uint32_t rpm_sample_count;

static bool rotary_encoder_update_success;


/*
 * Print one CSV row:
 *
 * phase,duty_percent,pwm,rpm,samples
 */
static void app_print_csv_result(void)
{
	float duty_percent =
			((float) current_pwm / (float) APP_PWM_ARR_VALUE) * 100.0f;

	float average_rpm = 0.0f;

	if (rpm_sample_count > 0U)
	{
		average_rpm = rpm_sum / (float) rpm_sample_count;
	}

	if (sweep_state == APP_SWEEP_UP)
	{
		uart_write_str(USART2, "UP,");
	}
	else if (sweep_state == APP_SWEEP_DOWN)
	{
		uart_write_str(USART2, "DOWN,");
	}
	else
	{
		uart_write_str(USART2, "COMPLETE,");
	}

	uart_write_float(USART2, duty_percent);
	uart_write_str(USART2, ",");

	uart_write_int(USART2, current_pwm);
	uart_write_str(USART2, ",");

	uart_write_float(USART2, average_rpm);
	uart_write_str(USART2, ",");

	uart_write_int(USART2, rpm_sample_count);
	uart_write_line(USART2, "");
}


static void app_reset_rpm_samples(void)
{
	rpm_sum = 0.0f;
	rpm_sample_count = 0U;
}


static void app_apply_pwm(void)
{
	dc_motor_set_speed_and_direction(
			dc_motor,
			MOTOR_DRIVER_FORWARD,
			current_pwm
	);
}


static void app_update_status_led(uint32_t current_ms)
{
	if (current_ms - previous_led_toggle_ms < APP_STATUS_LED_PERIOD_MS)
	{
		return;
	}

	previous_led_toggle_ms = current_ms;

	led_toggle(board_get_status_led());
}


/*
 * Keep the encoder velocity estimate updating normally.
 *
 * During the final second of each PWM step, accumulate RPM
 * samples for averaging.
 */
static void app_update_encoder(uint32_t current_ms)
{
	rotary_encoder_update_distance(encoder);

	if (current_ms - previous_encoder_update_ms < APP_ENCODER_UPDATE_PERIOD_MS)
	{
		return;
	}

	previous_encoder_update_ms = current_ms;

	rotary_encoder_update_success =
			rotary_encoder_update_velocity(encoder, current_ms)
			== ROTARY_ENCODER_OK;

	if (!rotary_encoder_update_success)
	{
		return;
	}

	uint32_t elapsed_ms = current_ms - sweep_step_start_ms;

	if (elapsed_ms >= APP_PWM_SAMPLE_START_MS)
	{
		rpm_sum += rotary_encoder_get_rpm(encoder);
		rpm_sample_count++;
	}
}


static void app_advance_sweep(uint32_t current_ms)
{
	/*
	 * First report the PWM level that just completed its
	 * 5-second measurement window.
	 */
	app_print_csv_result();

	/*
	 * Then determine the next PWM command.
	 */
	switch (sweep_state)
	{
		case APP_SWEEP_UP:

			if (current_pwm >= APP_PWM_MAX)
			{
				sweep_state = APP_SWEEP_DOWN;

				/*
				 * 100% was already measured on the way up,
				 * so begin downward sweep at 99%.
				 */
				current_pwm = APP_PWM_MAX - APP_PWM_STEP;
			}
			else
			{
				current_pwm += APP_PWM_STEP;
			}

			break;


		case APP_SWEEP_DOWN:

			if (current_pwm <= APP_PWM_MIN)
			{
				sweep_state = APP_SWEEP_COMPLETE;

				dc_motor_coast(dc_motor);

				uart_write_line(USART2, "=== EXPERIMENT COMPLETE ===");

				return;
			}

			current_pwm -= APP_PWM_STEP;

			break;


		case APP_SWEEP_COMPLETE:
		default:
			return;
	}


	app_reset_rpm_samples();

	sweep_step_start_ms = current_ms;

	app_apply_pwm();
}


static void app_update_sweep(uint32_t current_ms)
{
	if (sweep_state == APP_SWEEP_COMPLETE)
	{
		return;
	}

	if (current_ms - sweep_step_start_ms < APP_PWM_DWELL_TIME_MS)
	{
		return;
	}

	app_advance_sweep(current_ms);
}


void app_init(void)
{
	dc_motor = board_get_motor();

	if (dc_motor == NULL)
	{
		return;
	}

	encoder = board_get_motor_encoder();

	if (encoder == NULL)
	{
		return;
	}


	rotary_encoder_update_success =
			rotary_encoder_set_count_zero(encoder)
			== ROTARY_ENCODER_OK;


	uint32_t current_ms = millis();

	previous_led_toggle_ms = current_ms;
	previous_encoder_update_ms = current_ms;

	sweep_step_start_ms = current_ms;

	current_pwm = APP_PWM_MIN;

	sweep_state = APP_SWEEP_UP;

	app_reset_rpm_samples();


	uart_write_line(USART2, "=== PWM VS RPM NO-LOAD EXPERIMENT ===");
	uart_write_line(USART2, "phase,duty_percent,pwm,rpm,samples");

	app_apply_pwm();
}


void app_update(void)
{
	uint32_t current_ms = millis();

	app_update_status_led(current_ms);
	app_update_encoder(current_ms);
	app_update_sweep(current_ms);
}
