#pragma once

#include <stdbool.h>
#include "timer/timer.h"

typedef enum {
    TIM_IC_INPUT_OUTPUT = 0,
    TIM_IC_INPUT_DIRECT = 1,
    TIM_IC_INPUT_INDIRECT = 2,
    TIM_IC_INPUT_TRC = 3
} TIM_IC_Input_Selection_t;

typedef enum {
    TIM_IC_PSC_NONE = 0,
    TIM_IC_PSC_TWO = 1,
    TIM_IC_PSC_FOUR = 2,
    TIM_IC_PSC_EIGHT = 3
} TIM_IC_Prescaler_t;

typedef enum {
    TIM_IC_FILTER_NONE = 0,

    TIM_IC_FILTER_FCK_INT_N2 = 1,
    TIM_IC_FILTER_FCK_INT_N4 = 2,
    TIM_IC_FILTER_FCK_INT_N8 = 3,

    TIM_IC_FILTER_FDTS_DIV2_N6 = 4,
    TIM_IC_FILTER_FDTS_DIV2_N8 = 5,
    
    TIM_IC_FILTER_FDTS_DIV4_N6 = 6,
    TIM_IC_FILTER_FDTS_DIV4_N8 = 7,

    TIM_IC_FILTER_FDTS_DIV8_N6 = 8,
    TIM_IC_FILTER_FDTS_DIV8_N8 = 9,

    TIM_IC_FILTER_FDTS_DIV16_N5 = 10,
    TIM_IC_FILTER_FDTS_DIV16_N6 = 11,
    TIM_IC_FILTER_FDTS_DIV16_N8 = 12,

    TIM_IC_FILTER_FDTS_DIV32_N5 = 13,
    TIM_IC_FILTER_FDTS_DIV32_N6 = 14,
    TIM_IC_FILTER_FDTS_DIV32_N8 = 15
} TIM_IC_Filter_t;

typedef enum {
    TIM_IC_EDGE_RISING = 0,
    TIM_IC_EDGE_FALLING = 1,
    TIM_IC_EDGE_BOTH = 2
} TIM_IC_Edge_t;

typedef struct {
    TIM_TypeDef *TIMx;
    uint8_t channel;
    TIM_IC_Input_Selection_t input_selection;
    TIM_IC_Prescaler_t psc;
    TIM_IC_Filter_t filter;
    TIM_IC_Edge_t mode;
} TIM_Input_Capture_Config_t;

TIM_Status_t timer_input_capture_config_polarity(TIM_Input_Capture_Config_t *cfg);

TIM_Status_t timer_input_capture_init(TIM_Input_Capture_Config_t *cfg);

TIM_Status_t timer_input_capture_read(TIM_Input_Capture_Config_t *cfg, uint32_t *capture_value);

bool timer_input_capture_flag_is_set(TIM_Input_Capture_Config_t *cfg);

TIM_Status_t timer_input_capture_clear_flag(TIM_Input_Capture_Config_t *cfg);
