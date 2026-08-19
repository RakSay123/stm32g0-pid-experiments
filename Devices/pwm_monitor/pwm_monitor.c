#include "pwm_monitor.h"

void pwm_monitor_init(PWM_Monitor_t *monitor)
{
    GPIO_Config_t gpio_cfg = {
        .port = monitor->port,
        .pin = monitor->pin,
        .mode = GPIO_MODE_AF,
        .otype = GPIO_PUSH_PULL,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_NO_PULL,
        .alternate = monitor->alternate
    };

    gpio_init(&gpio_cfg);

    monitor->capture_cfg->mode = TIM_IC_EDGE_RISING;
    timer_input_capture_init(monitor->capture_cfg);

    monitor->state = PWM_MONITOR_WAIT_RISING;
}

static uint32_t pwm_monitor_elapsed_ticks(uint32_t start, uint32_t end, uint32_t arr)
{
    if (end >= start) return end - start;

    return (arr + 1U) - start + end;
}

static void pwm_monitor_switch_polarity(PWM_Monitor_t *monitor, TIM_IC_Edge_t edge)
{
	monitor->capture_cfg->mode = edge;
	timer_input_capture_config_polarity(monitor->capture_cfg);
}

void pwm_monitor_update(PWM_Monitor_t *monitor)
{
	if (!timer_input_capture_flag_is_set(monitor->capture_cfg)) return;

	uint32_t capture_value;
	if (timer_input_capture_read(monitor->capture_cfg, &capture_value) != TIM_OK) return;

	timer_input_capture_clear_flag(monitor->capture_cfg);

    switch (monitor->state)
    {
        case PWM_MONITOR_WAIT_RISING:
            monitor->rising_timestamp = capture_value;
            pwm_monitor_switch_polarity(monitor, TIM_IC_EDGE_FALLING);
            monitor->state = PWM_MONITOR_WAIT_FALLING;
            break;

         case PWM_MONITOR_WAIT_FALLING:
             monitor->falling_timestamp = capture_value;

             monitor->high_ticks = pwm_monitor_elapsed_ticks(monitor->rising_timestamp, monitor->falling_timestamp, monitor->capture_cfg->TIMx->ARR);

             pwm_monitor_switch_polarity(monitor, TIM_IC_EDGE_RISING);
             monitor->state = PWM_MONITOR_WAIT_NEXT_RISING;
             break;

        case PWM_MONITOR_WAIT_NEXT_RISING:
            monitor->next_rising_timestamp = capture_value;

            monitor->period_ticks = pwm_monitor_elapsed_ticks(monitor->rising_timestamp, monitor->next_rising_timestamp, monitor->capture_cfg->TIMx->ARR);
            monitor->low_ticks = pwm_monitor_elapsed_ticks(monitor->falling_timestamp, monitor->next_rising_timestamp, monitor->capture_cfg->TIMx->ARR);
            
            if (monitor->period_ticks != 0U) monitor->duty_percent = (monitor->high_ticks * 100U) / monitor->period_ticks;
            
            monitor->rising_timestamp = monitor->next_rising_timestamp;
            
            pwm_monitor_switch_polarity(monitor, TIM_IC_EDGE_FALLING);
            monitor->state = PWM_MONITOR_WAIT_FALLING;
            break;

        default:
            return;
    }
}

void pwm_monitor_print(PWM_Monitor_t *monitor)
{
    uart_write_str(monitor->USARTx, "[PWM_MONITOR] Period: ");
    uart_write_int(monitor->USARTx, monitor->period_ticks);
    uart_write_line(monitor->USARTx, " Ticks");

    uart_write_str(monitor->USARTx, "[PWM_MONITOR] High: ");
	uart_write_int(monitor->USARTx, monitor->high_ticks);
	uart_write_line(monitor->USARTx, " Ticks");

	uart_write_str(monitor->USARTx, "[PWM_MONITOR] Low: ");
	uart_write_int(monitor->USARTx, monitor->low_ticks);
	uart_write_line(monitor->USARTx, " Ticks");

	uart_write_str(monitor->USARTx, "[PWM_MONITOR] Duty: ");
	uart_write_int(monitor->USARTx, monitor->duty_percent);
	uart_write_line(monitor->USARTx, "%");

	uart_write_line(monitor->USARTx, "");
}
