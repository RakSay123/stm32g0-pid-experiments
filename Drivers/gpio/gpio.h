#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>
#include "stm32g070xx.h"

/**
 * @brief GPIO pin operating modes (MODER register values)
 */
typedef enum {
	GPIO_MODE_INPUT = 0,
	GPIO_MODE_OUTPUT = 1,
	GPIO_MODE_AF = 2,
	GPIO_MODE_ANALOG = 3
} GPIO_Mode_t;

/**
 * @brief GPIO pin output types
 */
typedef enum {
	GPIO_PUSH_PULL = 0,
	GPIO_OPEN_DRAIN = 1
} GPIO_OType_t;

/**
 * @brief GPIO pin output speeds
 */
typedef enum {
	GPIO_SPEED_VERY_LOW = 0,
	GPIO_SPEED_LOW = 1,
	GPIO_SPEED_HIGH = 2,
	GPIO_SPEED_VERY_HIGH = 3
} GPIO_Speed_t;

/**
 * @brief GPIO pin internal pull up/pull down resistor configurations
 */
typedef enum {
	GPIO_NO_PULL = 0,
	GPIO_PULL_UP = 1,
	GPIO_PULL_DOWN = 2
} GPIO_Pull_t;

/**
 * @brief GPIO pin alternate function
 */
typedef enum {
	GPIO_AF0 = 0,
	GPIO_AF1 = 1,
	GPIO_AF2 = 2,
	GPIO_AF3 = 3,
	GPIO_AF4 = 4,
	GPIO_AF5 = 5,
	GPIO_AF6 = 6,
	GPIO_AF7 = 7
} GPIO_AF_t;

/**
 * @brief GPIO pin output states
 */
typedef enum {
	GPIO_LOW = 0,
	GPIO_HIGH = 1
} GPIO_State_t;

/**
 * @brief Configuration structure for GPIO initialization
 *
 * @details Contains all parameters required to configure a GPIO pin,
 *          including mode, output type, speed, pull-up/down, and alternate function.
 */
typedef struct {
	GPIO_TypeDef *port;
	uint8_t pin;
	GPIO_Mode_t mode;
	GPIO_OType_t otype;
	GPIO_Speed_t speed;
	GPIO_Pull_t pull;
	GPIO_AF_t alternate;
} GPIO_Config_t;

/**
 * @brief Turns on appropriate clock signal depending on which GPIO port is intended to be used
 *
 * @param[in]  GPIO_TypeDef *GPIOx  Input GPIO peripheral
 *
 * @return None
 *
 * @warning Passing an invalid GPIO port results in undefined behavior
 */
void gpio_enable_clock(GPIO_TypeDef *GPIOx);

/**
 * @brief Configures the mode of the GPIO pin (input, output, alternate function, analog)
 *
 * @details Output: Configures the MODER register for the specified pin
 * 			Input: Receives signals, like a button
 * 			Alternate Function: Selects alternate function by configuring AFRL/AFRH registers.
         	 	 	 	 	 	Required when pin is used by peripherals (USART, I2C, SPI, etc.)
 * 			Analog: Disables digital input/output buffers, reducing power consumption.
        			Used for ADC inputs or unused pins
 *
 * @param[in]  GPIOx  GPIO port used when configuring mode
 * @param[in]  pin    Pin used when configuring mode
 * @param[in]  mode   Given mode configuration
 *
 * @return None
 */
void gpio_mode_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Mode_t mode);

/**
 * @brief Configures the output type (push-pull, open drain)
 *
 * @details Push-pull: Pin can drive output HIGH and LOW
 *          Open drain: Pin can only drive output LOW, usually paired with internal pull up
 *
 * @param[in]  GPIOx  GPIO port used when configuring output type
 * @param[in]  pin    Pin used when configuring output type
 * @param[in]  otype  Given output type configuration
 *
 * @return None
 */
void gpio_output_type_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_OType_t otype);

/**
 * @brief Configures the output speed (very low, low, high, very high)
 *
 * @details Very Low: Up to 2 MHz with a 50 pF load
 * 			Low: Up to 10 MHz with a 50 pF load
 * 			High: Up to 30 MHz with a 50 pF load
 * 			Very High: Up to 60 MHz with a 30 pF load
 *
 * @param[in]  GPIOx  GPIO port used when configuring output speed
 * @param[in]  pin    Pin used when configuring output speed
 * @param[in]  speed  Given output speed configuration
 *
 * @return None
 */
void gpio_speed_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Speed_t speed);

/**
 * @brief Configures internal pull (no pull, pull up, pull down), useful for input mode
 *
 * @details No pull: Pin does not have a default "driving value"
 *          Pull up: Via a resistor, pin by default drives HIGH
 *          Pull down: Via a resistor, pin by default drives LOW
 *
 * @param[in]  GPIOx  GPIO port used when configuring internal pull
 * @param[in]  pin    Pin used when configuring internal pull
 * @param[in]  pull   Given internal pull configuration
 *
 * @return None
 */
void gpio_pull_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Pull_t pull);

/**
 * @brief Configures the alternate function for a pin
 *
 * @details AF0-AF7 differ for each pin, refer to reference manual to see
 *
 * @param[in]  GPIOx      GPIO port used when configuring AF
 * @param[in]  pin        Pin used when selecting AF
 * @param[in]  alternate  Given AF configuration
 *
 * @return None
 *
 * @note Be sure you have the correct AF configured
 */
void gpio_alternate_function_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_AF_t alternate);

/**
 * @brief Initializes GPIO pin to given configuration
 *
 * @details Configures only relevant registers based on selected mode to avoid unintended side effects.
 *
 * @param[in]  cfg  Struct that contains configuration info about port, pin, mode,
 *                  output type, output speed, internal pull, and alternate functions
 *
 * @return None
 */
void gpio_init(GPIO_Config_t *cfg);

/**
 * @brief Drives a given pin from a given port (HIGH or LOW) via the BSRR register
 *
 * @details Uses BSRR register for atomic set/reset operations.
 * 			Writing to lower 16 bits sets the pin, upper 16 bits resets it.
 *
 * @param[in]  GPIOx  GPIO port for the pin where output is to be driven from
 * @param[in]  pin    Pin where output is to be driven from
 * @param[in]  state  Output type
 *
 * @return None
 */
void gpio_write(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_State_t state);

/**
 * @brief Switches pin output from HIGH to LOW or LOW to HIGH from a specific pin
 *
 * @details Reads current output state from ODR and toggles using BSRR.
         	Ensures atomic write while relying on software state decision.
 *
 * @param[in]  GPIOx  GPIO port where pin is situated in
 * @param[in]  pin    Pin where output is to be switched
 *
 * @return None
 */
void gpio_toggle(GPIO_TypeDef *GPIOx, uint8_t pin);

/**
 * @brief Switches pin output from HIGH to LOW or LOW to HIGH from a specific pin (config)
 *
 * @details Reads current output state from ODR and toggles using BSRR.
         	Ensures atomic write while relying on software state decision.
 *
 * @param[in]  GPIO  GPIO pin and its configurations
 *
 * @return None
 */
void gpio_toggle_pin(GPIO_Config_t *GPIO);

/**
 * @brief Reads input from a specific pin using the IDR register
 *
 * @param[in]  GPIOx  GPIO port where pin is situated in
 * @param[in]  pin    Pin where input is read from
 *
 * @return GPIO_State_t  Either HIGH or LOW
 *
 * @note Reads actual pin state, which may differ from ODR due to external circuitry.
 */
GPIO_State_t gpio_read(GPIO_TypeDef *GPIOx, uint8_t pin);

#endif /* GPIO_H_ */
