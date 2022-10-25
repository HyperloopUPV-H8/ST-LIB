/*
 * ADC.cpp
 *
 *  Created on: 20 oct. 2022
 *      Author: alejandro
 */

#include "../Inc/HALAL/Services/ADC/ADC.hpp"

#define ADC_BUF1_LEN 1
#define ADC_BUF2_LEN 1
#define ADC_BUF3_LEN 1

extern ADC_HandleTypeDef adc1;
extern ADC_HandleTypeDef adc2;
extern ADC_HandleTypeDef adc3;

extern uint32_t adc_buf1[ADC_BUF1_LEN];
extern uint32_t adc_buf2[ADC_BUF2_LEN];
extern uint32_t adc_buf3[ADC_BUF3_LEN];

uint32_t adc_read1[ADC_BUF1_LEN];
uint32_t adc_read2[ADC_BUF2_LEN];
uint32_t adc_read3[ADC_BUF3_LEN];


forward_list<uint8_t> ADC::IDmanager = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};

map<ADC_HandleTypeDef*, uint32_t[]> adcReadMap = {
		{hadc1, adc_read1},
		{hadc2, adc_read2},
		{hadc3, adc_read3}
};
map<Pin, ADCchannel> pinADCMap = {
		{PE2, {&adc1, adc_buf1, ADC_BUF1_LEN, 1}},
		{PE3, {&adc2, adc_buf2, ADC_BUF2_LEN, 1}},
		{PE4, {&adc3, adc_buf3, ADC_BUF3_LEN, 1}},
};


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	adcReadMap[hadc] = hadc;

}
optional<uint8_t> ADC::register_adc(Pin pin) {
	if (pinADCMap.contains(pin)) {
		Pin::register_pin(pin, ALTERNATIVE);
		uint8_t id = IDmanager.front();
		ADC::serviceIDs[id] = pin;
		ADC::IDmanager.pop_front();
		return id;
	}

	return nullopt;
}

void ADC::turn_on_adc(uint8_t id){
	Pin pin = serviceIDs[id];
	ADCchannel adcChannel = pinADCMap[pin];
	HAL_ADC_Start(adcChannel.adc);
	HAL_ADC_Start_DMA(adcChannel.adc, adcChannel.buffer, adcChannel.buflen);

}

void ADC::turn_off_adc(uint8_t id) {
	Pin pin = serviceIDs[id];
	ADCchannel adcChannel = pinTimerMap[pin];
	HAL_ADC_Stop_DMA(adcChannel.adc);
}

optional<uint16_t> ADC::get_pin_value(Pin pin) {
	if (pinADCMap.contains(pin)) { // check if pin is linked with id
		ADCchannel adcChannel = pinADCMap[pin];
		return (uint16_t)adcChannel.buffer[adcChannel.bufpos];
	}
	return {};
}

