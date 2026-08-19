#include "bno055.h"

#define CHIP_ID_REG 	0x00
#define PAGE_ID 		0x07
#define EUL_DATA_X_LSB  0x1A
#define CALIB_STAT 		0x35
#define SYS_STATUS 		0x39
#define SYS_ERR 		0x3A
#define UNIT_SEL 		0x3B
#define OPR_MODE 		0x3D
#define PWR_MODE        0x3E

#define BNO055_CHIP_ID  0xA0

static I2C_Status_t bno055_read_u8(BNO055_t *dev, uint8_t reg, uint8_t *value)
{
	return i2c_read_register(dev->i2c, dev->addr, reg, value, 1);
}

static I2C_Status_t bno055_write_u8(BNO055_t *dev, uint8_t reg, uint8_t value)
{
	return i2c_write_register(dev->i2c, dev->addr, reg, &value, 1);
}

uint8_t bno055_get_chip_id(BNO055_t *dev)
{
	uint8_t id;
	I2C_Status_t status = bno055_read_u8(dev, CHIP_ID_REG, &id);

	if (status != I2C_OK)
	{
		return 0;
	}

	return id;
}

uint8_t bno055_wait_for_mode(BNO055_t *dev, uint8_t expected)
{
    uint32_t timeout = 100;

    while (timeout--)
    {
        uint8_t mode;
        bno055_read_u8(dev, OPR_MODE, &mode);

        if ((mode & 0x0F) == expected) return 1;

        systick_delay_ms(10);
    }

    return 0;
}

I2C_Status_t bno055_set_operation_mode(BNO055_t *dev, BNO055_OPERATION_MODE_t mode)
{
	return bno055_write_u8(dev, OPR_MODE, (uint8_t)mode);
}

I2C_Status_t bno055_set_page(BNO055_t *dev, BNO055_PAGE_t page)
{
	return bno055_write_u8(dev, PAGE_ID, (uint8_t)page);
}

I2C_Status_t bno055_set_units(BNO055_t *dev, uint8_t units)
{
	return bno055_write_u8(dev, UNIT_SEL, units);
}

I2C_Status_t bno055_set_power_mode(BNO055_t *dev, BNO055_POWER_MODE_t mode)
{
	return bno055_write_u8(dev, PWR_MODE, (uint8_t)mode);
}

I2C_Status_t bno055_init(BNO055_t *dev, BNO055_Config_t *cfg)
{
	systick_delay_ms(650);

	uint8_t id;

	do {
		id = bno055_get_chip_id(dev);
		systick_delay_ms(100);
	} while (id != BNO055_CHIP_ID);

	if (bno055_set_operation_mode(dev, BNO055_OP_MODE_CONFIG) != I2C_OK) return I2C_ERR;
	systick_delay_ms(100);

	if (!bno055_wait_for_mode(dev, (uint8_t)BNO055_OP_MODE_CONFIG)) return I2C_ERR_TIMEOUT;

	if (bno055_set_page(dev, cfg->page) != I2C_OK) return I2C_ERR;
	systick_delay_ms(100);

	if (bno055_set_power_mode(dev, cfg->power) != I2C_OK) return I2C_ERR;
	systick_delay_ms(100);

	uint8_t units = (uint8_t)cfg->units;
	if (bno055_set_units(dev, units) != I2C_OK) return I2C_ERR;
	systick_delay_ms(100);

	if (bno055_set_operation_mode(dev, cfg->operation) != I2C_OK) return I2C_ERR;
	systick_delay_ms(1000);

	if (!bno055_wait_for_mode(dev, (uint8_t)cfg->operation)) return I2C_ERR_TIMEOUT;

	return I2C_OK;
}

uint8_t bno055_read_calib_stat(BNO055_t *dev)
{
	uint8_t calib;
	I2C_Status_t status = bno055_read_u8(dev, CALIB_STAT, &calib);

	if (status != I2C_OK)
	{
		return 0;
	}

	return calib;
}

BNO055_PAGE_t bno055_read_page(BNO055_t *dev)
{
	uint8_t page = 0;
	I2C_Status_t status = bno055_read_u8(dev, PAGE_ID, &page);

	if (status != I2C_OK)
	{
		return (BNO055_PAGE_t)0;
	}

	return (BNO055_PAGE_t)page;
}

uint8_t bno055_read_units(BNO055_t *dev)
{
	uint8_t units;
	I2C_Status_t status = bno055_read_u8(dev, UNIT_SEL, &units);

	if (status != I2C_OK)
	{
		return 0;
	}

	return units;
}

void bno055_read_euler(BNO055_t *dev, BNO055_Euler_t *euler)
{
	uint8_t buffer[6] = {0};
	I2C_Status_t status = i2c_read_register(dev->i2c, dev->addr, EUL_DATA_X_LSB, buffer, 6);

	if (status != I2C_OK)
	{
		euler->heading = 0;
		euler->roll = 0;
		euler->pitch = 0;
		return;
	}

	int16_t h = (int16_t)((buffer[1] << 8) | (buffer[0]));
	int16_t r = (int16_t)((buffer[3] << 8) | (buffer[2]));
	int16_t p = (int16_t)((buffer[5] << 8) | (buffer[4]));

	euler->heading = h / 16.0f;
	euler->roll = r / 16.0f;
	euler->pitch = p / 16.0f;
}

uint8_t bno055_read_sys_status(BNO055_t *dev)
{
	uint8_t status_code = 0;
	I2C_Status_t status = bno055_read_u8(dev, SYS_STATUS, &status_code);

	if (status != I2C_OK)
	{
		return 0;
	}

	return status_code;
}

uint8_t bno055_read_sys_err(BNO055_t *dev)
{
	uint8_t error = 0;
	I2C_Status_t status = bno055_read_u8(dev, SYS_ERR, &error);

	if (status != I2C_OK)
	{
		return 0;
	}

	return error;
}

uint8_t bno055_read_mode(BNO055_t *dev)
{
	uint8_t mode;
	I2C_Status_t status = bno055_read_u8(dev, OPR_MODE, &mode);

	if (status != I2C_OK)
	{
		return 0;
	}

	return (mode & 0x0F);
}
