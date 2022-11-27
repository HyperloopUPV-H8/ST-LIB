/*
 * Time.hpp
 *
 *  Created on: 11 nov. 2022
 *      Author: Dani
 */

#include <ST-LIB_LOW/Clocks/Stopwatch.hpp>
#include "HALAL/Services/Time/Time.hpp"


void Stopwatch::start(const string id){
	start_times[id] = Time::get_global_tick();
}

optional<uint64_t> Stopwatch::stop(const string id){
	if(start_times.contains(id)){
		uint64_t result = Time::get_global_tick() - start_times[id];
		start(id);
		return result;
	}
	return nullopt;
}
