
#include "ST-LIB.hpp"


#ifdef HAL_FDCAN_MODULE_ENABLED

extern FDCAN_HandleTypeDef hfdcan1;

FDCAN::Instance FDCAN::instance1 = { .TX = PD1, .RX = PD0, .hfdcan = &hfdcan1,
									 .instance = FDCAN1, .dlc = DLC::bytes_64,
									 .rx_location = FDCAN_RX_FIFO0, .fdcan_number = 1
									};

FDCAN::Peripheral FDCAN::fdcan1 = FDCAN::Peripheral::peripheral1;

unordered_map<FDCAN::Peripheral, FDCAN::Instance*> FDCAN::available_fdcans = {
		{FDCAN::fdcan1, &FDCAN::instance1}
};

unordered_map<FDCAN_HandleTypeDef*, FDCAN::Instance*> FDCAN::handle_to_fdcan = {
		{FDCAN::instance1.hfdcan, &FDCAN::instance1}
};


#endif
/************************************************
 *              Communication-SPI
 ***********************************************/
#undef HAL_SPI_MODULE_ENABLED
#ifdef HAL_SPI_MODULE_ENABLED
extern SPI_HandleTypeDef hspi3;


SPI::Instance SPI::instance3 = { .SCK = &PC10, .MOSI = &PC12, .MISO = &PC11, .SS = &PD0,
                                 .hspi = &hspi3, .instance = SPI3,
								 .baud_rate_prescaler = SPI_BAUDRATEPRESCALER_256,
                               };

SPI::Peripheral SPI::spi3 = SPI::Peripheral::peripheral3;

unordered_map<SPI::Peripheral, SPI::Instance*> SPI::available_spi = {
	{SPI::spi3, &SPI::instance3}
};
#endif
/************************************************
 *              Communication-UART
 ***********************************************/
#ifdef HAL_UART_MODULE_ENABLED
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

UART::Instance UART::instance1 = { .TX = PA9, .RX = PA10, .huart = &huart1,
								   .instance = USART1, .baud_rate = 115200, .word_length = UART_WORDLENGTH_8B,
                               };


UART::Instance UART::instance2 = { .TX = PD5, .RX = PD6, .huart = &huart2,
								   .instance = USART2, .baud_rate = 115200, .word_length = UART_WORDLENGTH_8B,
                               };


UART::Peripheral UART::uart1 = UART::Peripheral::peripheral1;
UART::Peripheral UART::uart2 = UART::Peripheral::peripheral2;

unordered_map<UART::Peripheral, UART::Instance*> UART::available_uarts = {
	{UART::uart1, &UART::instance1},
	{UART::uart2, &UART::instance2}
};

uint8_t UART::printf_uart = 0;
bool UART::printf_ready = false;

#endif
/************************************************
 *              Communication-I2C
 ***********************************************/
