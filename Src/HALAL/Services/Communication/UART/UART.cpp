/*
 * UART.hpp
 *
 *  Created on: 28 nov. 2022
 *      Author: Pablo
 */
#include "Communication/UART/UART.hpp"

#ifdef HAL_UART_MODULE_ENABLED

UART::Instance UART::instance3 = { .TX = PC10, .RX = PB2, .huart = &huart3,
								   .instance = USART3,
                               };
UART::Peripheral UART::uart3 = UART::Peripheral::peripheral3;


unordered_map<uint8_t, UART::Instance* > UART::registered_uart;
unordered_map<UART::Peripheral, UART::Instance*> UART::available_uarts = {
	{UART::uart3, &UART::instance3}
};

uint16_t UART::id_counter = 0;

optional<uint8_t> UART::inscribe(UART::Peripheral& uart){
	if ( !UART::available_uarts.contains(uart)){
		return nullopt; //TODO: Error handler or throw exception
	}

	UART::Instance*	uart_instance = UART::available_uarts[uart];

    Pin::inscribe(uart_instance->TX, ALTERNATIVE);
    Pin::inscribe(uart_instance->RX, ALTERNATIVE);

    uint8_t id = UART::id_counter++;

    return id;
}

void UART::start(){
		for_each(UART::registered_uart.begin(), UART::registered_uart.end(),
				[](pair<uint8_t, UART::Instance*> iter) { UART::init(iter.second); }
		);
}

bool UART::transmit_next_packet(uint8_t id, RawPacket& packet){
    if (not UART::registered_uart.contains(id))
        return false; //TODO: Error handler

    UART::Instance* uart = UART::registered_uart[id];

    if((uart->huart->ErrorCode & TXBUSYMASK) == 1)
       return false;

    if (HAL_UART_Transmit_DMA(uart->huart, packet.get_data(), packet.get_size()) != HAL_OK){
        return false; //TODO: Warning, Error during transmision
    }

    return true;
}

bool UART::receive_next_packet(uint8_t id, RawPacket& packet){
    if (not UART::registered_uart.contains(id))
        return false; //TODO: Error handler

    UART::Instance* uart = UART::registered_uart[id];

    if(((uart->huart->RxState & RXBUSYMASK) >> 1) == 1)
       return false;

    *packet.get_data() = 0;

    if (HAL_UART_Receive_DMA(uart->huart, packet.get_data(), packet.get_size()) != HAL_OK) {
        return false; //TODO: Warning, Error during receive
    }

    uart->receive_ready = false;
    return true;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart){
    auto result = find_if(UART::registered_uart.begin(), UART::registered_uart.end(), [&](auto uart){return uart.second->huart == huart;});

    if (result != UART::registered_uart.end()) {
        (*result).second->receive_ready = true;
    }else{
        //TODO: Warning: Data receive form an unknown UART
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

#endif
