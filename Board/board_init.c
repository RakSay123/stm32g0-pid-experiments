/*
 * Hardware initialization.
 *
 * Initializes board resources in dependency order and reports
 * failures through BOARD_Status_t.
 */

#include "board_init.h"

#include <stdint.h>
#include <stddef.h>

#include "board_config.h"
#include "device_instances.h"

#include "gpio/gpio.h"
#include "systick/systick.h"
#include "timer/timer.h"
#include "timer/timer_pwm/timer_pwm.h"
#include "systick/systick.h"

#include "led/led.h"
#include "motor_driver/motor_driver.h"
#include "tb6612fng/tb6612fng.h"
#include "tb6612fng/tb6612fng_motor_driver.h"
#include "dc_motor/dc_motor.h"

static TIM_Config_t tim16_cfg = {
	.TIMx = TIM16,
	.psc = BOARD_TIM16_PSC,
	.arr = BOARD_TIM16_ARR,
	.cnt = BOARD_TIM16_CNT,
	.interrupt_status = TIM_INTERRUPTS_DISABLED
};

BOARD_Status_t board_init(void)
{
	LED_t *status_led = board_get_status_led();
	TB6612FNG_t *tb6612fng = board_get_tb6612fng();
	MOTOR_DRIVER_t *motor_driver = board_get_motor_driver();
	DC_MOTOR_t *motor = board_get_motor();

	if (status_led == NULL) return BOARD_STATUS_LED_ERROR;
	if (tb6612fng == NULL) return BOARD_STATUS_TB6612FNG_ERROR;
	if (motor_driver == NULL) return BOARD_STATUS_MOTOR_DRIVER_ERROR;
	if (motor == NULL) return BOARD_STATUS_DC_MOTOR_ERROR;


	if (timer_init(&tim16_cfg) != TIM_OK) return BOARD_STATUS_TIM16_ERROR;
	systick_init(BOARD_FCLK_HZ / BOARD_SYSTICK_HZ);

	led_init(status_led); // void return type at the moment
	if (tb6612fng_init(tb6612fng) != TB6612FNG_OK) return BOARD_STATUS_TB6612FNG_ERROR;
	if (dc_motor_init(motor) != DC_MOTOR_OK) return BOARD_STATUS_DC_MOTOR_ERROR;

	return BOARD_STATUS_OK;
}
