/*
 * SPI.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */
#include "Communication/SPI/SPI.hpp"

extern SPI_HandleTypeDef hspi3;

extern SPIPeripheral SPIPeripheral1(PC10, PB2, PC11, PA4);

//TODO: A parte de todo lo que menciono abajo creo que tenemos que cambiar como estamos planteando
//      el tema de las clases de comunicación que son para perifericos, por ejemplo el SPI
//      deberiamos tener una clase para cada sensor que utilize las funciones de la clase SPI
//      para comunicarse, y es esta clase sensor la que deberia guardar ya en un paquete los 
//      datos obtenidos despues de realizar las medidas necesarias, es decir creo que (puede
//      que fuese yo solo el que entedia mal lo que se pretendia) estamos enfocando mal este servicio
//      que deberia limitarse a tener un buffer de salida y uno de entrada y despues tener funciones
//      read send y read que hagan simplemente eso leer y escribir en estos buffers.
//      Una clase por ejemplo IMU utilizara este servicio para obtener los datos de la 
//      IMU haciendo read a x frequencia, calcularlo todo y meterlo dentro de packetsvalues y a su vez
//      dentro packets. La clase SPI la voy ha hacer por DMA para ahorrar tiempo al procesador
//      por esto es posible que cuando pidas un dato aun no este y tengas que esperar al próximo read,
//      no pasaria nada pero habria que handlearlo en la clase IMU.


//TODO: Refactorizar todo, hacer que SPIPeripheral maneje todo, que tenga el HANDLE y el packet buffer,
//		es decir le pasas un hspi en vez de un SPIPeripheral y el te añade el SPIPeripheral al mapa de registrados.
//		Por tanto un mapa spi_un_map de HAndle a SPIPeripheral y un mapa registered de uint8_t a SPIPeripheral
// 		asi matamos tres pajaros de un tiro, el handleo del buffer de paquetes se hace dentro de la estructura de
//		SPIPeripheral, nos ahorramos un mapa, podemos buscar SPIPeripherals a partir de &hspi.
//
//		WARNING: 	Hay que tener en cuenta que al usar el spi_un_map para sacar el SPIPeripheral a partir de un hspi
//				 	podemos obtener un SPIPeripheral que no ha sido registrado, tengo que ver comom tengo en cuenta esto.

//TODO: Ortra cosa, me he dado cuenta que cuando registramos un periferico no lo eliminamos del mapa
// 		por tanto podrimaos volver a registrarlo sin querer tantas veces como queramos

unordered_map<uint8_t, pair<bool, queue<SPIPacket>>> SPI::tx_packet_buffer = {};

forward_list<uint8_t> SPI::ID_manager = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255 };

//TODO: ver si refactorizo esto para ahorrarme un mapa
unordered_map<SPIPeripheral, SPI_HandleTypeDef*, SPIPeripheral_hash_function> SPI::spi_un_map{
    {SPIPeripheral1, &hspi3},
};

unordered_map<uint8_t, SPI_HandleTypeDef*> SPI::registered_spi = {};

optional<uint8_t> SPI::register_SPI(SPIPeripheral& spi){
    if (!SPI::spi_un_map.contains(spi)){
    	 return nullopt;
    }

	Pin::register_pin(spi.SCK, ALTERNATIVE);
    Pin::register_pin(spi.MOSI, ALTERNATIVE);
    Pin::register_pin(spi.MISO, ALTERNATIVE);
    Pin::register_pin(spi.SS, ALTERNATIVE);

    uint8_t id = SPI::ID_manager.front();

    SPI::registered_spi[id] = SPI::spi_un_map[spi];
    SPI::tx_packet_buffer[id] = { true, queue<SPIPacket>() };

    SPI::ID_manager.pop_front();
    return id;
}

void SPI::send_packet_by_SPI(uint8_t id, SPIPacket& packet){
    if (!SPI::registered_spi.contains(id)){
    	return; //TODO: Handle exception if needed
    }

    SPI::tx_packet_buffer[id].second.push(packet);

    try_send_next_packet(id);
}

void SPI::try_send_next_packet(uint8_t id) {
    if (!SPI::registered_spi.contains(id))
       return; //TODO: Handle exception if needed

    pair<bool, queue<SPIPacket>>& buffer_status = SPI::tx_packet_buffer[id];

//    if (buffer_status != SPI::tx_packet_buffer[id]) {
//		__NOP();
//	}

    if (!buffer_status.first || buffer_status.second.empty())
        return;

    SPIPacket next_packet = buffer_status.second.front();
    SPI_HandleTypeDef* spi = SPI::registered_spi[id];

    if (HAL_SPI_Transmit_DMA(spi, next_packet.get_data(), next_packet.get_size()) != HAL_OK)
    	return; //TODO: Warning, Error during tranmision

    buffer_status.first = false;
    buffer_status.second.pop();

}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi){
    for (auto i: SPI::registered_spi){
        if (i.second == hspi) {
            SPI::tx_packet_buffer[i.first].first = true;
            SPI::try_send_next_packet(i.first);
            return;
        }
    }
}
