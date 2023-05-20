#include "pressurePID.h"

    float PressurePID::calculatePressurePID(float actualValue, float desiredValue, float iterationTime) { // maybe make non-static? values are in pressure units


        float error = desiredValue - actualValue;
        float integral = prevIntegral + error * iterationTime;
        float derivative = (error - prevError) / iterationTime;
        
        float output = Kp*error + Ki*integral + Kd*derivative + bias;

        prevError = error;
        prevIntegral = integral;

        // DO SOMETHING WITH ITERATION TIME HERE
        
        // iteration time could be tPeriod? Might not update fast enough and need to check during other states besides just in inspiration

        return output;
        
    }
