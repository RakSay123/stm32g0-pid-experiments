#include "pid.h"

PID_Status_t pid_init(PID_t *pid)
{
	return PID_OK;
}

PID_Status_t pid_reset(PID_t *pid)
{
	return PID_OK;
}

PID_Status_t pid_update(PID_t *pid, float *output, float setpoint, float measurement, float dt)
{
	return PID_OK;
}
