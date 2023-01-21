/*
 * UART.hpp
 *
 *  Created on: 28 nov. 2022
 *      Author: Pablo
 */
#include "Communication/UART/UART.hpp"

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
	array<uint8_t, 1> arr {data};
    return transmit(id, arr);
}

bool UART::transmit_polling(uint8_t id, uint8_t data){
    if (not UART::registered_uart.contains(id))
        return false; //TODO: Error handler

    UART_HandleTypeDef* handle = get_handle(id);

    if((handle->ErrorCode & TXBUSYMASK) == 1)
       return false;

    if (HAL_UART_Transmit(handle, &data, 1, 10) != HAL_OK){
        return false; //TODO: Warning, Error during transmision
    }

    return true;
}

bool UART::transmit_polling(uint8_t id, uint8_t* data, int16_t size){
    if (not UART::registered_uart.contains(id))
        return false; //TODO: Error handler

    UART_HandleTypeDef* handle = get_handle(id);

    if((handle->ErrorCode & TXBUSYMASK) == 1)
       return false;

    if (HAL_UART_Transmit(handle, data, size, 10) != HAL_OK){
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

    UART_HandleTypeDef* handle = get_handle(id);

    if(((handle->RxState & RXBUSYMASK) >> 1) == 1)
       return false;


    if (HAL_UART_Receive(handle, data, size, 10) != HAL_OK) {
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

    UART_HandleTypeDef* handle = get_handle(id);

    if((handle->ErrorCode & TXBUSYMASK) == 1)
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

void UART::print_by_uart(string str, int len) {
		if (!UART::printf_ready) {
			return;
		}

		vector<uint8_t> myVector(str.begin(), str.end());
		uint8_t *ptr = &myVector[0];
		UART::transmit_polling(UART::printf_uart, ptr, static_cast<uint16_t>(len));
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *uart){
    //TODO: Fault, UART error
    return;
}

void UART::init(UART::Instance* uart){

	if (uart->initialized) {
		return;
	}

	UART_HandleTypeDef* handle = uart->huart;
	handle->Instance = uart->instance;
	handle->Init.BaudRate = uart->baud_rate;
	handle->Init.WordLength = uart->word_length;
	handle->Init.StopBits = UART_STOPBITS_1;
	handle->Init.Parity = UART_PARITY_NONE;
	handle->Init.Mode = UART_MODE_TX_RX;
	handle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	handle->Init.OverSampling = UART_OVERSAMPLING_16;
	handle->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	handle->Init.ClockPrescaler = UART_PRESCALER_DIV1;
	handle->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	if (HAL_UART_Init(handle) != HAL_OK){
		//TODO: Error Handler
	}

	if (HAL_UARTEx_SetTxFifoThreshold(handle, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK){
		//TODO: Error Handler
	}

	if (HAL_UARTEx_SetRxFifoThreshold(handle, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK){
		//TODO: Error Handler
	}

	if (HAL_UARTEx_DisableFifoMode(handle) != HAL_OK){
		//TODO: Error Handler
	}

	uart->initialized = true;
}

UART_HandleTypeDef* UART::get_handle(uint8_t id) {
	return registered_uart[id]->huart;
}

#ifdef __cplusplus
extern "C" {
#endif

int _write(int file, string str, int len) {

    UART::print_by_uart(str, len);
    return len;
}

#ifdef __cplusplus
}
#endif

