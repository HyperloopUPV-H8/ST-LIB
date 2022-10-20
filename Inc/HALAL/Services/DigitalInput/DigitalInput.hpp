/*
 * DigitalOut.hpp
 *
 *  Created on: Oct 17, 2022
 *      Author: stefan
 */

#ifndef INC_GPIO_MODULE_DIGITALOUTMODULE_DIGITALOUT_HPP_
#define INC_GPIO_MODULE_DIGITALOUTMODULE_DIGITALOUT_HPP_

#include "../../Models/PinModel/Pin.hpp"

class DigitalInput{
private:
	enum PinState{
		OFF,
		ON
	};
public:
	static map<uint8_t,Pin> serviceIDs;
	static forward_list<uint8_t> IDmanager;

	static uint8_t register_digital_input(Pin& pin);
	static void unregister_digital_input(uint8_t id);
	static PinState read_pin_state(uint8_t id);
};

#endif /* INC_GPIO_MODULE_DIGITALOUTMODULE_DIGITALOUT_HPP_ */
