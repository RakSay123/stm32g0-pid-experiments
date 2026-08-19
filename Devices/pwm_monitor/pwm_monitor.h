#pragma once

#include "stm32g070xx.h"
#include "gpio/gpio.h"
#include "uart/uart.h"
#include "timer/timer.h"
#include "timer/timer_input_capture/timer_input_capture.h"

typedef enum {
    PWM_MONITOR_WAIT_RISING,
    PWM_MONITOR_WAIT_FALLING,
    PWM_MONITOR_WAIT_NEXT_RISING
} PWM_Monitor_State_t;

typedef struct {
    GPIO_TypeDef *port;
	uint8_t pin;
	GPIO_AF_t alternate;

	TIM_Input_Capture_Config_t *capture_cfg;
	USART_TypeDef *USARTx;

	PWM_Monitor_State_t state;

	uint32_t rising_timestamp;
	uint32_t falling_timestamp;
	uint32_t next_rising_timestamp;

	uint32_t period_ticks;
	uint32_t high_ticks;
	uint32_t low_ticks;
	uint32_t duty_percent;
} PWM_Monitor_t;

void pwm_monitor_init(PWM_Monitor_t *monitor);

void pwm_monitor_update(PWM_Monitor_t *monitor);

void pwm_monitor_print(PWM_Monitor_t *monitor);
