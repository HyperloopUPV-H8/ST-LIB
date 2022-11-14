/*
 * SPI.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */
#include "Communication/SPI/SPI.hpp"

////////////////////////////////SPI::Peripheral////////////////////////////////
void SPI::Peripheral::set_rx_status(bool new_status){
	this->rx_status = new_status;
}

void SPI::Peripheral::set_tx_status(bool new_status){
	this->tx_status = new_status;
}

SPI::Peripheral SPI::peripheral1 = { .SCK = PC10, .MOSI = PB2, .MISO = PC11, .SS = PA4,
									 .hspi = &hspi3,
									 .tx_status = true, .tx_queue = queue<SPIPacket>(),
									 .rx_status = true, .rx_buffer = nullopt,
									};

//////////////////////////////////////SPI//////////////////////////////////////
forward_list<uint8_t> SPI::ID_manager = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255 };

unordered_map<uint8_t, SPI::Peripheral* > SPI::registered_spi;

optional<uint8_t> SPI::register_SPI(SPI::Peripheral& spi){

	Pin::register_pin(spi.SCK, ALTERNATIVE);
    Pin::register_pin(spi.MOSI, ALTERNATIVE);
    Pin::register_pin(spi.MISO, ALTERNATIVE);
    Pin::register_pin(spi.SS, ALTERNATIVE);

    uint8_t id = SPI::ID_manager.front();

    SPI::registered_spi[id] = &spi;

    SPI::ID_manager.pop_front();

    return id;
}

//////////////////////////////////////TX//////////////////////////////////////

void SPI::send_next_packet(uint8_t id, SPIPacket& packet){
    if (!SPI::registered_spi.contains(id))
    	return; //TODO: Handle exception if needed

    SPI::registered_spi[id]->tx_queue.push(packet);

    try_send_next_packet(id);
}

void SPI::try_send_next_packet(uint8_t id) {
    if (!SPI::registered_spi.contains(id))
       return; //TODO: Handle exception if needed

    SPI::Peripheral* spi = SPI::registered_spi[id];

    if (!spi->tx_status || spi->tx_queue.empty())
        return;

    SPIPacket next_packet = spi->tx_queue.front();

    if (HAL_SPI_Transmit_DMA(spi->hspi, next_packet.get_data(), next_packet.get_size()) != HAL_OK)
    	return; //TODO: Warning, Error during tranmision

    spi->set_tx_status(false);
    spi->tx_queue.pop();
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi){

    for (auto& i: SPI::registered_spi){
    	uint8_t id = i.first;
    	auto spi = i.second;

        if (spi->hspi == hspi) {
            spi->set_tx_status(true);
            SPI::try_send_next_packet(id);
            return;
        }
    }
}

void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi){
	__NOP();
}


//////////////////////////////////////RX//////////////////////////////////////
optional<SPIPacket> SPI::get_next_packet(uint8_t id, uint16_t data_size){
	if (!SPI::registered_spi.contains(id))
		return nullopt; //TODO: Handle exception if needed

	SPI::Peripheral* spi = SPI::registered_spi[id];

	if (!spi->rx_buffer.has_value()) {
		spi->rx_buffer = SPIPacket((uint8_t*)0, data_size);

		if (HAL_SPI_Receive_DMA(spi->hspi, spi->rx_buffer.value().get_data(), data_size) != HAL_OK) {
			spi->rx_buffer = nullopt;
			return nullopt; //TODO: Handle exception if needed
		}

		spi->set_rx_status(false);

		return nullopt;
	}

	if (!spi->rx_status) {
		return nullopt;
	}

	SPIPacket result = spi->rx_buffer.value();
	spi->rx_buffer = nullopt;

	return result;
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
	for (auto i: SPI::registered_spi){
		auto spi = i.second;

		if (spi->hspi == hspi) {
			spi->rx_status = true;
			return;
		}
	}
}

/////////////////////////////General error Handling////////////////////////////
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi){
	//TODO: Handle SPI ERROR if needed
	return;
}
