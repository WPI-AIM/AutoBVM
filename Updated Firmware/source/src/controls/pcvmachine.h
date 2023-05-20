#ifndef UVENT_PCVMACHINE_H
#define UVENT_PCVMACHINE_H

#include "actuators/actuator.h"
#include "machine.h"
#include "controls/fault.h"
#include "../config/uvent_conf.h"
#include "waveform.h"
#include "alarm/alarm.h"
#include "sensors/pressure_sensor.h"

// #define stringify(name) #name

// enum class States {
//     ST_STARTUP = 0,
//     ST_INSPR,
//     ST_INSPR_HOLD,
//     ST_EXPR,
//     ST_PEEP_PAUSE,
//     ST_EXPR_HOLD,
//     ST_ACTUATOR_HOME,
//     ST_ACTUATOR_JOG,
//     ST_FAULT,
//     ST_DEBUG,
//     ST_OFF,
//     ST_COUNT// Add above. This needs to be the last item
// };

class PCVMachine {
public:
    // Constructor
    PCVMachine(States, Actuator*, Waveform*, PressureSensor* gauge_pressure, AlarmManager*, uint32_t* cycle_count);

    void setup();
    void run();
    const char* get_current_state_string();
    const char** get_state_list(uint8_t* size);
    States get_current_state();
    void change_state(States);
    // void change_motor(Motors);

    void handle_errors();
    void set_fault(Fault);

    States get_last_state();        // JOSH PRESSURE
    States lastState;

    // PressurePID pid;

// const double Kp = 0.0011;       // VALUES OBTAINED FROM A RESEARCH PAPER
// const double Ki = 0.012;
// const double Kd = 0.000015;

const double Kp = 1.0;       // TEST VALUES
const double Ki = 0.0;
const double Kd = 0.0;

float prevError = 0;
float prevIntegral = 0;
float bias = 0;

// Used for timing purposes
bool firstPID = true;
float timeSincePID = 0;

float target = 15.0;

bool startPID = false;


private:
    // Current state of the state machine.
    States state;
    Motors motor;

    ControlModes mode;

    // Boolean to signify initial startup upon powering on the ventilator
    bool firstInit = true;

    // Condition to evaluate code on first entry into a state
    bool state_first_entry = false;

    /* An internal timer to take care of any internal state timings.
     * Increments everytime the run function is run to keep time.
     * and resets before change of state.
     */
    uint32_t machine_timer;
    uint32_t* cycle_count;

    //State transistion times
    const uint32_t start_home_in_ms = 2000;// Start to home after startup
    const uint32_t start_home_in_ticks = (start_home_in_ms * 1000) / CONTROL_HANDLER_PERIOD_US;

    const uint32_t check_actuator_move_in_ms = 400;// Wait time before checking for actuator movement.
    const uint32_t check_actuator_move_in_ticks = (check_actuator_move_in_ms * 1000) / CONTROL_HANDLER_PERIOD_US;

    // Debug fault flag for actuator.
    bool actuator_force_fault_debug = false;

    // Fault code
    Fault fault_id;

    Actuator* p_actuator;

    Waveform* p_waveform;
    waveform_params* p_waveparams;

    AlarmManager* p_alarm_manager;

    PressureSensor* p_gauge_pressure;

    bool inspiration_state_triggered;

    // Set the current state in the state machine
    void set_state(States);

    // Set motor type being used
    // void set_motor(Motors);

    void set_mode(ControlModes);

    // Boolean indicating if machine is in state ST_EXPR to correct homing bug.
    bool in_expiration;

    // State functions
    void state_startup();
    void state_inspiration();
    void state_inspiration_hold();
    void state_expiration();
    void state_peep_pause();
    void state_expiration_hold();
    void state_actuator_home();
    void state_actuator_jog();
    void state_fault();
    void state_debug();
    void state_off();
};
#endif