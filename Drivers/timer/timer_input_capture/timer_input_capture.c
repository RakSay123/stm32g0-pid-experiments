#include "timer/timer_input_capture/timer_input_capture.h"

TIM_Status_t timer_input_capture_config_polarity(TIM_Input_Capture_Config_t *cfg)
{
    uint32_t base = 4U * (cfg->channel - 1U);

    uint32_t ccxp  = (1U << (base + 1U));
    uint32_t ccxnp = (1U << (base + 3U));

    cfg->TIMx->CCER &= ~(ccxp | ccxnp);

    switch (cfg->mode)
    {
        case TIM_IC_EDGE_RISING:
            break;

        case TIM_IC_EDGE_FALLING:
            cfg->TIMx->CCER |= ccxp;
            break;

        case TIM_IC_EDGE_BOTH:
            cfg->TIMx->CCER |= ccxp | ccxnp;
            break;

        default:
            return TIM_ERR;
    }

    return TIM_OK;
}

TIM_Status_t timer_input_capture_init(TIM_Input_Capture_Config_t *cfg)
{
    // if (cfg == NULL || cfg->TIMx == NULL) return TIM_ERR;
    if (cfg->channel < 1U || cfg->channel > 4U) return TIM_ERR;
    if (cfg->input_selection > TIM_IC_INPUT_TRC) return TIM_ERR;
    if (cfg->psc > TIM_IC_PSC_EIGHT) return TIM_ERR;
    if (cfg->filter > TIM_IC_FILTER_FDTS_DIV32_N8) return TIM_ERR;
    if (cfg->mode > TIM_IC_EDGE_BOTH) return TIM_ERR;
    
    cfg->TIMx->CR1 &= ~TIM_CR1_CEN;

    volatile uint32_t *ccmr;
    if (cfg->channel <= 2U) ccmr = &cfg->TIMx->CCMR1;
    else ccmr = &cfg->TIMx->CCMR2;

    uint32_t field_shift = ((cfg->channel == 1U) || (cfg->channel == 3U)) ? 0U : 8U;
    uint32_t ccer_base = 4U * (cfg->channel - 1U);
    uint32_t channel_enable = (1U << ccer_base);
    uint32_t capture_flag = (1U << cfg->channel);

    cfg->TIMx->CCER &= ~channel_enable;

    *ccmr &= ~((3U << field_shift) | (3U << (field_shift + 2)) | (15U << (field_shift + 4U)));
    *ccmr |= (((uint32_t)cfg->input_selection << field_shift) | ((uint32_t)cfg->psc << (field_shift + 2U)) | ((uint32_t)cfg->filter << (field_shift + 4U)));

    TIM_Status_t status = timer_input_capture_config_polarity(cfg);
    if (status != TIM_OK) return status;

    cfg->TIMx->SR &= ~capture_flag;

    cfg->TIMx->CCER |= channel_enable;
    cfg->TIMx->CR1 |= TIM_CR1_CEN;

    return TIM_OK;
}

TIM_Status_t timer_input_capture_read(TIM_Input_Capture_Config_t *cfg, uint32_t *capture_value)
{
    switch (cfg->channel)
    {
        case 1:
            *capture_value = cfg->TIMx->CCR1;
            break;
        case 2:
            *capture_value = cfg->TIMx->CCR2;
            break;
        case 3:
            *capture_value = cfg->TIMx->CCR3;
            break;
        case 4:
            *capture_value = cfg->TIMx->CCR4;
            break;
        default:
            return TIM_ERR;
    }

    return TIM_OK;
}

bool timer_input_capture_flag_is_set(TIM_Input_Capture_Config_t *cfg)
{
    if (cfg->channel < 1U || cfg->channel > 4U) return false;

    uint32_t capture_flag = (1U << cfg->channel);
    return (cfg->TIMx->SR & capture_flag) != 0U;
}

TIM_Status_t timer_input_capture_clear_flag(TIM_Input_Capture_Config_t *cfg)
{
    if (cfg->channel < 1U || cfg->channel > 4U) return TIM_ERR;

    uint32_t capture_flag = (1U << cfg->channel);
    cfg->TIMx->SR &= ~capture_flag;

    return TIM_OK;
}
