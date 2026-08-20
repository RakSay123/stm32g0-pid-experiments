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
#include "uart/uart.h"

#include "led/led.h"
#include "motor_driver/motor_driver.h"
#include "tb6612fng/tb6612fng.h"
#include "tb6612fng/tb6612fng_motor_driver.h"
#include "dc_motor/dc_motor.h"

static GPIO_Config_t usart2_tx = {
	.port = GPIOA,
	.pin = 2,
	.mode = GPIO_MODE_AF,
	.otype = GPIO_PUSH_PULL,
	.speed = GPIO_SPEED_HIGH,
	.pull = GPIO_NO_PULL,
	.alternate = GPIO_AF1
};

static GPIO_Config_t usart2_rx = {
	.port = GPIOA,
	.pin = 3,
	.mode = GPIO_MODE_AF,
	.otype = GPIO_PUSH_PULL,
	.speed = GPIO_SPEED_HIGH,
	.pull = GPIO_NO_PULL,
	.alternate = GPIO_AF1
};

static UART_Config_t usart2_cfg = {
	.USARTx = USART2,
	.fclk = BOARD_FCLK_HZ,
	.baud_rate = BOARD_DEBUG_UART_BAUD,
};

static TIM_Config_t tim3_cfg = {
	.TIMx = TIM3,
	.psc = BOARD_TIM3_PSC,
	.arr = BOARD_TIM3_ARR,
	.cnt = BOARD_TIM3_CNT,
	.interrupt_status = TIM_INTERRUPTS_DISABLED
};

static TIM_Config_t tim16_cfg = {
	.TIMx = TIM16,
	.psc = BOARD_TIM16_PSC,
	.arr = BOARD_TIM16_ARR,
	.cnt = BOARD_TIM16_CNT,
	.interrupt_status = TIM_INTERRUPTS_DISABLED
};

static TIM_Config_t tim17_cfg = {
	.TIMx = TIM16,
	.psc = BOARD_TIM17_PSC,
	.arr = BOARD_TIM17_ARR,
	.cnt = BOARD_TIM17_CNT,
	.interrupt_status = TIM_INTERRUPTS_DISABLED
};

BOARD_Status_t board_init(void)
{
	LED_t *status_led = board_get_status_led();
	ROTARY_ENCODER_t *motor_encoder = board_get_motor_encoder();
	TB6612FNG_t *tb6612fng = board_get_tb6612fng();
	MOTOR_DRIVER_t *motor_driver = board_get_motor_driver();
	DC_MOTOR_t *motor = board_get_motor();

	if (status_led == NULL) return BOARD_STATUS_LED_ERROR;
	if (tb6612fng == NULL) return BOARD_STATUS_TB6612FNG_ERROR;
	if (motor_driver == NULL) return BOARD_STATUS_MOTOR_DRIVER_ERROR;
	if (motor == NULL) return BOARD_STATUS_DC_MOTOR_ERROR;


	if (gpio_init(&usart2_tx) != GPIO_OK) return BOARD_STATUS_GPIO_ERROR;
	if (gpio_init(&usart2_rx) != GPIO_OK) return BOARD_STATUS_GPIO_ERROR;
	systick_init(BOARD_FCLK_HZ / BOARD_SYSTICK_HZ);
	uart_init(&usart2_cfg);
	if (timer_init(&tim3_cfg) != TIM_OK) return BOARD_STATUS_TIM3_ERROR;
	if (timer_init(&tim16_cfg) != TIM_OK) return BOARD_STATUS_TIM16_ERROR;
	if (timer_init(&tim17_cfg) != TIM_OK) return BOARD_STATUS_TIM17_ERROR;
	systick_init(BOARD_FCLK_HZ / BOARD_SYSTICK_HZ);

	led_init(status_led); // void return type at the moment
	if (rotary_encoder_init(motor_encoder) != ROTARY_ENCODER_OK) return BOARD_STATUS_ENCODER_ERROR;
	if (tb6612fng_init(tb6612fng) != TB6612FNG_OK) return BOARD_STATUS_TB6612FNG_ERROR;
	if (tb6612fng_motor_driver_bind(motor_driver, tb6612fng) != MOTOR_DRIVER_OK) return BOARD_STATUS_MOTOR_DRIVER_ERROR;
	if (dc_motor_init(motor) != DC_MOTOR_OK) return BOARD_STATUS_DC_MOTOR_ERROR;

	return BOARD_STATUS_OK;
}
