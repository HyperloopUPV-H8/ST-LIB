/*
 * ErrorHandler.cpp
 *
 *  Created on: Dec 22, 2022
 *      Author: Pablo
 */

#include "ErrorHandler/ErrorHandler.hpp"

string ErrorHandlerModel::description = "";
string ErrorHandlerModel::line = "Error-No-Line";
string ErrorHandlerModel::func = "Error-No-Line";
string ErrorHandlerModel::file = "Error-No-File";
double ErrorHandlerModel::error_triggered = 0;

void ErrorHandlerModel::GetMetaData(int line, const char * func, const char * file){
		ErrorHandlerModel::line = to_string(line);
		ErrorHandlerModel::func = std::string(func);
		ErrorHandlerModel::file = std::string(file);
}

void ErrorHandlerModel::ErrorHandlerUpdate(){
	if (!UART::printf_ready) {
		return;
	}

	printf("Error: %s%s", ErrorHandlerModel::description.c_str(), endl);

	//TODO:Enviar el mismo mensaje por ethernet.

	//TODO: Parpadeo del led de fault.
	//HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
}

