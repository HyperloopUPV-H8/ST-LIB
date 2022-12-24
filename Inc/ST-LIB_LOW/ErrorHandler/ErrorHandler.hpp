/*
 * ErrorHandler.hpp
 *
 *  Created on: Dec 22, 2022
 *      Author: predawnia
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

	template<Integral... Args>
	static void ErrorHandler(string format, Args... args);

	static void ErrorHandlerUpdate();

};
