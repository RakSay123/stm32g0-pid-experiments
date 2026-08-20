/*
 * Accessors for board-specific hardware instances.
 *
 * Applications should retrieve devices through these functions
 * rather than referencing global objects directly.
 */

#pragma once

#include "led/led.h"
#include "tb6612fng/tb6612fng.h"
#include "tb6612fng/tb6612fng_motor_driver.h"
#include "dc_motor/dc_motor.h"

LED_t* board_get_status_led(void);

TB6612FNG_t* board_get_tb6612fng(void);

MOTOR_DRIVER_t* board_get_motor_driver(void);

DC_MOTOR_t* board_get_motor(void);
