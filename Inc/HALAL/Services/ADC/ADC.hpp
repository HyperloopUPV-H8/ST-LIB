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
	uint8_t channel;
};

class ADC {
public:
	static map<uint8_t, Pin> serviceIDs;

	static map<Pin, ADCchannel> pinTimerMap;
	static forward_list<uint8_t> IDmanager;

	static optional<uint8_t> register_adc(Pin pin);
	static void turn_on_adc(uint8_t id);
	static void turn_off_adc(uint8_t id);
	static optional<uint16_t> get_pin_value(Pin pin);

private:
	static optional<uint16_t[]> get_adc_value(ADC_HandleTypeDef* hadc);

};



#endif /* HALAL_SERVICES_ADC_HPP_ */
