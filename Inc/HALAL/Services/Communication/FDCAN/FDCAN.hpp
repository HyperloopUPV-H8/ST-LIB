/*
 *  FDCAN.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */

#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "ErrorHandler/ErrorHandler.hpp"
#include "stm32h7xx_hal.h"

#ifdef HAL_FDCAN_MODULE_ENABLED

using std::unordered_map;
using std::vector;
using std::queue;



class FDCAN{
public:
	enum DLC{
		bytes_0 = FDCAN_DLC_BYTES_0,
		bytes_1 = FDCAN_DLC_BYTES_1,
		bytes_2 = FDCAN_DLC_BYTES_2,
		bytes_3 = FDCAN_DLC_BYTES_3,
		bytes_4 = FDCAN_DLC_BYTES_4,
		bytes_5 = FDCAN_DLC_BYTES_5,
		bytes_6 = FDCAN_DLC_BYTES_6,
		bytes_7 = FDCAN_DLC_BYTES_7,
		bytes_8 = FDCAN_DLC_BYTES_8,
		bytes_12 = FDCAN_DLC_BYTES_12,
		bytes_16 = FDCAN_DLC_BYTES_16,
		bytes_20 = FDCAN_DLC_BYTES_20,
		bytes_24 = FDCAN_DLC_BYTES_24,
		bytes_32 = FDCAN_DLC_BYTES_32,
		bytes_48 = FDCAN_DLC_BYTES_48,
		bytes_64 = FDCAN_DLC_BYTES_64,
	};

	struct Packet{
		vector<uint8_t> rx_data;
		uint32_t identifier;
		DLC data_length;

	};

private:
    /**
     * @brief Struct which defines all data referring to FDCAN peripherals. It is
     *        declared private in order to prevent unwanted use. Only
     *        predefined instances should be used.
     *
     */
    struct Instance{
        Pin TX;
        Pin RX;
        FDCAN_HandleTypeDef* hfdcan;
        FDCAN_GlobalTypeDef* instance;
        DLC dlc;
        FDCAN_TxHeaderTypeDef tx_header;
        uint32_t rx_location;
        queue<FDCAN::Packet> rx_queue;
        vector<uint8_t> tx_data;
        uint8_t fdcan_number;
        bool start = false;

    };

    /**
	 * @brief Enum which abstracts the use of the Instance struct to facilitate the mocking of the HALAL.Struct
	 *
	 */
	enum Peripheral{
		peripheral1 = 0,
		peripheral2 = 1,
		peripheral3 = 2,
	};

public:
    static uint16_t id_counter;

    static unordered_map<uint8_t, FDCAN::Instance*> registered_fdcan;
    static unordered_map<FDCAN::Peripheral, FDCAN::Instance*> available_fdcans;
    static unordered_map<FDCAN_HandleTypeDef*, FDCAN::Instance*> handle_to_fdcan;
    static unordered_map<FDCAN::DLC, uint8_t> dlc_to_len;
    /**
	* @brief FDCAN  wrapper enum of the STM32H723.
	*
	*/
    static FDCAN::Peripheral fdcan1;

    /**
     * @brief FDCAN instances of the STM32H723.
     *
     */
    static FDCAN::Instance instance1;

    static optional<uint8_t> inscribe(FDCAN::Peripheral& fdcan);

    static void start();

    static bool transmit(uint8_t id, uint32_t message_id, span<uint8_t> data);

    static bool read(uint8_t id, FDCAN::Packet* data);

    static bool wait_and_read(uint8_t id, FDCAN::Packet* data);

    /**
	 * @brief This method is used to check if the FDCAN have received any new packet.
	 *
	 * @param id Id of the FDCAN
	 * @return bool Return true if the data queue has any packet.
	 */
	static bool data_test(uint8_t id);

private:

    static void init(FDCAN::Instance* fdcan);



};

#endif
