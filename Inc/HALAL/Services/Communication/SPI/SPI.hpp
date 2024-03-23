/*
 * SPI.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */

#pragma once

#include "PinModel/Pin.hpp"
#include "Packets/SPIPacket.hpp"
#include "DigitalOutputService/DigitalOutputService.hpp"
#include "DMA/DMA.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

#ifdef HAL_SPI_MODULE_ENABLED

#define MASTER_SPI_CHECK_DELAY 100000 //how often the master should check if the slave is ready, in nanoseconds

//TODO: Hay que hacer el Chip select funcione a traves de un GPIO en vez de a traves del periferico.

/**
 * @brief SPI service class. Abstracts the use of the SPI service of the HAL library.
 * 
 */
class SPI{
public:
	/**
	 * @brief Possible states of the SPI class
	 */
    enum SPIstate{
    	IDLE = 0,
		STARTING_PACKET,
    	WAITING_PACKET,
		PROCESSING_PACKET,
    };

    /**
     * @brief Struct which defines all data referring to SPI peripherals. DO NOT MODIFY ON RUN TIME.
     * declared public so HAL callbacks can directly access the instance
     * NOT intended to be modified after the inscribe of the spi, configuration should be made in Runes.hpp
     *           
     */
    struct Instance{
        Pin* SCK; /**< Clock pin. */
        Pin* MOSI; /**< MOSI pin. */
        Pin* MISO; /**< MISO pin. */
        Pin* SS; /**< Slave select pin. */

        SPI_HandleTypeDef* hspi;  /**< HAL spi struct pin. */  
        SPI_TypeDef* instance; /**< HAL spi instance. */
        DMA::Stream hdma_tx; /**< HAL DMA handler for writting */
        DMA::Stream hdma_rx; /**< HAL DMA handler for reading */

        uint32_t baud_rate_prescaler; /**< SPI baud prescaler.*/
        uint32_t mode = SPI_MODE_MASTER; /**< SPI mode. */
        uint32_t data_size = SPI_DATASIZE_8BIT;  /**< SPI data size. Default 8 bit */
        uint32_t first_bit = SPI_FIRSTBIT_MSB; /**< SPI first bit,. */
        uint32_t clock_polarity = SPI_POLARITY_LOW; /**< SPI clock polarity. */
        uint32_t clock_phase = SPI_PHASE_1EDGE; /**< SPI clock phase. */
        uint32_t nss_polarity = SPI_NSS_POLARITY_LOW; /**< SPI chip select polarity. */
       
        bool initialized = false; /**< Peripheral has already been initialized */
        bool use_DMA = false;
        string name;
        SPIstate state = IDLE; /**< State of the spi on the packet communication*/
        uint16_t available_end = 0; /**< variable that checks for what packet id is the other end ready*/
        uint16_t SPIPacketID = 0; /**< SPIPacket being processed, if any*/
        uint64_t last_end_check = 0; /**< last clock cycle where the available end was checked*/
        uint64_t packet_count = 0; /**< packet completed counter for debugging*/
    };


    static void turn_on_chip_select(SPI::Instance* spi);
    static void turn_off_chip_select(SPI::Instance* spi);

    /**
     * @brief Enum that abstracts the use of the Instance struct to facilitate the mocking of the HALAL.
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

    static uint16_t id_counter;
    
    static map<uint8_t, SPI::Instance* > registered_spi;

    static unordered_map<SPI::Peripheral, SPI::Instance*> available_spi;

    /**
     * @brief map used in HAL callbacks to obtain the instance of the respective handler
     */
    static map<SPI_HandleTypeDef*, SPI::Instance*> registered_spi_by_handler;

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
    static uint8_t inscribe(SPI::Peripheral& spi);

    /**
     * @brief Method that initializes all enrolled SPI peripherals
     *        service. The peripherals that you want to use
     *  	  must be enrolled before initializing.
     */
    static void start();

