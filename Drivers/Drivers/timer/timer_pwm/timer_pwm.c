#include "timer_pwm.h"

#define MSK_CH_1 ((7U << 4) | (1U << 16))
#define MSK_CH_2 ((7U << 12) | (1U << 24))
#define MSK_CH_3 MSK_CH_1
#define MSK_CH_4 MSK_CH_2

#define PRELOAD_CH_1 (1U << 3)
#define PRELOAD_CH_2 (1U << 11)
#define PRELOAD_CH_3 PRELOAD_CH_1
#define PRELOAD_CH_4 PRELOAD_CH_2

#define CC_EN_CH_1 (1U << 0)
#define CC_EN_CH_2 (1U << 4)
#define CC_EN_CH_3 (1U << 8)
#define CC_EN_CH_4 (1U << 12)

static TIM_Status_t timer_pwm_configure_duty_cycle(TIM_PWM_Config_t *cfg)
{
	if (cfg->duty_cycle > 100) cfg->duty_cycle = 100;

	uint32_t duty_value = ((cfg->TIMx->ARR + 1) * cfg->duty_cycle) / 100;

	switch (cfg->channel)
	{
		case 1:
			cfg->TIMx->CCR1 = duty_value;
			break;
		case 2:
			cfg->TIMx->CCR2 = duty_value;
			break;
		case 3:
			cfg->TIMx->CCR3 = duty_value;
			break;
		case 4:
			cfg->TIMx->CCR4 = duty_value;
			break;
		default:
			return TIM_ERR;
			break;
	}

	return TIM_OK;
}

static TIM_Status_t timer_pwm_get_mode_value(TIM_PWM_Mode_t mode, uint32_t *value)
{
    switch (mode)
    {
        case TIM_PWM1:
            *value = 6U;
            return TIM_OK;

        case TIM_PWM2:
            *value = 7U;
            return TIM_OK;

        default:
            return TIM_ERR;
    }
}

static TIM_Status_t timer_pwm_config_channel(TIM_PWM_Config_t *cfg)
{
    uint32_t mode;

    if (timer_pwm_get_mode_value(cfg->pwm_mode, &mode) != TIM_OK)
    {
        return TIM_ERR;
    }

    switch (cfg->channel)
    {
        case 1:
            cfg->TIMx->CCMR1 &= ~MSK_CH_1;
            cfg->TIMx->CCMR1 |=  (mode << 4);
            cfg->TIMx->CCMR1 |=  PRELOAD_CH_1;
            cfg->TIMx->CCER  |=  CC_EN_CH_1;
            break;

        case 2:
            cfg->TIMx->CCMR1 &= ~MSK_CH_2;
            cfg->TIMx->CCMR1 |=  (mode << 12);
            cfg->TIMx->CCMR1 |=  PRELOAD_CH_2;
            cfg->TIMx->CCER  |=  CC_EN_CH_2;
            break;

        case 3:
            cfg->TIMx->CCMR2 &= ~MSK_CH_3;
            cfg->TIMx->CCMR2 |=  (mode << 4);
            cfg->TIMx->CCMR2 |=  PRELOAD_CH_3;
            cfg->TIMx->CCER  |=  CC_EN_CH_3;
            break;

        case 4:
            cfg->TIMx->CCMR2 &= ~MSK_CH_4;
            cfg->TIMx->CCMR2 |=  (mode << 12);
            cfg->TIMx->CCMR2 |=  PRELOAD_CH_4;
            cfg->TIMx->CCER  |=  CC_EN_CH_4;
            break;

        default:
            return TIM_ERR;
    }

    return TIM_OK;
}

static TIM_Status_t timer_pwm_enable_main_output_if_needed(TIM_TypeDef *TIMx)
{
    if (TIMx == TIM1 || TIMx == TIM15 || TIMx == TIM16 || TIMx == TIM17)
    {
        TIMx->BDTR |= TIM_BDTR_MOE;
    }

	return TIM_OK;
}

TIM_Status_t timer_pwm_init(TIM_PWM_Config_t *cfg)
{
	TIM_Status_t status = TIM_OK;
	cfg->TIMx->CR1 &= ~TIM_CR1_CEN;

	status = timer_pwm_config_channel(cfg);
	if (status != TIM_OK) return status;

	status = timer_pwm_configure_duty_cycle(cfg);
	if (status != TIM_OK) return status;
	
	timer_pwm_enable_main_output_if_needed(cfg->TIMx);

	cfg->TIMx->CR1 |= TIM_CR1_CEN;

	return status;
}

TIM_Status_t timer_pwm_set_duty_cycle(TIM_PWM_Config_t *cfg, uint32_t percentage)
{
	TIM_Status_t status = TIM_OK;

	cfg->duty_cycle = percentage;

	status = timer_pwm_configure_duty_cycle(cfg);
	if (status != TIM_OK) return status;

	return status;
}

TIM_Status_t timer_pwm_set_compare_value(TIM_PWM_Config_t *cfg, uint32_t compare_value)
{
	TIM_Status_t status = TIM_OK;
	if (compare_value > (cfg->TIMx->ARR + 1)) compare_value = cfg->TIMx->ARR + 1;

	switch (cfg->channel)
	{
		case 1: cfg->TIMx->CCR1 = compare_value;
			break;
		case 2: cfg->TIMx->CCR2 = compare_value;
			break;
		case 3: cfg->TIMx->CCR3 = compare_value;
			break;
		case 4: cfg->TIMx->CCR4 = compare_value;
			break;
		default:
			status =  TIM_ERR;
			break;
	}

	return status;
}
