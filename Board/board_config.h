/*
 * Board-wide compile-time configuration.
 *
 * Examples:
 * - UART baud rates
 * - Configuration constants
 */

#pragma once

#define BOARD_FCLK_HZ                 16000000U
#define BOARD_SYSTICK_HZ              1000U

#define BOARD_TIM16_PSC				  16 - 1
#define BOARD_TIM16_ARR               1000 - 1
#define BOARD_TIM16_CNT               0
