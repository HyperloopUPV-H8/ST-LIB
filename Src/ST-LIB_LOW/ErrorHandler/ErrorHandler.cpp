/*
 * ErrorHandler.cpp
 *
 *  Created on: Dec 22, 2022
 *      Author: Pablo
 */

#include "ErrorHandler/ErrorHandler.hpp"

string ErrorHandlerModel::description = "";
double ErrorHandlerModel::error_triggered = 0;

void ErrorHandlerModel::ErrorHandlerUpdate(){
	if (!UART::printf_ready) {
		return;
	}

	printf("Error: %s%s", ErrorHandlerModel::description.c_str(), endl);

	//TODO:Enviar el mismo mensaje por ethernet.

	//TODO: Parpadeo del led de fault.
	//HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
}

