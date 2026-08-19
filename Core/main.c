/*
 * Firmware entry point.
 *
 * Responsibilities:
 * - Initialize the board hardware.
 * - Initialize the application state.
 * - Execute the application loop forever.
 *
 * This file should remain as small and stable as possible.
 */

#include "board_init.h"
#include "app.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

int main(void)
{
	const BOARD_Status_t board_status = board_init();
	if (board_status != BOARD_STATUS_OK) while (1);

	app_init();

	while (1)
	{
		app_update();
	}
}
