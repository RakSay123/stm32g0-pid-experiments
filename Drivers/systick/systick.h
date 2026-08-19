#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <stdint.h>
#include "stm32g070xx.h"

void systick_init(uint32_t ticks);

void systick_delay_ms(uint32_t ms);

void systick_delay_s(uint32_t s);

uint32_t millis(void);

#endif /* SYSTICK_H_ */
