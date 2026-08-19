#include "dc_motor.h"

DC_MOTOR_Status_t dc_motor_init(DC_MOTOR_t *motor)
{
	if (motor == NULL || motor->motor_driver == NULL) return DC_MOTOR_ERR;

	if (tb6612fng_init(motor->motor_driver) != TB6612FNG_OK) return DC_MOTOR_ERR;

	return DC_MOTOR_OK;
}

DC_MOTOR_Status_t dc_motor_set_speed(DC_MOTOR_t *motor, uint16_t speed)
{
	if (tb6612fng_set_duty_cycle(motor->motor_driver, motor->driver_channel, speed) != TB6612FNG_OK) return DC_MOTOR_ERR;

	return DC_MOTOR_OK;
}

DC_MOTOR_Status_t dc_motor_set_direction(DC_MOTOR_t *motor, TB6612FNG_Direction_t direction)
{
	if (tb6612fng_set_direction(motor->motor_driver, motor->driver_channel, direction) != TB6612FNG_OK) return DC_MOTOR_ERR;

	return DC_MOTOR_OK;
}

DC_MOTOR_Status_t dc_motor_set_speed_and_direction(DC_MOTOR_t *motor, TB6612FNG_Direction_t direction, uint16_t speed)
{
	if (dc_motor_set_direction(motor, direction) != DC_MOTOR_OK) return DC_MOTOR_ERR;
	if (dc_motor_set_speed(motor, speed) != DC_MOTOR_OK) return DC_MOTOR_ERR;

	return DC_MOTOR_OK;
}

DC_MOTOR_Status_t dc_motor_brake(DC_MOTOR_t *motor)
{
	if (tb6612fng_brake(motor->motor_driver, motor->driver_channel) != TB6612FNG_OK) return DC_MOTOR_ERR;

	return DC_MOTOR_OK;
}

DC_MOTOR_Status_t dc_motor_coast(DC_MOTOR_t *motor)
{
	if (tb6612fng_coast(motor->motor_driver, motor->driver_channel) != TB6612FNG_OK) return DC_MOTOR_ERR;

	return DC_MOTOR_OK;
}
