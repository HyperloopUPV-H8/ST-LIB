#include "IMU/IMU.hpp"

IMU::IMU(SPI::Peripheral spi,double* accel_x,double* accel_y,double* accel_z,double* gyro_x,double* gyro_y,double* gyro_z,double* temp):
accel_x(accel_x),accel_y(accel_y),accel_z(accel_z),gyro_x(gyro_x),gyro_y(gyro_y),gyro_z(gyro_z),temp(temp)
{
    spi_id = SPI::inscribe(spi);
    spi_instance = SPI::registered_spi[spi_id];
}



uint8_t IMU::read_register(uint8_t reg)
{
    //chip SS is active low
    uint8_t new_reg = reg | 0b10000000;
    SPI::turn_off_chip_select(spi_instance);
    SPI::transmit(spi_id,new_reg);
    SPI::receive(spi_id,rx_data);
    return *rx_data;

}

bool IMU::write_register(uint8_t reg,uint8_t data){
    //chip SS is active low
    SPI::turn_off_chip_select(spi_instance);
    SPI::transmit(spi_id,reg);
    SPI::transmit(spi_id,data);
    SPI::turn_on_chip_select(spi_instance);
    return read_register(reg) == data;
}
uint8_t IMU::read_pwm_mgmt0_fields(){
        return read_register(PWR_MGMT0);
}

void IMU::write_sensors_configuration(	TEMPERATURE_OPERACION_MODES temp_dis,
								 	 RC_OPERATION_MODE idle,
									ACCELERATION_OPERATION_MODES accel_mode	){
	SensorConfigRegister reg;
	reg.value = 0;
	reg.fields.TEMP_DIS		= temp_dis;
	reg.fields.ACCEL_MODE	= accel_mode;
	reg.fields.GYRO_MODE	= 0;
	reg.fields.IDLE 		= idle;
	write_register(PWR_MGMT0, reg.value);
}

SensorConfigRegister IMU::read_sensor_configuration() {
	SensorConfigRegister reg;
	reg.value = read_register(PWR_MGMT0);
	return reg;
}


AccelerationConfigRegister IMU::read_acceleration_config(){
    AccelerationConfigRegister reg;
    reg.value = read_register(ACCEL_CONFIG0);
}


void IMU::write_acceleration_config(ACCELERATION_FULL_SCALE sf,ACCELERATION_ODR odr)
{
    AccelerationConfigRegister reg;
    reg.fields.ACCEL_FS_SEL = sf;
    reg.fields.ACCEL_ODR = odr;
    reg.fields.__RESERVED_2 = 0;
    accel_sensitivity = ACCLERATION_FS_SENSITIVITY[(uint8_t)sf];
    write_register(ACCEL_CONFIG0,reg.value);
}


uint8_t IMU::read_id(){
    return read_register(0x75);
}

void IMU::soft_reset(){
    DeviceConfigRegister reg;
    reg.value = 0;
    reg.fields.SOFT_RESET_CONFIG = 1;
    write_register(DEVICE_CONFIG,reg.value);
    HAL_Delay(5);
}


void IMU::turn_on_sensors(){
    write_sensors_configuration(TEMPERATURE_ON,RC_ALLWAYS_ON,ACCELERATION_LOW_NOISE);
    HAL_Delay(5);
}

void IMU::turn_off_sensors(){
    write_sensors_configuration(TEMPERATURE_OFF,RC_ALLWAYS_ON,ACCELERATION_OFF);
    HAL_Delay(5);
}


double IMU::read_temp(){
    int16_t raw = (read_register(TEMP_DATA1)<<8) | read_register(TEMP_DATA0);
    return (raw/132.48) +25;
}
double IMU::read_accel_x(){
	int16_t raw = (int16_t) (read_register(ACCEL_DATA_X1) << 8) | read_register(ACCEL_DATA_X0);
	return  raw / accel_sensitivity;    
}
double IMU::read_accel_y(){
	int16_t raw = (int16_t) (read_register(ACCEL_DATA_Y1) << 8) | read_register(ACCEL_DATA_Y0);
	return  raw / accel_sensitivity;
}

double IMU::read_accel_z(){
	int16_t raw = (int16_t) (read_register(ACCEL_DATA_Z1) << 8) | read_register(ACCEL_DATA_Z0);
	return  raw / accel_sensitivity;
}

void IMU::start_imu(){
	soft_reset();
	HAL_Delay(10);

	write_acceleration_config(ACCELERATION_8G, ACCELERATION_50HZ);
	config_accel_antialias(ANTIALIAS_FREQ_536_HZ);
}

void IMU::read_imu_data(){
	*accel_x = read_accel_x();
	*accel_y = read_accel_y();
	*accel_z = read_accel_z();
}