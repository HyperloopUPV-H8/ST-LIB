/*
 *  FDCAN.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */

#include "Communication/FDCAN/FDCAN.hpp"

#ifdef HAL_FDCAN_MODULE_ENABLED

uint16_t FDCAN::id_counter = 0;

unordered_map<uint8_t, FDCAN::Instance*> FDCAN::registered_fdcan = {};

unordered_map<FDCAN::DLC, uint8_t> FDCAN::dlc_to_len = {{DLC::BYTES_0, 0}, {DLC::BYTES_1, 1}, {DLC::BYTES_2, 2}, {DLC::BYTES_3, 3}, {DLC::BYTES_4, 4},
														{DLC::BYTES_5, 5}, {DLC::BYTES_6, 6}, {DLC::BYTES_7, 7}, {DLC::BYTES_8, 8}, {DLC::BYTES_12, 12},
														{DLC::BYTES_16, 16}, {DLC::BYTES_20, 20}, {DLC::BYTES_24, 24}, {DLC::BYTES_32, 32}, {DLC::BYTES_48, 48},
														{DLC::BYTES_64, 64}
													    };

optional<uint8_t> FDCAN::inscribe(FDCAN::Peripheral& fdcan){
	if (!FDCAN::available_fdcans.contains(fdcan)) {
		return nullopt;
	}

	FDCAN::Instance* fdcan_instance = FDCAN::available_fdcans[fdcan];

	Pin::inscribe(fdcan_instance->TX, ALTERNATIVE);
	Pin::inscribe(fdcan_instance->RX, ALTERNATIVE);

	uint8_t id = FDCAN::id_counter++;

	FDCAN::registered_fdcan[id] = fdcan_instance;

	return id;
}

