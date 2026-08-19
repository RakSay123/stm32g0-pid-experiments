#include "timer.h"

static TIM_Status_t timer_enable_clock(TIM_TypeDef *TIMx)
{
    if (TIMx == TIM1) RCC->APBENR2 |= (1U << 11);
    else if (TIMx == TIM3) RCC->APBENR1 |= (1U << 1);
#ifdef TIM4
    else if (TIMx == TIM4) RCC->APBENR1 |= (1U << 2);
#endif
#ifdef TIM6
    else if (TIMx == TIM6) RCC->APBENR1 |= (1U << 4);
#endif
#ifdef TIM7
    else if (TIMx == TIM7) RCC->APBENR1 |= (1U << 5);
#endif
    else if (TIMx == TIM14) RCC->APBENR2 |= (1U << 15);
#ifdef TIM15
    else if (TIMx == TIM15) RCC->APBENR2 |= (1U << 16);
#endif
    else if (TIMx == TIM16) RCC->APBENR2 |= (1U << 17);
    else if (TIMx == TIM17) RCC->APBENR2 |= (1U << 18);
    else return TIM_ERR;

    return TIM_OK;
}

static IRQn_Type timer_get_irq(TIM_TypeDef *TIMx)
{
    if (TIMx == TIM1) return TIM1_BRK_UP_TRG_COM_IRQn;
    if (TIMx == TIM3) return TIM3_IRQn;
#ifdef TIM4
    if (TIMx == TIM4) return TIM4_IRQn;
#endif
#ifdef TIM6
    if (TIMx == TIM6) return TIM6_IRQn;
#endif
#ifdef TIM7
    if (TIMx == TIM7) return TIM7_IRQn;
#endif
    if (TIMx == TIM14) return TIM14_IRQn;
#ifdef TIM15
    if (TIMx == TIM15) return TIM15_IRQn;
#endif
    if (TIMx == TIM16) return TIM16_IRQn;
    if (TIMx == TIM17) return TIM17_IRQn;

    return (IRQn_Type)-1;
}

static TIM_Status_t timer_NVIC_interrupt_enable(TIM_TypeDef *TIMx)
{
    IRQn_Type irq = timer_get_irq(TIMx);
    
    if (irq == (IRQn_Type)-1) return TIM_ERR;

    NVIC_EnableIRQ(irq);

    return TIM_OK;
}

TIM_Status_t timer_init(TIM_Config_t *cfg)
{
    TIM_Status_t status = TIM_OK;

    status = timer_enable_clock(cfg->TIMx);
    if (status != TIM_OK) return status;

    cfg->TIMx->CR1 &= ~TIM_CR1_CEN;
    
    cfg->TIMx->PSC = cfg->psc;
    cfg->TIMx->ARR = cfg->arr;

    cfg->TIMx->EGR |= TIM_EGR_UG;
    cfg->TIMx->SR &= ~TIM_SR_UIF;
    cfg->TIMx->CNT = cfg->cnt;

    if (cfg->interrupt_status == TIM_INTERRUPTS_ENABLED) 
    {    
        cfg->TIMx->DIER |= TIM_DIER_UIE;
        timer_NVIC_interrupt_enable(cfg->TIMx);
    }
    else
    {   
        cfg->TIMx->DIER &= ~TIM_DIER_UIE;
    }

    cfg->TIMx->CR1 |= TIM_CR1_CEN;

    return status;
}

static TIM_Status_t timer_wait(TIM_TypeDef *TIMx)
{
    while (!(TIMx->SR & TIM_SR_UIF));
    TIMx->SR &= ~TIM_SR_UIF;

    return TIM_OK;
}

TIM_Status_t timer_delay_s(TIM_TypeDef *TIMx, uint32_t s)
{
    TIM_Status_t status = TIM_OK;
    for (int i = 0; i < s; i++)
    {
        status = timer_wait(TIMx);
        if (status != TIM_OK) return status;
    }

    return status;
}

TIM_Status_t timer_get_update_flag(TIM_TypeDef *TIMx)
{
    if (TIMx->SR & TIM_SR_UIF) return TIM_OK;
    else return TIM_ERR;
}

TIM_Status_t timer_clear_update_flag(TIM_TypeDef *TIMx)
{
    TIMx->SR &= ~TIM_SR_UIF;
    return TIM_OK;   
}
