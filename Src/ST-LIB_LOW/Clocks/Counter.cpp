/*
 * Time.hpp
 *
 *  Created on: 11 nov. 2022
 *      Author: Dani
 */

#pragma once

#include <ST-LIB_LOW/Clocks/Counter.hpp>
#include <Time/Time.hpp>

void Counter::update(){
	this->freq = (this->counter + 0.0)/this->UDPATE_PERIOD;
	this->counter = 0;
}

Counter::Counter(){
	this->time_id = Time::register_low_precision_alarm(this->UDPATE_PERIOD,[&](){ this->update(); });
}

Counter::~Counter(){
	Time::unregister_low_precision_alarm(this->time_id);
}

void Counter::count(){
	this->counter++;
}
