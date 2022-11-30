/*
 * SPI.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */

#pragma once

#include "PinModel/Pin.hpp"
#include "Packets/RawPacket.hpp"

#ifdef HAL_SPI_MODULE_ENABLED

extern SPI_HandleTypeDef hspi3;

/**
 * @brief SPI service class. Abstracts the use of the SPI service of the HAL library.
 * 
 */
class SPI{
private:
    /**
     * @brief Struct wich defines all data refering to SPI peripherals. It is 
     *        declared private in order to prevent unwanted use. Only 
     *        predefined instaces should be used. 
     *           
     */
    struct Instance{
        Pin SCK; /**< Clock pin. */
        Pin MOSI; /**< MOSI pin. */
        Pin MISO; /**< MISO pin. */
        Pin SS; /**< Slave select pin. */
        SPI_HandleTypeDef* hspi;  /**< HAL spi struct pin. */  
        bool receive_ready = false; /**< Receive value is ready to use pin. */
    };

    /**
     * @brief Enum which abstracts the use of the Instance struct to facilitate the mocking of the HALAL.Struct
     *
     */
    enum Peripheral{
        peripheral3 = 2,
    };

public:
    static forward_list<uint8_t> id_manager;
    
    static unordered_map<uint8_t, SPI::Instance* > registered_spi;

    /**
     * @brief SPI 3 wrapper enum of the STM32H723.
     *
     */
    static SPI::Peripheral spi3;

    /**
     * @brief SPI 3 instance of the STM32H723.
     *
     */
    static SPI::Instance instance3;

    /**
     * @brief Registers a new SPI.
     * 
     * @param spi SPI peripheral to register.
     * @return uint8_t Id of the service.
     */
    static uint8_t inscribe(SPI::Peripheral& spi);

    /**@brief	Transmits 1 SPIPacket of any size by DMA and
     *          interrupts. Handles the packet size automatically. To
     *          to send various packets in a row you must check if the spi is busy
     *          using is_busy().
     * 
     * @param id Id of the SPI
     * @param packet Pakcet to be send
     * @return bool Returns true if the request to send the packet has been done
     *            successfully. Returns false if the SPI is busy or a problem
     *            has occurred.
     */
    static bool transmit_next_packet(uint8_t id, RawPacket& packet);

    /**						
     * @brief This method request the receive of a new SPIPacket of any size
     *        by DMA and interrupts. Thus the packet should not be used until
     *        you have checked that the value is already available using the 
     *        method has_next_paclet().
     * @see   SPI::has_next_packet()
     * 
     * @param id Id of the SPI
     * @param packet SPIPacket in which the data will be stored
     * @return bool Return true if the order to receive a new packet has been
     *            processed correctly. Return false if the SPI is busy or a
     *            problem has occurred.
     */
    static bool receive_next_packet(uint8_t id, RawPacket& packet);

    /**
     * @brief This method is used to check if the SPI receive operation has finished and data is ready.
     * 
     * @param id Id of the SPI
     * @return bool Return true if the packet is ready to use and false if not.
     */
    static bool has_next_packet(uint8_t id);

    /**
     * @brief This method is used to check if the SPI transmit operations busy.
     * 
     * @param id Id of the SPI
     * @return bool Return true if the SPI transmit operation is busy and false if not.
     */
    static bool is_busy(uint8_t id);
};

#endif
