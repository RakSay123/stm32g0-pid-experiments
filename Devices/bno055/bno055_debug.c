#include "bno055_debug.h"

#define BNO055_CHIP_ID  0xA0

uint8_t bno055_verify_chip_id(BNO055_t *dev, USART_TypeDef *USARTx)
{
	uint8_t id = bno055_get_chip_id(dev);

	uart_write_str(USARTx, "[BNO055] Device CHIP_ID: ");
	uart_write_hex(USARTx, id);
	uart_write_line(USARTx, "");

	if (id == BNO055_CHIP_ID) uart_write_line(USARTx, "[BNO055] Device found! ");
	else uart_write_line(USARTx, "[BNO055] Device not found");

	return (id == BNO055_CHIP_ID);
}

void bno055_print_calib_stat(BNO055_t *dev, USART_TypeDef *USARTx)
{
	uint8_t calib = bno055_read_calib_stat(dev);

	uart_write_str(USARTx, "[BNO055] Calibration Code: ");
	uart_write_hex(USARTx, calib);
	uart_write_line(USARTx, "");
}

void bno055_print_page(BNO055_t *dev, USART_TypeDef *USARTx)
{
	BNO055_PAGE_t page = bno055_read_page(dev);

	uart_write_str(USARTx, "[BNO055] Page Code: ");
	uart_write_hex(USARTx, (uint8_t)page);
	uart_write_line(USARTx, "");

	uart_write_str(USARTx, "[BNO055] Page ");

	if (page == 0x00) uart_write_str(USARTx, "0");
	else if (page == 0x01) uart_write_str(USARTx, "1");
	else uart_write_str(USARTx, "Unknown");

	uart_write_line(USARTx, "");
}

void bno055_print_euler(BNO055_t *dev, USART_TypeDef *USARTx)
{
	BNO055_Euler_t euler;

	bno055_read_euler(dev, &euler);

	uart_write_str(USARTx, "[BNO055] Heading: ");
	uart_write_float(USARTx, euler.heading);
	uart_write_line(USARTx, "");

	uart_write_str(USARTx, "[BNO055] Roll: ");
	uart_write_float(USARTx, euler.roll);
	uart_write_line(USARTx, "");

	uart_write_str(USARTx, "[BNO055] Pitch: ");
	uart_write_float(USARTx, euler.pitch);
	uart_write_line(USARTx, "");
}

void bno055_print_sys_status(BNO055_t *dev, USART_TypeDef *USARTx)
{
	uint8_t status = bno055_read_sys_status(dev);

	uart_write_str(USARTx, "[BNO055] System Status Code: ");
	uart_write_hex(USARTx, status);
	uart_write_line(USARTx, "");

	uart_write_str(USARTx, "[BNO055] System Status: ");
	if (status == 0x00) uart_write_str(USARTx, "System idle");
	else if (status == 0x01) uart_write_str(USARTx, "System error");
	else if (status == 0x02) uart_write_str(USARTx, "Initializing peripherals");
	else if (status == 0x03) uart_write_str(USARTx, "System initialization");
	else if (status == 0x04) uart_write_str(USARTx, "Executing self test");
	else if (status == 0x05) uart_write_str(USARTx, "Sensor fusion algorithm running");
	else if (status == 0x06) uart_write_str(USARTx, "Sensor running without fusion algorithm");
	else uart_write_str(USARTx, "System status unknown");

	uart_write_line(USARTx, "");
}

void bno055_print_sys_err(BNO055_t *dev, USART_TypeDef *USARTx)
{
	uint8_t error = bno055_read_sys_err(dev);

	uart_write_str(USARTx, "[BNO055] System Error Code: ");
	uart_write_hex(USARTx, error);
	uart_write_line(USARTx, "");

	uart_write_str(USARTx, "[BNO055] System Error: ");
	if (error == 0x00) uart_write_str(USARTx, "No error");
	else if (error == 0x01) uart_write_str(USARTx, "Peripheral initialization error");
	else if (error == 0x02) uart_write_str(USARTx, "System initialization error");
	else if (error == 0x03) uart_write_str(USARTx, "Self test result failed");
	else if (error == 0x04) uart_write_str(USARTx, "Register map value out of range");
	else if (error == 0x05) uart_write_str(USARTx, "Register map address out of range");
	else if (error == 0x06) uart_write_str(USARTx, "Register map write error");
	else if (error == 0x07) uart_write_str(USARTx, "BNO low power mode not available for selected operation mode");
	else if (error == 0x08) uart_write_str(USARTx, "Accelerometer power mode not available");
	else if (error == 0x09) uart_write_str(USARTx, "Fusion algorithm configuration error");
	else if (error == 0x0A) uart_write_str(USARTx, "Sensor configuration error");
	else  uart_write_str(USARTx, "System error unknown");

	uart_write_line(USARTx, "");
}

void bno055_print_mode(BNO055_t *dev, USART_TypeDef *USARTx)
{
	uint8_t mode = bno055_read_mode(dev);

	uart_write_str(USARTx, "[BNO055] Mode Code: ");
	uart_write_hex(USARTx, mode);
	uart_write_line(USARTx, "");

	uart_write_str(USARTx, "[BNO055] Operation Mode: ");

	if (mode == 0x00) uart_write_str(USARTx, "CONFIGMODE");
	else if (mode == 0x01) uart_write_str(USARTx, "ACCONLY");
	else if (mode == 0x02) uart_write_str(USARTx, "MAGONLY");
	else if (mode == 0x03) uart_write_str(USARTx, "GYROONLY");
	else if (mode == 0x04) uart_write_str(USARTx, "ACCMAG");
	else if (mode == 0x05) uart_write_str(USARTx, "ACCGYRO");
	else if (mode == 0x06) uart_write_str(USARTx, "MAGGYRO");
	else if (mode == 0x07) uart_write_str(USARTx, "AMG");
	else if (mode == 0x08) uart_write_str(USARTx, "IMU");
	else if (mode == 0x09) uart_write_str(USARTx, "COMPASS");
	else if (mode == 0x0A) uart_write_str(USARTx, "M4G");
	else if (mode == 0x0B) uart_write_str(USARTx, "NDOF_FMC_OFF");
	else if (mode == 0x0C) uart_write_str(USARTx, "NDOF");
	else uart_write_str(USARTx, "Unknown");

	uart_write_line(USARTx, "");
}

void bno055_print_all(BNO055_t *dev, USART_TypeDef *USARTx)
{
	bno055_print_mode(dev, USARTx);
	bno055_print_calib_stat(dev, USARTx);
	bno055_print_sys_status(dev, USARTx);
	bno055_print_sys_err(dev, USARTx);

	bno055_print_euler(dev, USARTx);

	uart_write_line(USARTx, "");
}
