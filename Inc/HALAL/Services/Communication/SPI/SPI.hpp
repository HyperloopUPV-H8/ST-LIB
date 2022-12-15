/*
 * SPI.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */

#pragma once

#include "PinModel/Pin.hpp"
#include "Packets/RawPacket.hpp"
#include "DigitalOutputService/DigitalOutputService.hpp"

#ifdef HAL_SPI_MODULE_ENABLED

//TODO: Hay que hacer el Chip select funcione a traves de un GPIO en vez de a traves del periferico.

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
        SPI_TypeDef* instance; /**< HAL spi instance. */

        uint32_t baud_rate_prescaler; /**< SPI baud prescaler.*/
        uint32_t mode = SPI_MODE_MASTER; /**< SPI mode. */
        uint32_t data_size = SPI_DATASIZE_8BIT;  /**< SPI data size. Default 8 bit */
        uint32_t first_bit = SPI_FIRSTBIT_MSB; /**< SPI first bit,. */
        uint32_t clock_polarity = SPI_POLARITY_LOW; /**< SPI clock polarity. */
        uint32_t clock_phase = SPI_PHASE_1EDGE; /**< SPI clock phase. */
        uint32_t nss_polarity = SPI_NSS_POLARITY_LOW; /**< SPI chip select polarity. */
       
        uint8_t digital_output_ss; /**< ID of digital output service pin. */
        bool receive_ready = false; /**< Receive value is ready to use pin. */
        bool initialized = false; /**< Peripheral has already been initialized */
    };

    /**
     * @brief Enum which abstracts the use of the Instance struct to facilitate the mocking of the HALAL.Struct
     *
     */
    enum Peripheral{
     	peripheral1 = 0,
     	peripheral2 = 1,
        peripheral3 = 2,
 		peripheral4 = 3,
 		peripheral5 = 4,
 		peripheral6 = 5,
     };

public:
    static uint16_t id_counter;
    
    static unordered_map<uint8_t, SPI::Instance* > registered_spi;

    static unordered_map<SPI::Peripheral, SPI::Instance*> available_spi;

    /**
     * @brief SPI 3 wrapper enum of the STM32H723.
     *
     */
    static SPI::Peripheral spi1;
    static SPI::Peripheral spi2;
    static SPI::Peripheral spi3;
    static SPI::Peripheral spi4;
    static SPI::Peripheral spi5;
    static SPI::Peripheral spi6;

    /**
     * @brief SPI 3 instance of the STM32H723.
     *
     */
    static SPI::Instance instance1;
	static SPI::Instance instance2;
	static SPI::Instance instance3;
	static SPI::Instance instance4;
	static SPI::Instance instance5;
	static SPI::Instance instance6;

    /**
     * @brief Registers a new SPI.
     * 
     * @param spi SPI peripheral to register.
     * @return uint8_t Id of the service.
     */
    static optional<uint8_t> inscribe(SPI::Peripheral& spi);

    /**
     * @brief Method that initializes all enrolled SPI peripherals
     *        service. The peripherals that you want to use
     *  	  must be enrolled before initializing.
     */
    static void start();

    /**@brief	Transmits 1 SPIPacket of any size by polling.
     * 			Handles the packet size automatically.
     * 
     * @param id Id of the SPI
     * @param packet Pakcet to be send
     * @return bool Returns true if the request to send the packet has been done
     *            successfully. Returns false if a problem
     *            has occurred.
     */
    static bool transmit_next_packet(uint8_t id, RawPacket& packet);

    /**						
     * @brief This method request the receive of a new SPIPacket of any size
     *        by polling.
     * 
     * @param id Id of the SPI
     * @param packet SPIPacket in which the data will be stored
     * @return bool Return true if the order to receive a new packet has been
     *            processed correctly. Return false if a problem has occurred.
     */
    static bool receive_next_packet(uint8_t id, RawPacket& packet);


private:
    /**
     * @brief This method initializes the SPI peripheral that is passed to it as a parameter.
     * 
     * @param spi Peripheral instance to be initialized.
     */
    static void init(SPI::Instance* spi);
};

#endif
