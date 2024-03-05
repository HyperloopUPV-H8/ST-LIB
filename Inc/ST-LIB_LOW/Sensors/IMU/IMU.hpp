#pragma once
#include "HALAL/Services/Communication/SPI/SPI.hpp"

#include "IMU/IMU_registers.hpp"
class IMU{
public:
    IMU() =default;
    IMU(SPI::Peripheral spi,double* accel_x,double* accel_y,double* accel_z,double* gyro_x,double* gyro_y,double* gyro_z,double* temp);
    void read();
private:
        //
        //          METHODS
        //

    uint8_t read_register(uint8_t reg);
    bool write_register(uint8_t reg,uint8_t data);
    uint8_t read_pwm_mgmt0_fields();
    void write_sensors_configuration(TEMPERATURE_OPERACION_MODES temp_dis,
                RC_OPERATION_MODE idle,ACCELERATION_OPERATION_MODES accel_mode);
    SensorConfigRegister read_sensor_configuration();

         //
         //          MEMBER VARIABLES
         //       
    double* accel_x{};
    double* accel_y{};
    double* accel_z{};
    double* gyro_x{};
    double* gyro_y{};
    double* gyro_z{};
    double* temp{};
    uint8_t spi_id;
    SPI::Instance* spi_instance;
    uint8_t rx_data[1]{};

    const double accel_sensitivity {16384};
    const double gyro_sensitivity  {16.4};
};