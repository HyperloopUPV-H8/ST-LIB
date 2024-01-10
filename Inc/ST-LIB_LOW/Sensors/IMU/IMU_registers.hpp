#pragma once

#include <map>
#include <stdint.h>

enum REGISTERS_ADDRESSES {
	TEMP_DATA1		= 0x1D,
	TEMP_DATA0		= 0x1E,

	ACCEL_DATA_X1	= 0x1F,
	ACCEL_DATA_X0	= 0x20,
	ACCEL_DATA_Y1	= 0x21,
	ACCEL_DATA_Y0	= 0x22,
	ACCEL_DATA_Z1	= 0x23,
	ACCEL_DATA_Z0	= 0x24,

	GYRO_DATA_X1	= 0x25,
	GYRO_DATA_X0	= 0x26,
	GYRO_DATA_Y1	= 0x27,
	GYRO_DATA_Y0	= 0x28,
	GYRO_DATA_Z1	= 0x29,
	GYRO_DATA_Z0	= 0X2A,

	PWR_MGMT0		= 0X4E,
	WHO_AM_I		= 0x75,

	DEVICE_CONFIG	= 0X11,
	INT_CONFIG		= 0X14,
	ACCEL_CONFIG0	= 0X50,
	GYRO_CONFIG0	= 0X4F,
	INTF_CONFIG1	= 0x4D,
	SENSOR_CONFIG0	= 0x03,

	GYRO_CONFIG_STATIC2	= 0X0B,

	GYRO_CONFIG_STATIC3	= 0x0C,
	GYRO_CONFIG_STATIC4	= 0x0D,
	GYRO_CONFIG_STATIC5	= 0x0E,

	ACCEL_CONFIG_STATIC2	= 0x03,
	ACCEL_CONFIG_STATIC3	= 0x04,
	ACCEL_CONFIG_STATIC4	= 0x05,

	GYRO_CONFIG_STATIC6		= 0x0F,
	GYRO_CONFIG_STATIC7		= 0x10,
	GYRO_CONFIG_STATIC8		= 0x11,
	GYRO_CONFIG_STATIC9		= 0x12,
	GYRO_CONFIG_STATIC10	= 0x13
};

enum TEMPERATURE_OPERACION_MODES {
	TEMPERATURE_OFF = 0b01,
	TEMPERATURE_ON	= 0b00
};

enum RC_OPERATION_MODE {
	RC_ALLWAYS_ON	= 0b01,
	RC_STANBY		= 0b00,
};

enum ACCELERATION_OPERATION_MODES {
	ACCELERATION_OFF		= 0b00,
	ACCELERATION_OFF_2		= 0b01,
	ACCELERATION_LOW_POWER	= 0b10,
	ACCELERATION_LOW_NOISE	= 0b11,
};

enum INT_MODE {
	PULSED_MODE		= 0b00,
	LATCHED_MODE	= 0b01,
};

enum INT_DRIVE_CIRCUIT {
	OPEN_DRAIN		= 0b00,
	PUSH_PULL		= 0b01,
};

enum INT_POLARITY {
	ACTIVE_LOW		= 0b00,
	ACTIVE_HIGH		= 0b00,
};

enum ACCELERATION_FULL_SCALE {
	ACCELERATION_16G	= 0b000,
	ACCELERATION_8G		= 0b001,
	ACCELERATION_4G		= 0b010,
	ACCELERATION_2G		= 0b011,
};

enum ACCELERATION_ODR {
	ACCELERATION_32KHZ	= 0x0001, // LN MODE
	ACCELERATION_16KHZ	= 0x0010,
	ACCELERATION_8KHZ	= 0x0011,
	ACCELERATION_4KHZ	= 0x0100,
	ACCELERATION_2KHZ	= 0x0101,
	ACCELERATION_1KHZ	= 0x0110,
	ACCELERATION_500HZ	= 0x0111, // LP or LN MODE
	ACCELERATION_200HZ	= 0x1000, // LP MODE
	ACCELERATION_100HZ	= 0x1001,
	ACCELERATION_50HZ	= 0x1010,
	ACCELERATION_25HZ	= 0x1011,
	ACCELERATION_12_5HZ	= 0x1100,
	ACCELERATION_6KHZ	= 0x1101,
	ACCELERATION_3HZ	= 0x1110,
	ACCELERATION_1HZ	= 0x1111,
};

