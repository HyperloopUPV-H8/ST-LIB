/*
 * ErrorHandler.hpp
 *
 *  Created on: Dec 22, 2022
 *      Author: Pablo
 */

#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "Time/Time.hpp"
#include "Communication/UART/UART.hpp"


class ErrorHandlerModel{
private:
	static string description;

public:
	static double error_triggered;

public:

	template<class... Args>
	static void ErrorHandler(string format, Args... args);

	static void ErrorHandlerUpdate();

};


template<class... Args>
void ErrorHandlerModel::ErrorHandler(string format, Args... args){
	 ErrorHandlerModel::error_triggered = 1.0;

	 const int32_t size = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
	 const unique_ptr<char[]> buffer = std::make_unique<char[]>(size);

	 std::snprintf(buffer.get(), size, format.c_str(), args...);

	 description += std::string(buffer.get(), buffer.get() + size - 1);
	 description += " | TimeStamp(nanoseconds): " + std::to_string(Time::get_global_tick());
}

