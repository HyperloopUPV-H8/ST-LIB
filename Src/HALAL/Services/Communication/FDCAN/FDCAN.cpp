/*
 *  FDCAN.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */
#include "Communication/FDCAN/FDCAN.hpp"

#ifdef HAL_FDCAN_MODULE_ENABLED

unordered_map<uint8_t, FDCAN::Instance*> FDCAN::registered_fdcan = {};

unordered_map<FDCAN::DLC, uint8_t> FDCAN::dlc_to_len = {{DLC::bytes_0, 0}, {DLC::bytes_1, 1}, {DLC::bytes_2, 2}, {DLC::bytes_3, 3}, {DLC::bytes_4, 4},
														{DLC::bytes_5, 5}, {DLC::bytes_6, 6}, {DLC::bytes_7, 7}, {DLC::bytes_8, 8}, {DLC::bytes_12, 12},
														{DLC::bytes_16, 16}, {DLC::bytes_20, 20}, {DLC::bytes_24, 24}, {DLC::bytes_32, 32}, {DLC::bytes_48, 48},
														{DLC::bytes_64, 64}
													    };

optional<uint8_t> FDCAN::inscribe(FDCAN::Peripheral& fdcan){
	if (!FDCAN::available_fdcans.contains(fdcan)) {
		ErrorHandler("El fdcan %d no es un fdcan válido.", fdcan);
		return nullopt;
	}

	FDCAN::Instance* fdcan_instance = FDCAN::available_fdcans[fdcan];

	Pin::inscribe(fdcan_instance->TX, ALTERNATIVE);
	Pin::inscribe(fdcan_instance->RX, ALTERNATIVE);

	uint8_t id = FDCAN::id_counter++;
	fdcan_instance->fdcan_number = fdcan;

	FDCAN::registered_fdcan[id] = fdcan_instance;

	return id;
}

void FDCAN::start(){
	for(auto iter: FDCAN::registered_fdcan){
		FDCAN::init(iter.second);

		FDCAN_TxHeaderTypeDef header = FDCAN_TxHeaderTypeDef();
		header.FDFormat = FDCAN_FD_CAN;
		header.DataLength = iter.second->dlc;
		header.TxFrameType = FDCAN_DATA_FRAME;
		header.BitRateSwitch = FDCAN_BRS_ON;
		header.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
		header.FDFormat = FDCAN_FRAME_FD_BRS;
		header.IdType = FDCAN_STANDARD_ID;
		header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
		header.MessageMarker = 0;
		header.Identifier = 0;

		iter.second->tx_header = header;

		iter.second->rx_queue = queue<FDCAN::Packet>();
		iter.second->tx_data = vector<uint8_t>();

		if(HAL_FDCAN_Start(iter.second->hfdcan) != HAL_OK){
			ErrorHandler("Error al iniciar el FDCAN %d.", iter.second->fdcan_number);
		}

	    if(HAL_FDCAN_ActivateNotification(iter.second->hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK){
	    	ErrorHandler("Error al activar las notificaciones del FDCAN %d.", iter.second->fdcan_number);
	    }

	    iter.second->start = true;
	}
}

bool FDCAN::transmit(uint8_t id, uint32_t message_id, span<uint8_t> data){
	if (not FDCAN::registered_fdcan.contains(id)) {
		ErrorHandler("No existe ningun fdcan registrado con id: %d.", id);
		return false;
	}

	if (not FDCAN::registered_fdcan[id]->start) {
		ErrorHandler("El FDCAN %d no esta inicializado.", FDCAN::registered_fdcan[id]->fdcan_number);
		return false;
	}

	FDCAN::registered_fdcan[id]->tx_header.Identifier = message_id;

	HAL_StatusTypeDef error = HAL_FDCAN_AddMessageToTxFifoQ(FDCAN::registered_fdcan[id]->hfdcan, &FDCAN::registered_fdcan[id]->tx_header, data.data());

	if (error != HAL_OK) {
		ErrorHandler("Error al enviar el mensage con id: %d por el FDCAN %d", message_id, FDCAN::registered_fdcan[id]->fdcan_number);
		return false;
	}

	return true;
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs){
	if (not FDCAN::handle_to_fdcan.contains(hfdcan)) {
		printf("Warning: message received from an unknown FDCAN");
		return;
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
		ErrorHandler("No existe ningun fdcan registrado con id: %d.", id);
		return false;
	}

	if (FDCAN::registered_fdcan[id]->rx_queue.empty()) {
		data->rx_data.clear();
		data->data_length = FDCAN::DLC::bytes_0;
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
		ErrorHandler("No existe ningun fdcan registrado con id: %d.", id);
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

bool FDCAN::data_test(uint8_t id){
	if (not FDCAN::registered_fdcan.contains(id)) {
		ErrorHandler("No existe ningun fdcan registrado con id: %d.", id);
		return false;
	}

	if (FDCAN::registered_fdcan[id]->rx_queue.empty()) {
		return false;
	}

	return true;
}

void FDCAN::init(FDCAN::Instance* fdcan){
	FDCAN_HandleTypeDef* handle = fdcan->hfdcan;

	handle->Init.FrameFormat = FDCAN_FRAME_FD_BRS;
	handle->Init.Mode = FDCAN_MODE_NORMAL;
	handle->Init.AutoRetransmission = ENABLE;
	handle->Init.TransmitPause = DISABLE;
	handle->Init.ProtocolException = DISABLE;
	handle->Init.NominalPrescaler = 16;
	handle->Init.NominalSyncJumpWidth = 0x10;
	handle->Init.NominalTimeSeg1 = 0x3F;
	handle->Init.NominalTimeSeg2 = 0x10;
	handle->Init.DataPrescaler = 1;
	handle->Init.DataSyncJumpWidth = 0x4;
	handle->Init.DataTimeSeg1 = 0xF;
	handle->Init.DataTimeSeg2 = 0x4;
	handle->Init.MessageRAMOffset = 0;
	handle->Init.StdFiltersNbr = 1;
	handle->Init.ExtFiltersNbr = 0;
	handle->Init.RxFifo0ElmtsNbr = 0;
	handle->Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
	handle->Init.RxFifo1ElmtsNbr = 0;
	handle->Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
	handle->Init.RxBuffersNbr = 0;
	handle->Init.RxBufferSize = FDCAN_DATA_BYTES_8;
	handle->Init.TxEventsNbr = 0;
	handle->Init.TxBuffersNbr = 0;
	handle->Init.TxFifoQueueElmtsNbr = 0;
	handle->Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	handle->Init.TxElmtSize = FDCAN_DATA_BYTES_8;
	if (HAL_FDCAN_Init(handle) != HAL_OK)
	{
		ErrorHandler("Error al inciar el FDCAN %d.", fdcan->fdcan_number);
	}

}
#endif

