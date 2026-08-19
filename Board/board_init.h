/*
 * Board initialization interface.
 *
 * board_init() initializes every hardware resource required by
 * the application before execution begins.
 */

#pragma once

typedef enum {
	BOARD_STATUS_OK = 0,
	BOARD_STATUS_LED_ERROR,
	BOARD_STATUS_SYSTICK_ERROR
} BOARD_Status_t;

BOARD_Status_t board_init(void);
