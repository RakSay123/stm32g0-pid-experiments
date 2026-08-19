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

#include "systick/systick.h"

#include "led/led.h"

BOARD_Status_t board_init(void)
{
	LED_t *status_led = board_get_status_led();
	if (status_led == NULL) return BOARD_STATUS_LED_ERROR;

	systick_init(BOARD_FCLK_HZ / BOARD_SYSTICK_HZ);

	led_init(status_led); // void return type at the moment

	return BOARD_STATUS_OK;
}