    /**@brief	Transmits 1 byte by SPI.
     * 
     * @param id Id of the SPI
     * @param data Data to be send
     * @return bool Returns true if the data has been send successfully.
     * 			    Returns false if a problem has occurred.
     */
    static bool transmit(uint8_t id, uint8_t data);

    /**@brief	Transmits size bytes by SPI.
	 *
	 * @param id Id of the SPI
	 * @param data Data to be send
	 * @param size Size in bytes to be send
	 * @return bool Returns true if the data has been send successfully.
	 * 			    Returns false if a problem has occurred.
	 */
    static bool transmit(uint8_t id, span<uint8_t> data);

    /**@brief	Transmits size bytes by SPI via DMA
	 *
	 * @param id Id of the SPI
	 * @param data Data to be send
	 * @param size Size in bytes to be send
	 * @return bool Returns true if the data has been send successfully.
	 * 			    Returns false if a problem has occurred.
	 */
    static bool transmit_DMA(uint8_t id, span<uint8_t> data);
    /**						
     * @brief This method requests an array of data. The data
     * 		  will be stored in data parameter. You must make sure you have
     * 		  enough space
     * 
     * @param id Id of the SPI
     * @param data Pointer where data will be stored
     * @param size Size in bytes to receive.
     * @return bool Returns true if the data have been read successfully.
     * 			    Returns false if a problem has occurred.
     */
    static bool receive(uint8_t id, span<uint8_t> data);
    /**						
     * @brief This method requests an array of data. The data
     * 		  will be stored in data parameter. You must make sure you have
     * 		  enough space, this function uses DMA
     * 
     * @param id Id of the SPI
     * @param data Pointer where data will be stored
     * @param size Size in bytes to receive.
     * @return bool Returns true if the data have been read successfully.
     * 			    Returns false if a problem has occurred.
     */
    static bool receive_DMA(uint8_t id, span<uint8_t> data);

    /**
	 * @brief This method transmits one order of command_size bytes and
	 * 		  then stores the data received after that order.
	 *
	 * @param id Id of the SPI
	 * @param command_data Command
	 * @param command_size Command size in bytes to receive
	 * @param receive_data Pointer where data will be stored
	 * @param receive_size Number of bytes to read
	 * @return bool Returns true if the data have been read successfully.
	 * 			    Returns false if a problem has occurred.
	 */
    static bool transmit_and_receive(uint8_t id, span<uint8_t> command_data, span<uint8_t> receive_data);

    /**
	 * @brief This method transmits one order of command_size bytes and
	 * 		  then stores the data received after that order using DMA
	 *
	 * @param id Id of the SPI
	 * @param command_data Command
	 * @param command_size Command size in bytes to receive
	 * @param receive_data Pointer where data will be stored
	 * @param receive_size Number of bytes to read
	 * @return bool Returns true if the data have been read successfully.
	 * 			    Returns false if a problem has occurred.
	 */
    static bool transmit_and_receive_DMA(uint8_t id, span<uint8_t> command_data, span<uint8_t> receive_data);
    /**
     * @brief update that has to be called in order for master to check if the slave is ready to send the packet. If it is not called periodically, the master_transmit_packet will not work. Not needed for dummy communication (not using packets)
     */
    static void packet_update();

    /**
     * @brief master send packet method, which tries to send a single packet
     */
    static bool master_transmit_packet(uint8_t id, SPIPacket packet);

    /**
     * @brief slave listen packets method. When called, the slave will start to listen packets until the state is set again to IDLE
     */
    static void slave_listen_packets(uint8_t id);

    /**
     * @brief This method sets chip select to high level.
     *
     * @param spi Id of the SPI
     */
    static void chip_select_on(uint8_t id);

    /**
	 * @brief This method sets chip select to low level.
	 *
	 * @param spi Id of the SPI
	 */
    static void chip_select_off(uint8_t id);


private:
    /**
     * @brief This method initializes the SPI peripheral that is passed to it as a parameter.
     * 
     * @param spi Peripheral instance to be initialized.
     */
    static void init(SPI::Instance* spi);
};

#endif
