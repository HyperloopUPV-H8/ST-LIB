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
	static string line;
	static string func;
	static string file;

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
	static void ErrorHandler( string format, Args... args);

	 /**
	 * @brief Get all metadata needed for the error message, including the line function and file.
	 *        The default parameters are not necessary but are there in case the compiler macros stop
	 *        working because a change of the compiler.
	 *
	 * @param line Line where the error occurred
	 * @param func Function where the error occurred
	 * @param file File where the file occurred
	 * @return uint8_t Id of the service.
	 */
	static void GetMetaData( int line = __builtin_LINE(), const  char * func = __builtin_FUNCTION(), const char * file = __builtin_FILE());

	/**
	 * @brief Transmit the error message.
	 */
	static void ErrorHandlerUpdate();

};

template<class... Args>
void ErrorHandlerModel::ErrorHandler( string format, Args... args){
	 ErrorHandlerModel::error_triggered = 1.0;

	 const int32_t size = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
	 const unique_ptr<char[]> buffer = std::make_unique<char[]>(size);

	 std::snprintf(buffer.get(), size, format.c_str(), args...);

	 description += std::string(buffer.get(), buffer.get() + size - 1) + " | Line: " + ErrorHandlerModel::line
					            + " Function: \"" + ErrorHandlerModel::func + "\" File: " + ErrorHandlerModel::file ;

#ifdef HAL_TIM_MODULE_ENABLED
	 description += " | TimeStamp: " + std::to_string(Time::get_global_tick());
#endif
}

#define ErrorHandler(x, args...) do { ErrorHandlerModel::GetMetaData(__LINE__, __FUNCTION__, __FILE__); \
					                  ErrorHandlerModel::ErrorHandler(x, args);}while(0)

