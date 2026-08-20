#pragma once

#include "motor_driver/motor_driver.h"
#include "tb6612fng.h"

MOTOR_DRIVER_t tb6612fng_as_motor_driver(TB6612FNG_t *tb6612fng);
