/*
 * ErrorHandler.cpp
 *
 *  Created on: Dec 22, 2022
 *      Author: Pablo
 */

#include "ErrorHandler/ErrorHandler.hpp"

string ErrorHandlerModel::description = "Error-No-Description-Found";
string ErrorHandlerModel::line = "Error-No-Line-Found";
string ErrorHandlerModel::func = "Error-No-Func-Found";
string ErrorHandlerModel::file = "Error-No-File-Found";
double ErrorHandlerModel::error_triggered = 0;

void ErrorHandlerModel::SetMetaData(int line, const char * func, const char * file){
		ErrorHandlerModel::line = to_string(line);
		ErrorHandlerModel::func = string(func);
		ErrorHandlerModel::file = string(file);
}

void ErrorHandlerModel::ErrorHandlerUpdate(){

#ifdef HAL_UART_MODULE_ENABLED
	if (!UART::printf_ready) {
		return;
	}

	printf("Error: %s%s", ErrorHandlerModel::description.c_str(), endl);
#endif

#ifdef HAL_ETH_MODULE_ENABLED
	//TODO:Enviar el mismo mensaje por ethernet.
#endif

#ifdef LED_FAULT
	HAL_GPIO_TogglePin(LED_FAULT.gpio_pin, LED_FAULT.port);
#endif
}

