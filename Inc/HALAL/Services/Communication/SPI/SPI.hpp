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
public:

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
       
        bool receive_ready = false; /**< Receive value is ready to use pin. */
        bool initialized = false; /**< Peripheral has already been initialized */
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

    static unordered_map<SPI::Peripheral, SPI::Instance*> available_spi;

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
    static optional<uint8_t> inscribe(SPI::Peripheral& spi);

    /**
     * @brief Metodo que incializa todos los perifericos SPI inscritos
     *        al servicio. Los perifericos que se quieran usar
     *        deben estar inscritos antes de inicializarlos.
     * 
     */
    static void start();

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

private:
    /**
     * @brief This method initializes the SPI peripheral that is passed to it as a parameter.
     * 
     * @param spi Peripheral instance to be initialized.
     */
    static void init(SPI::Instance* spi);
};

#endif
