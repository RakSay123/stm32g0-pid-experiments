/*
 * Board initialization interface.
 *
 * board_init() initializes every hardware resource required by
 * the application before execution begins.
 */

#pragma once

typedef enum {
	BOARD_STATUS_OK = 0,
	BOARD_STATUS_GPIO_ERROR,
	BOARD_STATUS_SYSTICK_ERROR,
	BOARD_STATUS_TIM3_ERROR,
	BOARD_STATUS_TIM16_ERROR,
	BOARD_STATUS_TIM17_ERROR,
	BOARD_STATUS_LED_ERROR,
	BOARD_STATUS_ENCODER_ERROR,
	BOARD_STATUS_TB6612FNG_ERROR,
	BOARD_STATUS_MOTOR_DRIVER_ERROR,
	BOARD_STATUS_DC_MOTOR_ERROR,
	BOARD_STATUS_MOTOR_CONTROLLER_ERROR,
} BOARD_Status_t;

BOARD_Status_t board_init(void);
