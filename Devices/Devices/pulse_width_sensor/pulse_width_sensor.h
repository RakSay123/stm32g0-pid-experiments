#pragma once

#include <stdbool.h>
#include "timer/timer_input_capture/timer_input_capture.h"
#include "gpio/gpio.h"

typedef enum
{
    PULSE_WIDTH_SENSOR_OK,
    PULSE_WIDTH_SENSOR_NO_NEW_EDGE,
    PULSE_WIDTH_SENSOR_MEASUREMENT_READY,
    PULSE_WIDTH_SENSOR_INVALID_PULSE,
    PULSE_WIDTH_SENSOR_NO_DETECTION,
    PULSE_WIDTH_SENSOR_ERR
} PULSE_WIDTH_SENSOR_Status_t;

typedef enum {
	WAITING_FOR_RISING,
	WAITING_FOR_FALLING
} PULSE_WIDTH_SENSOR_State_t;

typedef struct {
	GPIO_TypeDef *port;
	uint8_t pin;
	GPIO_AF_t alternate;

	TIM_Input_Capture_Config_t *capture_cfg;

	PULSE_WIDTH_SENSOR_State_t state;

	bool measurement_ready;

	uint32_t rising_timestamp;
	uint32_t falling_timestamp;

	uint32_t high_ticks;

	uint32_t minimum_distance_mm;
	uint32_t maximum_distance_mm;

	uint32_t minimum_pulse_width_us;
	uint32_t maximum_pulse_width_us;
	uint32_t no_detection_pulse_width_us;

	uint32_t distance_mm;


} PULSE_WIDTH_SENSOR_t;

PULSE_WIDTH_SENSOR_Status_t pulse_width_sensor_init(PULSE_WIDTH_SENSOR_t *cfg);

PULSE_WIDTH_SENSOR_Status_t pulse_width_sensor_compute_distance_mm(PULSE_WIDTH_SENSOR_t *sensor, uint32_t *distance_mm);

