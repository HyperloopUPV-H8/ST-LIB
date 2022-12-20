/*
 * UART.hpp
 *
 *  Created on: 12 dec. 2022
 *      Author: Pablo
 */
#pragma once

#include "ST-LIB.hpp"
#include "C++Utilities/CppUtils.hpp"
#include "Packets/I2CPacket.hpp"

#define HALAL_I2C_EMPTY_ID (uint16_t)0

#ifdef HAL_I2C_MODULE_ENABLED

/**
 * @brief UART service class. Abstracts the use of the UART service of the HAL library.
 * 
 */
class I2C{
private:
    /**
     * @brief Struct which defines all data referring to UART peripherals. It is
     *        declared private in order to prevent unwanted use. Only 
     *        predefined instances should be used.
     *           
     */
    struct Instance{
        Pin SCL; /**< Clock pin. */
        Pin SDA; /**< MOSI pin. */
        I2C_HandleTypeDef* hi2c;  /**< HAL UART struct. */
        I2C_TypeDef* instance;
        uint32_t speed_frequency = 100; /**< frequency in khz*/
        uint32_t data_length = 8; /**< size in bits*/
        bool receive_ready = false; /**< Receive value is ready to use pin. */
        bool initialized = false;

    };

public:
    /**
      * @brief Enum which abstracts the use of the Instance struct to facilitate the mocking of the HALAL.Struct
      *
      */
     enum Peripheral{
         peripheral2 = 2,
     };

public:
    static uint16_t id_counter;

    static unordered_map<uint8_t, I2C::Instance* > active_i2c;

    static unordered_map<I2C::Peripheral, I2C::Instance*> available_i2cs;

    /**
     * @brief UART 3 wrapper enum of the STM32H723.
     *
     */
    static I2C::Peripheral i2c2;

    /**
     * @brief UART 3 instance of the STM32H723.
     *
     */
    static I2C::Instance instance2;

    /**
     * @brief Registers a new UART.
     * 
     * @param uart UART peripheral to register.
     * @return uint8_t Id of the service.
     */
    static optional<uint8_t> inscribe(I2C::Peripheral& i2c);

    /**
     * @brief This method initializes all registered UARTs. The peripherals
     * 		  must be enrolled before calling this method.
     * 
     */
    static void start();

    /**@brief	Transmits 1 RawPacket of any size by DMA and
     *          interrupts. Handles the packet size automatically. To
     *          to send various packets in a row you must check if the UART is busy
     *          using is_busy().
     * 
     * @param id Id of the UART
     * @param packet Packet to be send
     * @return bool Returns true if the request to send the packet has been done
     *            successfully. Returns false if the UART is busy or a problem
     *            has occurred.
     */
    static bool transmit_next_packet(uint8_t id, I2CPacket& packet);

    /**						
     * @brief This method request the receive of a new RawPacket of any size
     *        by DMA and interrupts. Thus the packet should not be used until
     *        you have checked that the value is already available using the 
     *        method has_next_paclet(). All calls to this functions previous
     *        the last packet is ready will be ignored.
     *
     * @see   UART::has_next_packet()
     * 
     * @param id Id of the UART
     * @param packet RawPacket in which the data will be stored
     * @return bool Return true if the order to receive a new packet has been
     *            processed correctly. Return false if the UART is busy or a
     *            problem has occurred.
     */
    static bool receive_next_packet(uint8_t id, I2CPacket& packet);

    static bool read_from(uint8_t id, I2CPacket& packet, uint16_t mem_addr, uint16_t mem_size);

    static bool write_to(uint8_t id, I2CPacket& packet, uint16_t mem_addr, uint16_t mem_size);

    /**
     * @brief This method is used to check if the UART receive operation has finished and data is ready.
     * 
     * @param id Id of the UART
     * @return bool Return true if the packet is ready to use and false if not.
     */
    static bool has_next_packet(uint8_t id);

    /**
     * @brief This method is used to check if the UART transmit operations busy.
     * 
     * @param id Id of the UART
     * @return bool Return true if the UART transmit operation is busy and false if not.
     */
    static bool is_busy(uint8_t id);

    private:
    /**
     * @brief This method initializes the UART peripheral that is passed to it as a parameter.
     * 
     * @param uart Peripheral instance to be initialized.
     */
    static void init(I2C::Instance* uart);
};

#endif
