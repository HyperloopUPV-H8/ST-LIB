#include "ST-LIB_LOW/Sensors/Sensor/Sensor.hpp"

vector<uint8_t> Sensor::adc_id_list{};
vector<uint8_t> Sensor::EXTI_id_list{};

void Sensor::start(){
	for(uint8_t adc_id : adc_id_list){
		ADC::turn_on(adc_id);
	}
	for(uint8_t exti_id : EXTI_id_list){
		ExternalInterrupt::turn_on(exti_id);
	}

}
