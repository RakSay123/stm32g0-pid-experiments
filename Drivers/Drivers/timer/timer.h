#pragma once

#include <stdint.h>
#include "stm32g070xx.h"

typedef enum {
    TIM_OK = 0,
    TIM_ERR = 1,
	TIM_ERR_INVALID = 2,
	TIM_ERR_TIMEOUT = 3
} TIM_Status_t;

typedef enum {
    TIM_INTERRUPTS_DISABLED = 0,
    TIM_INTERRUPTS_ENABLED = 1
} TIM_Config_Interrupts_t;

typedef struct {
    TIM_TypeDef *TIMx;
    uint32_t psc;
    uint32_t arr;
    uint16_t cnt;
    TIM_Config_Interrupts_t interrupt_status;
} TIM_Config_t;

TIM_Status_t timer_init(TIM_Config_t *cfg);

TIM_Status_t timer_delay_s(TIM_TypeDef *TIMx, uint32_t s);

TIM_Status_t timer_get_update_flag(TIM_TypeDef *TIMx);

TIM_Status_t timer_clear_update_flag(TIM_TypeDef *TIMx);
