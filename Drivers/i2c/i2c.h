#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include "stm32g070xx.h"

/**
 * @brief I2C configuration structure
 */
typedef struct {
    uint32_t pclk;   /**< Peripheral clock frequency (Hz) */
    uint32_t speed;  /**< I2C bus speed (Hz), e.g. 100kHz */
} I2C_Config_t;

/**
 * @brief I2C status codes
 */
typedef enum {
    I2C_OK = 0,
    I2C_ERR_NACK = -1,
    I2C_ERR_TIMEOUT = -2,
	I2C_ERR = -3
} I2C_Status_t;

/*
 * @brief I2C repeated start status codes
 */
typedef enum {
	I2C_NOT_REPEATED = 0,
	I2C_REPEATED = 1
} I2C_Repeated_Status_t;

/*
 * @brief I2C R/W operation specification
 */
typedef enum {
	I2C_WRITE = 0,
	I2C_READ = 1
} I2C_Direction_t;

/**
 * @brief Initialize I2C peripheral
 */
void i2c_init(I2C_TypeDef *I2Cx, const I2C_Config_t *cfg);

/**
 * @brief Write multiple bytes to a device register
 *
 * @return I2C_Status_t
 *
 * @note Performs:
 * START → REG → DATA → STOP
 */
I2C_Status_t i2c_write_register(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len);

/**
 * @brief Read multiple bytes from a device register
 *
 * @details Performs:
 * START → REG → (TC) → RESTART → READ → STOP
 *
 * @return I2C_Status_t
 */
I2C_Status_t i2c_read_register(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len);


#endif /* I2C_H_ */
