/*
 * UART.hpp
 *
 *  Created on: 28 nov. 2022
 *      Author: Pablo
 */
#include "Communication/UART/UART.hpp"

UART::Instance UART::instance3 = { .SCK = PC10, .MOSI = PB2,
                                 .huart = &huart3
                               };

UART::Peripheral UART::uart3 = UART::Peripheral::peripheral3;

forward_list<uint8_t> UART::id_manager = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255 };

unordered_map<uint8_t, SPI::Instance* > UART::registered_uart;

uint8_t UART::inscribe(UART::Peripheral& uart){
	UART::Instance*	uart_instance;

    switch(uart){
        case UART::Peripheral::peripheral3:
        uart_instance = &UART::instance3;
        break;
    }

    Pin::inscribe(uart_instance->SCK, ALTERNATIVE);
    Pin::inscribe(uart_instance->MOSI, ALTERNATIVE);

    uint8_t id = UART::id_manager.front();

    UART::registered_uart[id] = uart_instance;

    UART::id_manager.pop_front();

    return id;
}

bool UART::transmit_next_packet(uint8_t id, RawPacket& packet){
    if (!UART::registered_uart.contains(id))
        return false; //TODO: Error handler

    UART::Instance* uart = UART::registered_spi[id];

    //TODO: No funciona asi en el uart
    if(uart->huart->State == HAL_SPI_STATE_BUSY_TX)
       return false;

    //TODO: no funciona asi
    //TODO: Cambiar a DMA en el SPI importante
    if (HAL_UART_Transmit_IT(spi->hspi, packet.get_data(), packet.get_size()) != HAL_OK){
        return false; //TODO: Warning, Error during transmision
    }

    return true;
}

bool UART::receive_next_packet(uint8_t id, RawPacket& packet){
    if (!UART::registered_uart.contains(id))
        return false; //TODO: Error handler

    UART::Instance* uart = UART::registered_uart[id];

    //TODO No funciona asi
    if(uart->huart->State == HAL_SPI_STATE_BUSY_RX)
       return false;

    *packet.get_data() = 0;

    //TODO: no funciona asi en el uart
    if (HAL_UART_Receive_DMA(spi->hspi, packet.get_data(), packet.get_size()) != HAL_OK) {
        return false; //TODO: Warning, Error during receive
    }

    uart->receive_ready = false;
    return true;
}

//TODO: Ver si se llama asi
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart){
    auto result = find_if(UART::registered_uart.begin(), UART::registered_uart.end(), [&](auto uart){return uart.second->huart == huart;});

    if (result != UART::registered_uart.end()) {
        (*result).second->receive_ready = true;
    }else{
        //TODO: Warning: Data receive form an unknown UART
    }
}

bool UART::has_next_packet(uint8_t id){
    if (!UART::registered_uart.contains(id))
        return false; //TODO: Error handler

    auto* uart = UART::registered_uart[id];

    return uart->receive_ready;
}

bool UART::is_busy(uint8_t id){
    if (!UART::registered_uart.contains(id))
        return false; //TODO: Error handler

    UART::Instance* uart = UART::registered_uart[id];

    //TODO no funciona asi en UART
    if(uart->hspi->State == HAL_SPI_STATE_BUSY_TX)
       return true;

    return false;
}

//TODO: Cambiar a uart
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi){
    //TODO: Fault, SPI error
    return;
}
