/*
 * UART.hpp
 *
 *  Created on: 28 nov. 2022
 *      Author: Pablo
 */
#include "Communication/UART/UART.hpp"

#ifdef HAL_UART_MODULE_ENABLED

unordered_map<uint8_t, UART::Instance* > UART::registered_uart;

uint16_t UART::id_counter = 0;

optional<uint8_t> UART::inscribe(UART::Peripheral& uart){
	if ( !UART::available_uarts.contains(uart)){
		return nullopt; //TODO: Error handler or throw exception
	}

	UART::Instance*	uart_instance = UART::available_uarts[uart];

    Pin::inscribe(uart_instance->TX, ALTERNATIVE);
    Pin::inscribe(uart_instance->RX, ALTERNATIVE);

    uint8_t id = UART::id_counter++;

    UART::registered_uart[id] = uart_instance;

    return id;
}

void UART::start(){
		for(auto iter: UART::registered_uart){
			UART::init(iter.second);
		}
}

bool UART::transmit(uint8_t id, uint8_t data){
    if (not UART::registered_uart.contains(id))
        return false; //TODO: Error handler

    UART::Instance* uart = UART::registered_uart[id];

    if((uart->huart->ErrorCode & TXBUSYMASK) == 1)
       return false;

    if (HAL_UART_Transmit_DMA(uart->huart, &data, 1) != HAL_OK){
        return false; //TODO: Warning, Error during transmision
    }

    return true;
}

bool UART::transmit(uint8_t id, uint8_t* data, int16_t size){
    if (not UART::registered_uart.contains(id))
        return false; //TODO: Error handler

    UART::Instance* uart = UART::registered_uart[id];

    if((uart->huart->ErrorCode & TXBUSYMASK) == 1)
       return false;

    if (HAL_UART_Transmit_DMA(uart->huart, data, size) != HAL_OK){
        return false; //TODO: Warning, Error during transmision
    }

    return true;
}

bool UART::transmit_polling(uint8_t id, uint8_t data){
    if (not UART::registered_uart.contains(id))
        return false; //TODO: Error handler

    UART::Instance* uart = UART::registered_uart[id];

    if((uart->huart->ErrorCode & TXBUSYMASK) == 1)
       return false;

    if (HAL_UART_Transmit(uart->huart, &data, 1, 10) != HAL_OK){
        return false; //TODO: Warning, Error during transmision
    }

    return true;
}

bool UART::transmit_polling(uint8_t id, uint8_t* data, int16_t size){
    if (not UART::registered_uart.contains(id))
        return false; //TODO: Error handler

    UART::Instance* uart = UART::registered_uart[id];

    if((uart->huart->ErrorCode & TXBUSYMASK) == 1)
       return false;

    if (HAL_UART_Transmit(uart->huart, data, size, 10) != HAL_OK){
        return false; //TODO: Warning, Error during transmision
    }

    return true;
}

bool UART::receive(uint8_t id, uint8_t* data, uint16_t size){
    if (not UART::registered_uart.contains(id))
        return false; //TODO: Error handler

    UART::Instance* uart = UART::registered_uart[id];

    if(((uart->huart->RxState & RXBUSYMASK) >> 1) == 1)
       return false;


    if (HAL_UART_Receive_DMA(uart->huart, data, size) != HAL_OK) {
        return false; //TODO: Warning, Error during receive
    }

    uart->receive_ready = false;

    return true;
}

bool UART::receive_polling(uint8_t id, uint8_t* data, uint16_t size){
    if (not UART::registered_uart.contains(id))
        return false; //TODO: Error handler

    UART::Instance* uart = UART::registered_uart[id];

    if(((uart->huart->RxState & RXBUSYMASK) >> 1) == 1)
       return false;


    if (HAL_UART_Receive(uart->huart, data, size, 10) != HAL_OK) {
        return false; //TODO: Warning, Error during receive
    }

    return true;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart){
    auto result = find_if(UART::registered_uart.begin(), UART::registered_uart.end(), [&](auto uart){return uart.second->huart == huart;});

    if (result != UART::registered_uart.end()) {
        (*result).second->receive_ready = true;
    }else{
        __NOP();//TODO: Warning: Data received form an unknown UART
    }
}

bool UART::has_next_packet(uint8_t id){
    if (not UART::registered_uart.contains(id))
        return false; //TODO: Error handler

    UART::Instance* uart = UART::registered_uart[id];

    return uart->receive_ready;
}

bool UART::is_busy(uint8_t id){
    if (not UART::registered_uart.contains(id))
        return false; //TODO: Error handler

    UART::Instance* uart = UART::registered_uart[id];

    if((uart->huart->ErrorCode & TXBUSYMASK) == 1)
       return false;

    return false;
}

bool UART::set_up_printf(UART::Peripheral& uart){
		if (printf_ready) {
			return false;
		}

		optional<uint8_t> id = UART::inscribe(uart);

		if(!id.has_value()){
			return false; //TODO: Error handler, no se ha podido inscribir el uart
		}
		UART::printf_uart = id.value();
		setvbuf(stdout, NULL, _IONBF, 0);
		setvbuf(stderr, NULL, _IONBF, 0);

		UART::printf_ready = true;

		return UART::printf_ready;
	}

void UART::print_by_uart(char *ptr, int len){
		if (!UART::printf_ready) {
			return;
		}

		UART::transmit_polling(UART::printf_uart, (uint8_t*)ptr, static_cast<uint16_t>(len));
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *uart){
    //TODO: Fault, UART error
    return;
}

void UART::init(UART::Instance* uart){

	if (uart->initialized) {
		return;
	}

	uart->huart->Instance = uart->instance;
	uart->huart->Init.BaudRate = uart->baud_rate;
	uart->huart->Init.WordLength = uart->word_length;
	uart->huart->Init.StopBits = UART_STOPBITS_1;
	uart->huart->Init.Parity = UART_PARITY_NONE;
	uart->huart->Init.Mode = UART_MODE_TX_RX;
	uart->huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart->huart->Init.OverSampling = UART_OVERSAMPLING_16;
	uart->huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	uart->huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;
	uart->huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	if (HAL_UART_Init(uart->huart) != HAL_OK){
		//TODO: Error Handler
	}

	if (HAL_UARTEx_SetTxFifoThreshold(uart->huart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK){
		//TODO: Error Handler
	}

	if (HAL_UARTEx_SetRxFifoThreshold(uart->huart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK){
		//TODO: Error Handler
	}

	if (HAL_UARTEx_DisableFifoMode(uart->huart) != HAL_OK){
		//TODO: Error Handler
	}

	uart->initialized = true;
}

#ifdef __cplusplus
extern "C" {
#endif

int _write(int file, char *ptr, int len){
    UART::print_by_uart(ptr, len);
    return len;
}

#ifdef __cplusplus
}
#endif

#endif
