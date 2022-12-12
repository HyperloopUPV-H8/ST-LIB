/*
 * UART.hpp
 *
 *  Created on: 28 nov. 2022
 *      Author: Pablo
 */
#pragma once

#include "ST-LIB.hpp"
#include "C++Utilities/CppUtils.hpp"
#include "Packets/RawPacket.hpp"

#ifdef HAL_UART_MODULE_ENABLED

extern UART_HandleTypeDef huart3;

#define TXBUSYMASK 0b1
#define RXBUSYMASK 0b10
/**
 * @brief UART service class. Abstracts the use of the UART service of the HAL library.
 * 
 */
class UART{
private:
    /**
     * @brief Struct which defines all data referring to UART peripherals. It is
     *        declared private in order to prevent unwanted use. Only 
     *        predefined instances should be used.
     *           
     */
    struct Instance{
        Pin TX; /**< Clock pin. */
        Pin RX; /**< MOSI pin. */
        UART_HandleTypeDef* huart;  /**< HAL UART struct. */
        USART_TypeDef* instance;
        uint32_t baud_rate;
        uint32_t word_length;
        bool receive_ready = false; /**< Receive value is ready to use pin. */
        bool initialized = false;

    };

    /**
     * @brief Enum which abstracts the use of the Instance struct to facilitate the mocking of the HALAL.Struct
     *
     */
    enum Peripheral{
        peripheral3 = 2,
    };

public:
    static uint16_t id_counter;
    
    static unordered_map<uint8_t, UART::Instance* > registered_uart;

    static unordered_map<UART::Peripheral, UART::Instance*> available_uarts;

    /**
     * @brief UART 3 wrapper enum of the STM32H723.
     *
     */
    static UART::Peripheral uart3;

    /**
     * @brief UART 3 instance of the STM32H723.
     *
     */
    static UART::Instance instance3;

    /**
     * @brief Registers a new UART.
     * 
     * @param uart UART peripheral to register.
     * @return uint8_t Id of the service.
     */
    static optional<uint8_t> inscribe(UART::Peripheral& uart);

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
    static bool transmit_next_packet(uint8_t id, RawPacket& packet);

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
    static bool receive_next_packet(uint8_t id, RawPacket& packet);

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
    static void init(UART::Instance* uart);
};

#endif
