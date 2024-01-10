#include "IMU/IMU.hpp"

IMU::IMU(SPI::Peripheral spi,double* accel_x,double* accel_y,double* accel_z,double* gyro_x,double* gyro_y,double* gyro_z,double* temp):
accel_x(accel_x),accel_y(accel_y),accel_z(accel_z),gyro_x(gyro_x),gyro_y(gyro_y),gyro_z(gyro_z),temp(temp)
{
    spi_id = SPI::inscribe(spi);
    spi_instance = SPI::registered_spi[spi_id];
}

uint8_t IMU::read_pwm_mgmt0_fields(){
        return read_register(PWR_MGMT0);
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