void FDCAN::start(){
	for( std::pair<uint8_t, FDCAN::Instance*> inst: FDCAN::registered_fdcan){
		uint8_t id = inst.first;
		FDCAN::Instance* instance = inst.second;
		FDCAN::init(instance);

		FDCAN_TxHeaderTypeDef header;
		header.FDFormat = FDCAN_FD_CAN;
		header.DataLength = instance->dlc;
		header.TxFrameType = FDCAN_DATA_FRAME;
		header.BitRateSwitch = FDCAN_BRS_ON;
		header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
		header.IdType = FDCAN_STANDARD_ID;
		header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
		header.MessageMarker = 0;
		header.Identifier = 0x0;

		instance->tx_header = header;

		instance->rx_queue = queue<FDCAN::Packet>();
		instance->tx_data = vector<uint8_t>();

		if(HAL_FDCAN_Start(instance->hfdcan) != HAL_OK){
			ErrorHandler("Error during FDCAN %d initialization.", instance->fdcan_number);
		}

	    if(HAL_FDCAN_ActivateNotification(instance->hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK){
	    	ErrorHandler("Error activating FDCAN %d notifications.", instance->fdcan_number);
	    }

	    instance->start = true;

	    FDCAN::registered_fdcan[id] = instance;
	}
}

bool FDCAN::transmit(uint8_t id, uint32_t message_id, span<uint8_t> data, FDCAN::DLC dlc){
	if (not FDCAN::registered_fdcan.contains(id)) {
		ErrorHandler("There is no registered FDCAN with id: %d.", id);
		return false;
	}

	FDCAN::Instance* instance = registered_fdcan[id];

	if (not instance->start) {
		ErrorHandler("The FDCAN %d is not initialized.", instance->fdcan_number);
		return false;
	}

	instance->tx_header.Identifier = message_id;

	if (dlc != FDCAN::DLC::DEFAULT) {
		instance->tx_header.DataLength = dlc;
	}

	HAL_StatusTypeDef error = HAL_FDCAN_AddMessageToTxFifoQ(instance->hfdcan, &instance->tx_header, data.data());

	if (error != HAL_OK) {
		ErrorHandler("Error sending message with id: 0x%x by FDCAN %d", message_id, instance->fdcan_number);
		return false;
	}

	return true;
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs){
	if (not FDCAN::handle_to_fdcan.contains(hfdcan)) {
		printf("Warning: message received from an unknown FDCAN");
		return;
	}

	if (FDCAN::handle_to_fdcan[hfdcan]->rx_queue.size() >= FDCAN::handle_to_fdcan[hfdcan]->rx_queue_max_size) {
		return; // TODO: WARNING RX_QUEUE FULL
	}

	FDCAN::DLC dlc = FDCAN::handle_to_fdcan[hfdcan]->dlc;

	vector<uint8_t> data_buffer(FDCAN::dlc_to_len[dlc]);

	FDCAN_RxHeaderTypeDef header_buffer = FDCAN_RxHeaderTypeDef();
	HAL_FDCAN_GetRxMessage(hfdcan, FDCAN::handle_to_fdcan[hfdcan]->rx_location, &header_buffer, data_buffer.data());


	FDCAN::Packet packet_buffer = {data_buffer, header_buffer.Identifier, (FDCAN::DLC)header_buffer.DataLength};
	FDCAN::handle_to_fdcan[hfdcan]->rx_queue.push(packet_buffer);
}

bool FDCAN::read(uint8_t id, FDCAN::Packet* data){
	if (not FDCAN::registered_fdcan.contains(id)) {
		ErrorHandler("There is no FDCAN registered with id: %d.", id);
		return false;
	}

	if (FDCAN::registered_fdcan[id]->rx_queue.empty()) {
		data->rx_data.clear();
		data->data_length = FDCAN::DLC::BYTES_0;
		data->identifier = 0;
		return false;
	}

	FDCAN::Packet packet = FDCAN::registered_fdcan[id]->rx_queue.front();

	data->rx_data = packet.rx_data;
	data->identifier = packet.identifier;
	data->data_length = packet.data_length;

	FDCAN::registered_fdcan[id]->rx_queue.pop();

	return true;
}

bool FDCAN::wait_and_read(uint8_t id, FDCAN::Packet* data){
	if (not FDCAN::registered_fdcan.contains(id)) {
		ErrorHandler("There is no FDCAN registered with id: %d.", id);
		return false;
	}

	//Wait until the arrival of a message.
	while (FDCAN::registered_fdcan[id]->rx_queue.empty()) {
	}

	FDCAN::Packet packet = FDCAN::registered_fdcan[id]->rx_queue.front();

	data->rx_data = packet.rx_data;
	data->identifier = packet.identifier;
	data->data_length = packet.data_length;

	FDCAN::registered_fdcan[id]->rx_queue.pop();

	return true;
}

bool FDCAN::received_test(uint8_t id){
	if (not FDCAN::registered_fdcan.contains(id)) {
		ErrorHandler("FDCAN with id %u not registered", id);
		return false;
	}

	if (FDCAN::registered_fdcan[id]->rx_queue.empty()) {
		return false;
	}

	return true;
}

void FDCAN::init(FDCAN::Instance* fdcan){
	FDCAN_HandleTypeDef* handle = fdcan->hfdcan;

	handle->Instance = fdcan->instance;
	handle->Init.FrameFormat = FDCAN_FRAME_FD_BRS;
	handle->Init.Mode = FDCAN_MODE_NORMAL;
	handle->Init.AutoRetransmission = ENABLE;
	handle->Init.TransmitPause = DISABLE;
	handle->Init.ProtocolException = DISABLE;
	handle->Init.NominalPrescaler = 1;
	handle->Init.NominalSyncJumpWidth = 16;
	handle->Init.NominalTimeSeg1 = 59;
	handle->Init.NominalTimeSeg2 = 20;
	handle->Init.DataPrescaler = 1;
	handle->Init.DataSyncJumpWidth = 4;
	handle->Init.DataTimeSeg1 = 14;
	handle->Init.DataTimeSeg2 = 5;
	handle->Init.MessageRAMOffset = 0;
	handle->Init.StdFiltersNbr = 1;
	handle->Init.ExtFiltersNbr = 0;
	handle->Init.RxFifo0ElmtsNbr = 16;
	handle->Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_64;
	handle->Init.RxFifo1ElmtsNbr = 0;
	handle->Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_64;
	handle->Init.RxBuffersNbr = 0;
	handle->Init.RxBufferSize = FDCAN_DATA_BYTES_64;
	handle->Init.TxEventsNbr = 0;
	handle->Init.TxBuffersNbr = 0;
	handle->Init.TxFifoQueueElmtsNbr = 16;
	handle->Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	handle->Init.TxElmtSize = FDCAN_DATA_BYTES_64;

	if (HAL_FDCAN_Init(handle) != HAL_OK)
	{
		ErrorHandler("Error during FDCAN %d init.", fdcan->fdcan_number);
	}

}
#endif

