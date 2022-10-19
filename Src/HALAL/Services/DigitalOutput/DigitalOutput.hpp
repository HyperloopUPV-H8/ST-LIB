/*
 * DigitalOut.hpp
 *
 *  Created on: Oct 17, 2022
 *      Author: stefan
 */

#ifndef INC_GPIO_MODULE_DIGITALOUTMODULE_DIGITALOUT_HPP_
#define INC_GPIO_MODULE_DIGITALOUTMODULE_DIGITALOUT_HPP_

#include "../../Models/PinModel/Pin.hpp"

class DigitalOutput{
private:
	enum PinState{
		OFF,
		ON
	};
public:
	static map<uint8_t,Pin> serviceIDs;
	static forward_list<uint8_t> IDmanager;

	static uint8_t register_digital_output(Pin& pin);
	static void unregister_digital_output(uint8_t id);
	static void turn_on_pin(uint8_t id);
	static void turn_off_pin(uint8_t id);
	static void set_pin_state(uint8_t id, PinState state);
};

#endif /* INC_GPIO_MODULE_DIGITALOUTMODULE_DIGITALOUT_HPP_ */
