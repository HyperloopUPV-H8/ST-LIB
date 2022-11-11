/*
 * Time.hpp
 *
 *  Created on: 11 nov. 2022
 *      Author: Dani
 */

#pragma once

#include "C++Utilities/CppUtils.hpp"

class Counter{

private:
	static const uint16_t UDPATE_PERIOD = 125;
	uint8_t time_id;
	void update();

public:
	uint64_t counter = 0;
	double freq = 0.0;

	~Counter();
	Counter();
	void count();

};
