/*
 * ErrorHandler.cpp
 *
 *  Created on: Dec 22, 2022
 *      Author: predawnia
 */

#include "ErrorHandler/ErrorHandler.hpp"

double ErrorHandlerModel::error_triggered = 0;

template<Integral... Args>
void ErrorHandlerModel::ErrorHandler(string format, Args... args){
	 ErrorHandlerModel::error_triggered = 1.0;

	//TODO: set up uart prinf

	 snprintf(description, 500 , format.c_str(), args...);

	 description += " | TimeStamp: " + Time::get_global_tick();

}


void ErrorHandlerModel::ErrorHandlerUpdate(){

	printf("Error: %s%s", ErrorHandlerModel::description.c_str(), endl);

	// Parpadeo del led de fault.
	//HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
}

