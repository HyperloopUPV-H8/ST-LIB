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
    AccelerationConfigRegister read_acceleration_config();
    void write_acceleration_config(ACCELERATION_FULL_SCALE sf,ACCELERATION_ODR odr);
    uint8_t read_id();
    void soft_reset();
    void turn_on_sensors();
    void turn_off_sensors();
    void config_accel_antialias(uint16_t freq);
    double read_temp();
    double read_accel_x();
    double read_accel_y();
    double read_accel_z();
    void start_imu();
    void read_imu_data();
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

    double accel_sensitivity {16384};
    double gyro_sensitivity  {16.4};
};