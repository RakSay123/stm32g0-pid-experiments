#ifndef BNO055_H_
#define BNO055_H_

#include <stdint.h>
#include "i2c/i2c.h"
#include "systick/systick.h"

typedef struct {
	I2C_TypeDef *i2c;
	uint8_t addr;
} BNO055_t;

typedef enum {
	BNO055_OP_MODE_CONFIG = 0x00,
	BNO055_OP_MODE_ACCONLY = 0x01,
	BNO055_OP_MODE_MAGONLY = 0x02,
	BNO055_OP_MODE_GYROONLY = 0x03,
	BNO055_OP_MODE_ACCMAG = 0x04,
	BNO055_OP_MODE_ACCGYRO = 0x05,
	BNO055_OP_MODE_MAGGYRO = 0x06,
	BNO055_OP_MODE_AMG = 0x07,
	BNO055_OP_MODE_IMU = 0x08,
	BNO055_OP_MODE_COMPASS = 0x09,
	BNO055_OP_MODE_M4G = 0x0A,
	BNO055_OP_MODE_NDOF_FMC_OFF = 0x0B,
	BNO055_OP_MODE_NDOF = 0x0C,
} BNO055_OPERATION_MODE_t;

typedef enum {
	BNO055_PWR_MODE_NORMAL = 0x00,
	BNO055_PWR_MODE_LOW = 0x01,
	BNO055_PWR_MODE_SUSPEND = 0x02,
	BNO055_PWR_MODE_INVALID= 0x03,
} BNO055_POWER_MODE_t;

typedef enum {
	BNO055_PAGE_0 = 0,
	BNO055_PAGE_1 = 1
} BNO055_PAGE_t;

typedef enum {
	BNO055_ACC_METERS_PER_SECOND_SQUARED = (0 << 0),
	BNO055_ACC_MILLIGAL = (1 << 0),
	BNO055_GYR_DEGREES_PER_SECOND = (0 << 1),
	BNO055_GYR_RADIANS_PER_SECOND = (1 << 1),
	BNO055_EUL_DEGREES = (0 << 2),
	BNO055_EUL_RADIANS = (1 << 2),
	BNO055_TEMP_CELSIUS = (0 << 4),
	BNO055_TEMP_FAHRENHEIT = (1 << 4),
	BNO055_ORI_WINDOWS = (0 << 7),
	BNO055_ORI_ANDROID = (1 << 7)
} BNO055_UNIT_SEL_t;

typedef enum {
	BNO055_WRITE_MODE = 0,
	BNO055_WRITE_PAGE = 1,
	BNO055_WRITE_UNITS = 2,
	BNO055_WRITE_POWER = 3
} BNO055_WRITE_FUNCTION_t;

typedef struct {
	 float heading;
	 float roll;
	 float pitch;
} BNO055_Euler_t;

typedef struct {
	BNO055_PAGE_t page;
	BNO055_OPERATION_MODE_t operation;
	BNO055_POWER_MODE_t power;
	BNO055_UNIT_SEL_t units;
} BNO055_Config_t;

#include "bno055.h"

uint8_t bno055_get_chip_id(BNO055_t *dev);

uint8_t bno055_wait_for_mode(BNO055_t *dev, uint8_t expected);

I2C_Status_t bno055_set_operation_mode(BNO055_t *dev, BNO055_OPERATION_MODE_t mode);

I2C_Status_t bno055_set_page(BNO055_t *dev, BNO055_PAGE_t page);

I2C_Status_t bno055_set_units(BNO055_t *dev, uint8_t units);

I2C_Status_t bno055_set_power_mode(BNO055_t *dev, BNO055_POWER_MODE_t mode);

I2C_Status_t bno055_init(BNO055_t *dev, BNO055_Config_t *cfg);

uint8_t bno055_read_calib_stat(BNO055_t *dev);

BNO055_PAGE_t bno055_read_page(BNO055_t *dev);

uint8_t bno055_read_units(BNO055_t *dev);

void bno055_read_euler(BNO055_t *dev, BNO055_Euler_t *euler);

uint8_t bno055_read_sys_status(BNO055_t *dev);

uint8_t bno055_read_sys_err(BNO055_t *dev);

uint8_t bno055_read_mode(BNO055_t *dev);

#endif /* BNO055_H_ */
