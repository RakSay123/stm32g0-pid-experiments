#include <stddef.h>

#include "stm32g070xx.h"
#include "timer/timer_encoder/timer_encoder.h"

static TIM_Status_t timer_encoder_gpio_init(TIM_ENCODER_Config_t *cfg)
{
	GPIO_Config_t ch_a = {
		.port = cfg->channel_a_port,
		.pin = cfg->channel_a_pin,
		.mode = GPIO_MODE_AF,
		.otype = GPIO_PUSH_PULL,
		.speed = GPIO_SPEED_HIGH,
		.pull = GPIO_PULL_UP,
		.alternate = cfg->channel_a_AF
	};

	GPIO_Config_t ch_b = {
		.port = cfg->channel_b_port,
		.pin = cfg->channel_b_pin,
		.mode = GPIO_MODE_AF,
		.otype = GPIO_PUSH_PULL,
		.speed = GPIO_SPEED_HIGH,
		.pull = GPIO_PULL_UP,
		.alternate = cfg->channel_b_AF
	};

	gpio_init(&ch_a);
	gpio_init(&ch_b);

	return TIM_OK;
}

static TIM_Status_t timer_encoder_timer_disable(TIM_ENCODER_Config_t *cfg)
{
	cfg->TIMx->CR1 &= ~(1U << 0);
	return TIM_OK;
}

static TIM_Status_t timer_encoder_clear_flags(TIM_ENCODER_Config_t *cfg)
{
	cfg->TIMx->SR = 0U;
	return TIM_OK;
}

static TIM_Status_t timer_encoder_channel_disable(TIM_ENCODER_Config_t *cfg)
{
	cfg->TIMx->CCER &= ~((1U << 0) | (1U << 4));
	return TIM_OK;
}

static TIM_Status_t timer_encoder_config_input_init(TIM_ENCODER_Config_t *cfg)
{
	cfg->TIMx->CCMR1 &= ~((3U << 0) | (3U << 8));
	cfg->TIMx->CCMR1 |= ((1U << 0) | (1U << 8));
	return TIM_OK;
}

static TIM_Status_t timer_encoder_filter_init(TIM_ENCODER_Config_t *cfg)
{
	if ((uint32_t)cfg->channel_a_filter > 15U || (uint32_t)cfg->channel_b_filter > 15U) return TIM_ERR;

	cfg->TIMx->CCMR1 &= ~((15U << 4) | (15U << 12));
	cfg->TIMx->CCMR1 |= ((uint32_t)cfg->channel_a_filter << 4) | ((uint32_t)cfg->channel_b_filter << 12);

	return TIM_OK;
}

static TIM_Status_t timer_encoder_prescaler_init(TIM_ENCODER_Config_t *cfg)
{
	if ((uint32_t)cfg->channel_a_psc > TIM_ENCODER_IC_PSC_DIV8 || (uint32_t)cfg->channel_b_psc > TIM_ENCODER_IC_PSC_DIV8) return TIM_ERR;

	cfg->TIMx->CCMR1 &= ~((3U << 2) | (3U << 10));
	cfg->TIMx->CCMR1 |= ((uint32_t)cfg->channel_a_psc << 2) | ((uint32_t)cfg->channel_b_psc << 10);

	return TIM_OK;
}

static TIM_Status_t timer_encoder_polarity_config(TIM_ENCODER_Config_t *cfg)
{
	if ((uint32_t)cfg->channel_a_polarity > TIM_ENCODER_POLARITY_INVERTED || (uint32_t)cfg->channel_b_polarity > TIM_ENCODER_POLARITY_INVERTED) return TIM_ERR;

	cfg->TIMx->CCER &= ~((1U << 1) | (1U << 3) | (1U << 5) | (1U << 7));

	if (cfg->channel_a_polarity == TIM_ENCODER_POLARITY_INVERTED) cfg->TIMx->CCER |= (1U << 1);

	if (cfg->channel_b_polarity == TIM_ENCODER_POLARITY_INVERTED) cfg->TIMx->CCER |= (1U << 5);

	return TIM_OK;
}

