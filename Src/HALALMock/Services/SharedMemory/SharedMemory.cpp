
#include "HALALMock/Services/SharedMemory/SharedMemory.hpp"

void SharedMemory::start() {
    // TODO: create / open shared memory
}
EmulatedPin &SharedMemory::get_pin(Pin pin){
    uint8_t offset;
	auto it = pin_offsets.find(pin);
	if(it != pin_offsets.end()){
		offset = it -> second;
	}else{
		ErrorHandler("Pin %s doesn't exist",pin.to_string());
	}
	EmulatedPin *pin_memory = SharedMemory::gpio_memory + offset;
    return *pin_memory;
}