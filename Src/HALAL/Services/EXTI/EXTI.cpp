/*
 * EXTI.cpp
 *
 *  Created on: Nov 5, 2022
 *      Author: aleja
 */

#include "EXTI/EXTI.hpp"

forward_list<uint8_t> ExternalInterrupt::id_manager = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};
map<Pin, ExternalInterrupt::Instance> ExternalInterrupt::instances_data = {
	{PE7, ExternalInterrupt::Instance(PE7, GPIO_PIN_7)}
};
map<uint8_t, ExternalInterrupt::Instance> ExternalInterrupt::instances = {};

void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin) {
	for(auto i : ExternalInterrupt::instances) {
		ExternalInterrupt::Instance& instance = i.second;
		if (!(instance.gpio == GPIO_Pin)) { continue; }
		(*instance.action)();
	}
}

ExternalInterrupt::Instance::Instance(Pin p, uint16_t g) {
	pin = p;
	gpio = g;
}

optional<uint8_t> ExternalInterrupt::register_exti(Pin& pin, function<void()>* action) {
	if (!ExternalInterrupt::instances_data.contains(pin)) { return nullopt;}

	Pin::register_pin(pin, ALTERNATIVE);
	uint8_t id = id_manager.front();
	id_manager.pop_front();

	Instance instance = instances_data[pin];
	instances[id] = instance;
	instances[id].id = id;
	instances[id].action = action;

	return id;
}

void ExternalInterrupt::unregister_exti(uint8_t id) {
	Pin::unregister_pin(instances[id].pin);
	instances.erase(id);
	id_manager.push_front(id);
}

void ExternalInterrupt::turn_off_exti(uint8_t id) {
	Instance& instance = instances[id];
	instance.is_on = false;
}

void ExternalInterrupt::turn_on_exti(uint8_t id) {
	Instance& instance = instances[id];
	instance.is_on = true;
}

bool ExternalInterrupt::get_pin_value(uint8_t id) {
	Instance& instance = instances[id];
	return HAL_GPIO_ReadPin(GPIO_PORT, instance.pin.pin);
}



