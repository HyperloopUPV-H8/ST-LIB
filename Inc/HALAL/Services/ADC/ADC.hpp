/*
 * ADC.hpp
 *
 *  Created on: 20 oct. 2022
 *      Author: alejandro
 */

#ifndef HALAL_SERVICES_ADC_HPP_
#define HALAL_SERVICES_ADC_HPP_

#include "../Inc/HALAL/Models/PinModel/Pin.hpp"
#include "../Inc/C++Utilities/CppUtils.hpp"


struct ADCchannel {
	ADC_HandleTypeDef* adc;
	uint32_t* buffer;
	uint8_t buflen;
	uint8_t bufpos;
};

class ADC {
public:
	static map<uint8_t, Pin> serviceIDs;

	struct KeyHash {
		size_t operator()(const Pin& k) const {
			return std::hash<std::uint8_t>()(k.pin) ^
			(std::hash<std::uint8_t>()(*reinterpret_cast<uint64_t*>(k.port)) << 1);
		}
	};

    struct KeyEqual {
		bool operator()(const Pin& lhs, const Pin& rhs) const {
			return lhs.pin == rhs.pin && lhs.port == rhs.port;
		}
	};

	static unordered_map<Pin, ADCchannel, KeyHash, KeyEqual> pinTimerMap;
	static forward_list<uint8_t> IDmanager;

	static optional<uint8_t> register_adc(Pin pin);
	static void turn_on_adc(uint8_t id);
	static void turn_off_adc(uint8_t id);
	static optional<uint16_t> get_pin_value(Pin pin);

};



#endif /* HALAL_SERVICES_ADC_HPP_ */
