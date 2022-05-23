#ifndef UVENT_STEPPER_H
#define UVENT_STEPPER_H

#include "../../config/uvent_conf.h"
#include <AccelStepper.h>
#include <ams_as5048b.h>

class Stepper {
public:
    void init();
    bool run();
    void set_speed(float);
    // double get_position();
    void home();
    bool is_moving();
    bool remaining_steps_to_go();
    void set_position(double);
    void set_position_relative(double steps);
    void set_position_as_home(int32_t position);
    bool target_reached();
    void set_enable(bool en);
    uint32_t get_current_position();

private:
    // Accelstepper object for Stepper 5718L
    AccelStepper stepper_5718L{AccelStepper::DRIVER, STEPPER_STEP_PIN, STEPPER_DIRECTION_PIN};
};

#endif