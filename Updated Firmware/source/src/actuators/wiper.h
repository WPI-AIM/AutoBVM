#ifndef UVENT_WIPER_H
#define UVENT_WIPER_H

#include "../../config/uvent_conf.h"
#include <ams_as5048b.h>

class Wiper {
public:
    void init();
    bool run();
    // void set_speed(float);
    // double get_position();
    void home();

    // Use for when actuator is in expiration state (machine is in state ST_EXPR)
    // void home_expiration();
    
    // bool is_moving();
    // bool remaining_steps_to_go();
    // void set_position(double);
    // void set_position_relative(double steps);
    // void set_position_as_home(int32_t position);
    // bool target_reached();
    // void set_enable(bool en);
    // uint32_t get_current_position();

    bool get_wiperMotorOn();
    bool get_relayOn();
    void shutdown();

    // void set_interval(float value);
    // void set_pause(float value);

    // Set to public to be used in actuator.set_wiper_motor_on()
    bool wiperMotorOn;
    bool doLastCycle = false;
    float interval;  // Time between wiper rotation starts in ms
    // float pause;  // Time that wiper pauses in between inspiration and expiration

    private:

        bool relayOn = false;
        bool firstCycle = true;  // Used for wiper.run()
        bool firstLoop = false;
        bool secondLoop = false;

        uint32_t time1;
        uint32_t timeDiff;

};

#endif