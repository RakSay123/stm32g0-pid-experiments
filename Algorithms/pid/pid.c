#include "pid.h"

PID_Status_t pid_init(PID_t *pid, float kp, float ki, float kd, float output_min, float output_max)
{
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	pid->output_min = output_min;
	pid->output_max = output_max;

	return PID_OK;
}

PID_Status_t pid_reset(PID_t *pid)
{
	return PID_OK;
}

PID_Status_t pid_update(PID_t *pid, float *output, float setpoint, float measurement)
{
	float error = setpoint - measurement;
	*output = pid->kp * error;

//	if (*output > pid->output_max) *output = pid->output_max;
//	else if (*output < pid->output_min) *output = pid->output_min;

	return PID_OK;
}
