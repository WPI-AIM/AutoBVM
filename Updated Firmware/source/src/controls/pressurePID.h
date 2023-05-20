#ifndef UVENT_PRESSUREPID_H
#define UVENT_PRESSUREPID_H

class PressurePID { 
public:
    const double Kp = 0.0011;       // CHANGE FOR TUNING
    const double Ki = 0.012;
    const double Kd = 0.000015;

    float prevError = 0;
    float prevIntegral = 0;
    float bias = 0;

    // Used for timing purposes
    bool firstPID = true;
    float timeSincePID = 0;

    float target = 15.0; // CHANGE IN FUTURE DEPENDING ON GOOD ALLOWABLE VALUES, 15 IS JUST FOR TESTING PURPOSES

    float calculatePressurePID(float actualValue, float desiredValue, float iterationTime);

};

#endif //UVENT_PRESSUREPID_H
