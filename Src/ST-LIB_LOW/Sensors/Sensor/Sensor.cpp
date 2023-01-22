#include "ST-LIB_LOW/Sensors/DigitalSensor/DigitalSensor.hpp"
#include "ST-LIB_LOW/Sensors/LookupSensor/LookupSensor.hpp"
#include "ST-LIB_LOW/Sensors/Sensor/Sensor.hpp"
#include "ADC/ADC.hpp"

vector<uint8_t> SensorStarter::adc_id_list{};
vector<uint8_t> SensorStarter::EXTI_id_list{};

void SensorStarter::start(){
	for(uint8_t adc_id : adc_id_list){
		ADC::turn_on(adc_id);
	}
	for(uint8_t exti_id : EXTI_id_list){
		ExternalInterrupt::turn_on(exti_id);
	}

}
