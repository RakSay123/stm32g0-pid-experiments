#include "pulse_width_sensor.h"

PULSE_WIDTH_SENSOR_Status_t pulse_width_sensor_init(PULSE_WIDTH_SENSOR_t *cfg)
{
	GPIO_Config_t gpio_cfg = {
		.port = cfg->port,
		.pin = cfg->pin,
		.mode = GPIO_MODE_AF,
		.otype = GPIO_PUSH_PULL,
		.speed = GPIO_SPEED_HIGH,
		.pull = GPIO_NO_PULL,
		.alternate = cfg->alternate
	};

	gpio_init(&gpio_cfg); // In the future implement error checking for the GPIO

	cfg->capture_cfg->mode = TIM_IC_EDGE_RISING;
	if (timer_input_capture_init(cfg->capture_cfg) == TIM_ERR) return PULSE_WIDTH_SENSOR_ERR;

	cfg->state = WAITING_FOR_RISING;

	return PULSE_WIDTH_SENSOR_OK;
}

static uint32_t elapsed_ticks(uint32_t start, uint32_t end, uint32_t arr)
{
	if (end >= start) return end - start;

	return (arr + 1U) - start + end;
}

static PULSE_WIDTH_SENSOR_Status_t pulse_width_sensor_switch_polarity(PULSE_WIDTH_SENSOR_t *sensor, TIM_IC_Edge_t edge)
{
	sensor->capture_cfg->mode = edge;
	timer_input_capture_config_polarity(sensor->capture_cfg);

	return PULSE_WIDTH_SENSOR_OK;
}

PULSE_WIDTH_SENSOR_Status_t pulse_width_sensor_update(PULSE_WIDTH_SENSOR_t *sensor)
{
	if (!timer_input_capture_flag_is_set(sensor->capture_cfg)) return PULSE_WIDTH_SENSOR_NO_NEW_EDGE;

	TIM_Status_t status;
	uint32_t capture_value;

	status = timer_input_capture_read(sensor->capture_cfg, &capture_value);
	if (status != TIM_OK) return PULSE_WIDTH_SENSOR_ERR;

	status = timer_input_capture_clear_flag(sensor->capture_cfg);
	if (status != TIM_OK) return PULSE_WIDTH_SENSOR_ERR;

	switch (sensor->state)
	{
		case WAITING_FOR_RISING:
			sensor->rising_timestamp = capture_value;
			if (pulse_width_sensor_switch_polarity(sensor, TIM_IC_EDGE_FALLING) != PULSE_WIDTH_SENSOR_OK) return PULSE_WIDTH_SENSOR_ERR;
			sensor->state = WAITING_FOR_FALLING;
			return PULSE_WIDTH_SENSOR_OK;

		case WAITING_FOR_FALLING:
			sensor->falling_timestamp = capture_value;

			sensor->high_ticks = elapsed_ticks(sensor->rising_timestamp, sensor->falling_timestamp, sensor->capture_cfg->TIMx->ARR);
			sensor->measurement_ready = true;

			if (pulse_width_sensor_switch_polarity(sensor, TIM_IC_EDGE_RISING) != PULSE_WIDTH_SENSOR_OK) return PULSE_WIDTH_SENSOR_ERR;
			sensor->state = WAITING_FOR_RISING;
			return PULSE_WIDTH_SENSOR_MEASUREMENT_READY;

		default:
			return PULSE_WIDTH_SENSOR_ERR;
	}

	return PULSE_WIDTH_SENSOR_OK;
}

PULSE_WIDTH_SENSOR_Status_t pulse_width_sensor_compute_distance_mm(PULSE_WIDTH_SENSOR_t *sensor, uint32_t *distance_mm)
{
	if (!sensor->measurement_ready) return PULSE_WIDTH_SENSOR_NO_NEW_EDGE;

	sensor->measurement_ready = false;

	int32_t pulse_width_us = sensor->high_ticks;

	if (pulse_width_us >= sensor->no_detection_pulse_width_us)
	{
		sensor->distance_mm = 0U;
		*distance_mm = 0U;

		return PULSE_WIDTH_SENSOR_NO_DETECTION;
	}

	if (pulse_width_us < sensor->minimum_pulse_width_us || pulse_width_us > sensor->maximum_pulse_width_us) return PULSE_WIDTH_SENSOR_INVALID_PULSE;

	uint32_t calculated_distance_mm = ((pulse_width_us - 1000U) * 3U) / 4U;

	if ((calculated_distance_mm < sensor->minimum_distance_mm) || (calculated_distance_mm > sensor->maximum_distance_mm)) return PULSE_WIDTH_SENSOR_INVALID_PULSE;

	sensor->distance_mm = calculated_distance_mm;
	*distance_mm = calculated_distance_mm;

	return PULSE_WIDTH_SENSOR_OK;
}