enum ACCEL_LP_CLK_SEL {
	WAKE_UP_OSCILATOR	= 0b0,
	RC_OSCILLATOR_CLOCK	= 0b1
};

enum RTC_MODE {
	NO_RTC_REQUIRED = 0b0,
	RTC_REQUIRED	= 0b1
};

enum CLKSEL {
	ALLWAYS_INTERNAL_OSCILATOR	= 0b00,
	PLL_WHEN_AVAILABLE			= 0b01,
	DISABLE_ALL_CLOCKS			= 0b11,
};

enum FILTER_ENABLE {
	FILTER_ON		= 0b0,
	FILTER_DISABLED	= 0b1
};

std::map<uint8_t, double> ACCLERATION_FS_SENSITIVITY = {
		{ACCELERATION_16G, 2048},
		{ACCELERATION_8G, 4096},
		{ACCELERATION_4G, 8192},
		{ACCELERATION_2G, 16384}
};


#pragma pack(1)

struct SensorConfigFields {
	unsigned ACCEL_MODE	: 2;	// LSB
	unsigned GYRO_MODE	: 2;
	unsigned IDLE		: 1;
	unsigned TEMP_DIS	: 1;
	unsigned __RESERVED	: 2;	// MSB
};

union SensorConfigRegister{
	uint8_t value;
	SensorConfigFields fields;
};


struct DeviceConfigFields {
	unsigned SOFT_RESET_CONFIG	: 1; // LSB
	unsigned __RESERVED_2		: 3;
	unsigned SPI_MODE			: 1;
	unsigned __RESERVED			: 3; // MSB
};

union DeviceConfigRegister {
	uint8_t value;
	DeviceConfigFields fields;
};


struct IntConfigFields {
	unsigned INT2_POLARITY		: 1; // LSB
	unsigned INT2_DRIVE_CIRCUIT	: 1;
	unsigned INT2_MODE			: 1;
	unsigned INT1_POLARITY		: 1;
	unsigned INT1_DRIVE_CIRCUIT	: 1;
	unsigned INT1_MODE			: 1;
	unsigned __RESERVED			: 2; // MSB
};

union IntConfigRegister {
	uint8_t value;
	IntConfigFields fields;
};


struct AccelerationConfigFields {
	unsigned ACCEL_ODR		: 4; // LSB
	unsigned __RESERVED_2	: 1;
	unsigned ACCEL_FS_SEL	: 3; // MSB
};

union AccelerationConfigRegister{
	uint8_t value;
	AccelerationConfigFields fields;
};


////////////////////////////////////
//
// 		ANTI-ALIAS FILTER
//
////////////////////////////////////

struct AccelConfigStatic2 {
	unsigned ACCEL_AAF_DIS	: 1; // LSB
	unsigned ACCEL_AAF_DELT	: 6;
	unsigned _RESERVED		: 1; // MSB
};

union AccelConfigStatic2Register{
	uint8_t value;
	AccelConfigStatic2 fields;
};



struct AccelConfigStatic3 {
	unsigned ACCEL_AAF_DELTSQR : 8;
};

union AccelConfigStatic3Register{
	uint8_t value;
	AccelConfigStatic3 fields;
};



struct AccelConfigStatic4 {
	unsigned ACCEL_AAF_DELTSQR	: 4; // LSB
	unsigned ACCEL_AAF_BITSHIFT	: 4; // MSB

};

union AccelConfigStatic4Register{
	uint8_t value;
	AccelConfigStatic4 fields;
};


#pragma pack(0)