static TIM_Status_t timer_encoder_channel_enable(TIM_ENCODER_Config_t *cfg)
{
	cfg->TIMx->CCER |= (1U << 0) | (1U << 4);
	return TIM_OK;
}

static TIM_Status_t timer_encoder_mode_init(TIM_ENCODER_Config_t *cfg)
{
	cfg->TIMx->SMCR &= ~((7U << 0) | (1U << 16));

	switch (cfg->mode)
	{
		case TIM_ENCODER_MODE_TI2_EDGES:
			cfg->TIMx->SMCR |= (1U << 0);
			break;

		case TIM_ENCODER_MODE_TI1_EDGES:
			cfg->TIMx->SMCR |= (2U << 0);
			break;

		case TIM_ENCODER_MODE_TI1_TI2_EDGES:
			cfg->TIMx->SMCR |= (3U << 0);
			break;

		default:
			return TIM_ERR;
	}

	return TIM_OK;
}

static TIM_Status_t timer_encoder_timer_enable(TIM_ENCODER_Config_t *cfg)
{
	cfg->TIMx->CR1 |= (1U << 0);
	return TIM_OK;
}

TIM_Status_t timer_encoder_init(TIM_ENCODER_Config_t *cfg)
{
	if ((cfg == NULL) || (cfg->TIMx == NULL)) return TIM_ERR;

	TIM_Status_t status = TIM_OK;

	status = timer_encoder_gpio_init(cfg);
	if (status != TIM_OK) return status;

	status = timer_encoder_timer_disable(cfg);
	if (status != TIM_OK) return status;

	status = timer_encoder_clear_flags(cfg);
	if (status != TIM_OK) return status;

	status = timer_encoder_channel_disable(cfg);
	if (status != TIM_OK) return status;

	status = timer_encoder_config_input_init(cfg);
	if (status != TIM_OK) return status;

	status = timer_encoder_filter_init(cfg);
	if (status != TIM_OK) return status;

	status = timer_encoder_prescaler_init(cfg);
	if (status != TIM_OK) return status;

	status = timer_encoder_polarity_config(cfg);
	if (status != TIM_OK) return status;

	status = timer_encoder_mode_init(cfg);
	if (status != TIM_OK) return status;

	status = timer_encoder_channel_enable(cfg);
	if (status != TIM_OK) return status;

	status = timer_encoder_timer_enable(cfg);
	if (status != TIM_OK) return status;

	return status;
}

TIM_Status_t timer_encoder_get_count(TIM_ENCODER_Config_t *encoder, uint32_t *count)
{
	if (encoder == NULL || encoder->TIMx == NULL || count == NULL) return TIM_ERR;

	*count = encoder->TIMx->CNT;

	return TIM_OK;
}

TIM_Status_t timer_encoder_set_count(TIM_ENCODER_Config_t *encoder, uint32_t count)
{
	if (encoder == NULL || encoder->TIMx == NULL) return TIM_ERR;

	if (count > encoder->TIMx->ARR) return TIM_ERR;

	encoder->TIMx->CNT = count;

	return TIM_OK;
}

TIM_Status_t timer_encoder_reset_count(TIM_ENCODER_Config_t *encoder)
{
	return timer_encoder_set_count(encoder, 0U);
}

TIM_Status_t timer_encoder_get_direction(TIM_ENCODER_Config_t *encoder, TIM_ENCODER_Direction_t *direction)
{
	if (encoder == NULL || encoder->TIMx == NULL || direction == NULL) return TIM_ERR;

	if ((encoder->TIMx->CR1 & (1U << 4)) != 0U) *direction = TIM_ENCODER_DIRECTION_DOWN;
	else *direction = TIM_ENCODER_DIRECTION_UP;

	return TIM_OK;
}

