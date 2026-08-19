#ifndef BNO055_DEBUG_H_
#define BNO055_DEBUG_H_

#include "uart/uart.h"
#include "bno055/bno055.h"

uint8_t bno055_print_chip_id(BNO055_t *dev, USART_TypeDef *USARTx);

void bno055_print_calib_stat(BNO055_t *dev, USART_TypeDef *USARTx);

void bno055_print_page(BNO055_t *dev, USART_TypeDef *USARTx);

void bno055_print_euler(BNO055_t *dev, USART_TypeDef *USARTx);

void bno055_print_sys_status(BNO055_t *dev, USART_TypeDef *USARTx);

void bno055_print_sys_err(BNO055_t *dev, USART_TypeDef *USARTx);

void bno055_print_mode(BNO055_t *dev, USART_TypeDef *USARTx);

void bno055_print_all(BNO055_t *dev, USART_TypeDef *USARTx);


#endif /* BNO055_DEBUG_H_ */
