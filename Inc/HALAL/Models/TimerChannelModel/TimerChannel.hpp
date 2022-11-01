/*
 * TimerChannel.hpp
 *
 *  Created on: Nov 1, 2022
 *      Author: aleja
 */

#pragma once

struct TimerChannel {
	TIM_HandleTypeDef* timer;
	unsigned int channel;

	bool operator== (const TimerChannel &other) const {
		return (timer == other.timer && channel == other.channel);
	}

	bool operator< (const TimerChannel &other) const {
		if (timer == other.timer)
			return channel < other.channel;
		return timer < other.timer;
	}
};
