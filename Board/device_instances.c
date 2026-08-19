/*
 * Physical hardware descriptions.
 *
 * This file maps peripherals, GPIOs, timers, and devices
 * to the current board.
 */

#include "device_instances.h"

static LED_t status_led = {
	.port = GPIOA,
	.pin = 5,
	.mode = LED_MODE_GPIO,
};

LED_t* board_get_status_led(void)
{
	return &status_led;
}
