#include "systick.h"

#define ENABLE (1U << 0)
#define TICKINT (1U << 1)
#define CLK_SOURCE (1U << 2)
#define COUNTFLAG (1U << 16)

volatile uint32_t systick_ms = 0;

void systick_init(uint32_t ticks)
{
	SysTick->LOAD = ticks - 1; 						// Systick Reload Value Register
	SysTick->VAL = 0; 								// Systick Current Value Register
	SysTick->CTRL = ENABLE | TICKINT | CLK_SOURCE;	// Systick Control and Status register
}

void SysTick_Handler(void)
{
	systick_ms++;
}

void systick_delay_ms(uint32_t ms)
{
	for (uint32_t i = 0; i < ms; i++)
	{
		while (!(SysTick->CTRL & COUNTFLAG));
	}
}

void systick_delay_s(uint32_t s)
{
	systick_delay_ms(s * 1000);
}

uint32_t millis(void)
{
	return systick_ms;
}
