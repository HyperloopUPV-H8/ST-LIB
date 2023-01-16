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

	 /**
	 * @brief Triggers ErrorHandler and format the error message. The format works
	 * 	      exactly like printf format.
	 *
	 * @param format String which will be formated.
	 * @param args   Arguments specifying data to print
	 * @return uint8_t Id of the service.
	 */
	template<class... Args>
	static void ErrorHandler(string format, Args... args);

	/**
	 * @brief Transmit the error message.
	 */
	static void ErrorHandlerUpdate();

};


template<class... Args>
void ErrorHandlerModel::ErrorHandler(string format, Args... args){
	 ErrorHandlerModel::error_triggered = 1.0;

	 const int32_t size = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
	 const unique_ptr<char[]> buffer = std::make_unique<char[]>(size);

	 std::snprintf(buffer.get(), size, format.c_str(), args...);

	 description += std::string(buffer.get(), buffer.get() + size - 1);
	 description += " | TimeStamp: " + std::to_string(Time::get_global_tick());
}

