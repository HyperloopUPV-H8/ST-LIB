#pragma once

#include "../ControlBlock.hpp"
#include "math.h"

struct LevitationPosition{
    double y, z;
    double rotation_x, rotation_y, rotation_z;
};

class LevitationPositionCalculator : public ControlBlock<double(*)[8], LevitationPosition>{
public:
    static constexpr double x_sragy = 0.8069, y_sragy = 0.1042, z_sragy = 0.10825; 
    static constexpr double x_sragz = 0.81231, y_sragz = 0.2114, z_sragz = -0.0396;
    static constexpr double z_infr = 0.6328;
    static constexpr double grosorz = 0.015, grosory = 0.005;

    double(&distances)[8] = *this->input_value;

    LevitationPositionCalculator(double(&distances)[8]){
        this->input_value = &distances;
        output_value = {0.0,0.0,0.0,0.0,0.0};
    };
    void execute() override{
        reset_position();
        output_value.rotation_x = (asin((distances[1] - distances[0])/(2*y_sragz)) + asin((distances[3] - distances[2])/(2*y_sragz)))/2.0;
        output_value.rotation_y = (asin((distances[0] - distances[2])/(2*x_sragz)) + asin((distances[1] - distances[3])/(2*x_sragz)))/2.0;
        output_value.rotation_z = (asin((distances[4] - distances[6])/(2*x_sragy)) + asin((distances[7] - distances[5])/(2*x_sragy)))/2.0;

        output_value.y += (distances[4] - y_sragy*cos(output_value.rotation_z) - x_sragy*sin(output_value.rotation_z) + y_sragz*sin(output_value.rotation_x))/4.0;
        output_value.y += ((-1*distances[5]) + y_sragy*cos(output_value.rotation_z) - x_sragy*sin(output_value.rotation_z) + y_sragz*sin(output_value.rotation_x))/4.0;
        output_value.y += (distances[6] - y_sragy*cos(output_value.rotation_z) + x_sragy*sin(output_value.rotation_z) + y_sragz*sin(output_value.rotation_x))/4.0;
        output_value.y += ((-1*distances[7]) + y_sragy*cos(output_value.rotation_z) + x_sragy*sin(output_value.rotation_z) + y_sragz*sin(output_value.rotation_x))/4.0;

        output_value.z += (z_infr - distances[0] - z_sragz*cos(output_value.rotation_y) + x_sragz*sin(output_value.rotation_y) - y_sragz*sin(output_value.rotation_x))/4.0;
        output_value.z += (z_infr - distances[1] - z_sragz*cos(output_value.rotation_y) + x_sragz*sin(output_value.rotation_y) + y_sragz*sin(output_value.rotation_x))/4.0;
        output_value.z += (z_infr - distances[2] - z_sragz*cos(output_value.rotation_y) - x_sragz*sin(output_value.rotation_y) - y_sragz*sin(output_value.rotation_x))/4.0;
        output_value.z += (z_infr - distances[3] - z_sragz*cos(output_value.rotation_y) - x_sragz*sin(output_value.rotation_y) + y_sragz*sin(output_value.rotation_x))/4.0;
    }

    void reset_position(){
        output_value = {0.0,0.0,0.0,0.0,0.0};
    }
};
