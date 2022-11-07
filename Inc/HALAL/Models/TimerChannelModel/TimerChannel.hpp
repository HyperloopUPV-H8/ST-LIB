/*
 * TimerChannel.hpp
 *
 *  Created on: Nov 1, 2022
 *      Author: aleja
 */

#pragma once

struct TimerChannel {
	TIM_HandleTypeDef* timer;
	uint32_t channel;

	bool operator== (const TimerChannel &other) const {
		return (timer == other.timer && channel == other.channel);
	}

	bool operator< (const TimerChannel &other) const {
		if (timer == other.timer)
			return channel < other.channel;
		return timer < other.timer;
	}
};

struct TimerChannelRisingFalling {
	TIM_HandleTypeDef* timer;
	uint32_t channel_rising;
	uint32_t channel_falling;

	bool operator== (const TimerChannelRisingFalling &other) const {
		return (timer == other.timer && channel_rising == other.channel_rising && channel_falling == other.channel_falling);
	}

	bool operator< (const TimerChannelRisingFalling &other) const {
		if (timer == other.timer) {
			if(channel_rising == other.channel_rising) {
				return channel_falling < other.channel_falling;
			}
		}
			return channel_rising < other.channel_rising;
		return timer < other.timer;
	}
};

struct ChannelsRisingFalling {
	uint32_t rising;
	uint32_t falling;
};
