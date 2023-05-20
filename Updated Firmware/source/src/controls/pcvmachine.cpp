
#include <Arduino.h>
#include <display/layouts/layouts.h>
#include "pcvmachine.h"
#include "actuators/actuator.h"
#include "utilities/util.h"
#include "pressurePID.h"

// Stringify states                                     // NEED TO FIX LATER, REDEFINITION ERRORS
// const char* state_string[] =
//         {
//                 stringify(ST_STARTUP),
//                 stringify(ST_INSPR),
//                 stringify(ST_INSPR_HOLD),
//                 stringify(ST_EXPR),
//                 stringify(ST_PEEP_PAUSE),
//                 stringify(ST_EXPR_HOLD),
//                 stringify(ST_ACTUATOR_HOME),
//                 stringify(ST_ACTUATOR_JOG),
//                 stringify(ST_FAULT),
//                 stringify(ST_DEBUG),
//                 stringify(ST_OFF)};

// // PressurePID pid;

// // const double Kp = 0.0011;       // VALUES OBTAINED FROM A RESEARCH PAPER
// // const double Ki = 0.012;
// // const double Kd = 0.000015;

// const double Kp = 1.0;       // TEST VALUES
// const double Ki = 0.0;
// const double Kd = 0.0;

// float prevError = 0;
// float prevIntegral = 0;
// float bias = 0;

// // Used for timing purposes
// bool firstPID = true;
// float timeSincePID = 0;

// float target = 15.0;

// bool startPID = false;


PCVMachine::PCVMachine(States st, Actuator* act, Waveform* wave, PressureSensor* gp, AlarmManager* al, uint32_t* cc)  // no differential pressure sensor, just gauge
{
    p_actuator = act;
    state = st;

    p_waveform = wave;
    p_waveparams = wave->get_params();
    // Calculate the waveform parameters
    p_waveform->calculate_waveform();

    p_alarm_manager = al;
    cycle_count = cc;
    p_gauge_pressure = gp;
}

// Set the current state in the state machine
void PCVMachine::set_state(States new_state)
{
    state_first_entry = true;
    state = new_state;

    // Reset the timer before each state starts.
    machine_timer = 0;
}

// void Machine::set_motor(Motors new_motor)  // Set current/new motor
// {
//     motor = new_motor;
//     p_actuator->change_motor(new_motor);
// }

// void Machine::set_mode(ControlModes new_mode)  // Set control mode
// {
//     mode = new_mode;                                                                  
//     // p_actuator->change_motor(new_motor);
// }


// State functions
void PCVMachine::state_startup()
{

    in_expiration = false;

    if (state_first_entry) {
        state_first_entry = false;
    }

    // For testing. Each tick is CONTROL_HANDLER_PERIOD_US
    if (machine_timer > start_home_in_ticks) {
        lastState = States::ST_STARTUP;
        set_state(States::ST_OFF);
    }
}

void PCVMachine::state_inspiration()
{

    in_expiration = false;

    if (state_first_entry) {

        // Clear all faults. They will get processed as states run.
        fault_id = Fault::FT_NONE;

        // Check if paddle is at home.
        if (!(p_actuator->is_home()) && ENABLE_WIPER_MOTOR == 1) {
#if USE_AMS_FEEDBACK
            p_actuator->add_correction();
            state_first_entry = true;
#else
            lastState = States::ST_INSPR;
            set_state(States::ST_ACTUATOR_HOME);
            inspiration_state_triggered = true;
#endif
            return;
        }
        
        if(ENABLE_WIPER_MOTOR == 1)
        {
            p_actuator->set_wiper_motor_on();  // If wiper motor, mark it as on
            p_actuator->wiper_set_interval(p_waveparams->tPeriod);
        }

        (*cycle_count)++;

        state_first_entry = false;

        // Calculate the waveform parameters                Maybe need to put pressure PID here
        if (p_waveform->calculate_waveform() == -1) {
            // Set the fault ID:
            fault_id = Fault::FT_WAVEFORM_CALC_ERROR;
            // Error in waveform calculation. Switch to error state
            lastState = States::ST_INSPR;
            set_state(States::ST_FAULT);

            // Return early. Don't setup the actuator.
            return;
        }
        // NEED TO CHANGE THIS WITH PRESSURE, JOSH PRESSURE
        // need to calculate volume based on current pressure value, then use that to make motor move
        float goal_pos_deg = p_actuator->volume_to_degrees(C_Stat::FIFTY, p_waveparams->volume_ml / 1000); // Takes tidal volume and calculates motor rotation amount
        float vel_deg = 0;

        // Calculate how much and at what speed the actuator should move.
        p_actuator->calculate_trajectory(p_waveparams->tIn, p_waveparams->tHoldIn - p_waveparams->tIn, goal_pos_deg, vel_deg);
        // if(motor == Motors::WIPER)
        // {
        //     p_actuator->calculate_trajectory(p_waveparams->tPeriod, goal_pos_deg, vel_deg);

        // }

        // Move the actuator
        p_actuator->set_position(Tick_Type::TT_DEGREES, goal_pos_deg);
        p_actuator->set_speed(Tick_Type::TT_DEGREES, vel_deg);  // JOSH PRESSURE use this to set the speed to the output of PID stuff located in expiration state
    }

    // Check if target has been reached.
    if (p_waveform->is_inspiration_done()) {
        // Keep track of max pip.
        p_waveform->set_current_pip(p_gauge_pressure->get_pressure(units_pressure::cmH20)); //JOSH PRESSURE

        // Note the volume dispensed. Keep this as a copy of the command volume.
        // Replace this with pressure?
        p_waveparams->m_tidal_volume = p_waveparams->volume_ml;

        lastState = States::ST_INSPR;
        set_state(States::ST_INSPR_HOLD);
    }
}

void PCVMachine::state_inspiration_hold()
{

    in_expiration = false;  

    if (state_first_entry) {
        state_first_entry = false;
    }
    if (p_waveform->is_inspiration_hold_done()) {
        // Save the plateau pressure.
        p_waveparams->m_plateau_press = p_gauge_pressure->get_pressure(units_pressure::cmH20);  // JOSH PRESSURE

        // Mark the inspiration time
        p_waveform->mark_inspiration_time(now_s());

        lastState = States::ST_INSPR_HOLD;
        set_state(States::ST_EXPR);
    }
}

void PCVMachine::state_expiration()
{

    in_expiration = true;

    if (state_first_entry) {
        state_first_entry = false;

        float goal_pos_deg = 0;              // Fully retracted.
        float duration_s = p_waveparams->tEx;// - (now() - p_waveparams->tCycleTimer);
        float pause_s = p_waveparams->tHoldIn - p_waveparams->tIn;  // Pause between inspiration and expiration
        float vel_deg = 0;

        // Calculate how much and at what speed the actuator should move.
        p_actuator->calculate_trajectory(duration_s, pause_s, goal_pos_deg, vel_deg);
        // if(motor == Motors::WIPER)
        // {
        //     p_actuator->calculate_trajectory(p_waveparams->tPeriod, goal_pos_deg, vel_deg);

        // }

        // Move the actuator
        p_actuator->set_position(Tick_Type::TT_DEGREES, goal_pos_deg);
        p_actuator->set_speed(Tick_Type::TT_DEGREES, vel_deg);
    }

    // Check if target has been reached.
    if (p_actuator->target_reached()) {
        lastState = States::ST_EXPR;
        set_state(States::ST_PEEP_PAUSE);
    }
}

void PCVMachine::state_peep_pause()
{

    in_expiration = false;

    if (state_first_entry) {
        state_first_entry = false;
    }

    if (p_waveform->is_peep_pause_done()) {
        // Save the peep pressure.
        p_waveparams->m_peep = p_gauge_pressure->get_pressure(units_pressure::cmH20);   // JOSH PRESSURE
        p_waveform->set_pip_peak_and_reset();
        lastState = States::ST_EXPR;
        set_state(States::ST_EXPR_HOLD);
    }
}

void PCVMachine::state_expiration_hold() // Check and calculate pressure level/PID here
{

    in_expiration = false;

    if (state_first_entry) {
        state_first_entry = false;
    }

    if(get_last_state() == States::ST_OFF && get_current_state() == States::ST_INSPR)
    {
        startPID = true;
    }
    
    if(startPID == true)
    {

        float cur_pressure = p_gauge_pressure->get_pressure(units_pressure::cmH20);

        if(firstPID == true)
        {
            float error = target - cur_pressure;
            float integral = prevIntegral + error;
            float derivative = (error - prevError);
             
            double output = Kp*error + Ki*integral + Kd*derivative + bias;

            prevError = error;
            prevIntegral = integral;

            firstPID = false;
            timeSincePID = millis();
        }

        else
        {
            float error = target - cur_pressure;
            float integral = prevIntegral + (error * (millis() - timeSincePID));           // ERROR WILL KEEP GETTING MORE MASSIVE UNLESS PRESSURE GOES OVER TARGET
            float derivative = (error - prevError) / (millis() - timeSincePID);
            
            float output = Kp*error + Ki*integral + Kd*derivative + bias;

            prevError = error;
            prevIntegral = integral;

            timeSincePID = millis();
        }  
    } 

    if(get_current_state() == States::ST_OFF)
    {
        startPID = false;
    }
    if (p_waveform->is_expiration_done()) 
    {
        lastState = States::ST_EXPR_HOLD;
        set_state(States::ST_INSPR);

        p_waveform->calculate_respiration_rate();

        // Mark the inspiration time
        p_waveform->mark_expiration_time(now_s());

        // Calculate waveform params for UI reporting
        p_waveform->calculate_current_parameters();
    }

}

void PCVMachine::state_actuator_home()
{
    // Store the is_home status temporarily.
    bool is_home = p_actuator->is_home();

    if (state_first_entry) {
        state_first_entry = false;
            disable_start_button();

        // Check if the paddle is at home position
        // If not move the paddle to home.
        if (is_home) {
            // Let the motor driver know that this is 0 position
            p_actuator->set_position_as_home();

            // Reset measured parameters.
            p_waveform->reset_measured_params();

            enable_start_button();
            lastState = States::ST_ACTUATOR_HOME;
            set_state(States::ST_OFF);
        }
        else {
            // Start the home sequence
            if (in_expiration == true) {
                p_actuator->home_expiration();
            }
            else {
            p_actuator->home();
            }
        }
    }

    // Has the paddle reached home? If no, keep moving.
    if (is_home) {
        // Actuator is home. Stop the actuator.
        p_actuator->set_speed(Tick_Type::TT_DEGREES, 0);
        // Let the motor driver know that this is 0 position
        p_actuator->set_position_as_home();

        // Reset measured parameters.
        p_waveform->reset_measured_params();

        enable_start_button();
        //set_state(States::ST_OFF);
        if (inspiration_state_triggered) {
            inspiration_state_triggered = false;
            lastState = States::ST_ACTUATOR_HOME;
            set_state(States::ST_INSPR);
        }
        else {
            lastState = States::ST_ACTUATOR_HOME;
            set_state(States::ST_OFF);
        }
    }
    else {
        // Homing in progress.

        /* Check if the actuator is moving, by checking feedback
        * only if home is not reached.
        * Also do the check after a time delay as it takes time for
        * the drive to respond.
        * The || check for fault flag is for checking a forced fault through the parser.
        * Only check if feedback position chip is enabled.
        */
#if USE_AMS_FEEDBACK
        if (machine_timer > check_actuator_move_in_ticks) {
            if (((is_home == false) && (p_actuator->is_moving() == false)) || (actuator_force_fault_debug == true)) {
                // Set the fault ID:
                fault_id = Fault::FT_ACTUATOR_FAULT;
                enable_start_button();
                // Actuator is not moving. Switch to error state
                set_state(States::ST_FAULT);

                // Reset the force fault
                actuator_force_fault_debug = false;
            }
        }
        else {
            // Service is_moving, so that the prev_position is valid, when the above condition is true.
            p_actuator->is_moving();
        }
#endif
    }
}

void PCVMachine::state_actuator_jog()
{
    // Stub for jogging the actuator during debug.
}

void PCVMachine::state_fault()
{
    if (state_first_entry) {
        state_first_entry = false;

        // Stop the actuator
        p_actuator->set_speed(Tick_Type::TT_DEGREES, 0);

        Serial.print("Fault code : ");
        Serial.println((int) fault_id);
    }
}

void PCVMachine::state_debug()
{
}

void PCVMachine::state_off()
{
    if (state_first_entry) {
        state_first_entry = false;

        // Reset the cycle counter
        *cycle_count = 0;

        // Reset all alarms.
        p_alarm_manager->allOff();
    }
}

void PCVMachine::run()
{
    // Increment the soft timer.
    machine_timer++;
    handle_errors();

    // State Machine
    switch (state) {
        case States::ST_STARTUP:
            state_startup();
            break;
        case States::ST_INSPR:
            state_inspiration();
            break;
        case States::ST_INSPR_HOLD:
            state_inspiration_hold();
            break;
        case States::ST_EXPR:
            state_expiration();
            break;
        case States::ST_PEEP_PAUSE:
            state_peep_pause();
            break;
        case States::ST_EXPR_HOLD:
            state_expiration_hold();
            break;
        case States::ST_ACTUATOR_HOME:
            state_actuator_home();
            break;
        case States::ST_ACTUATOR_JOG:
            state_actuator_jog();
            break;
        case States::ST_FAULT:
            state_fault();
            break;
        case States::ST_DEBUG:
            state_debug();
            break;
        case States::ST_OFF:
            state_off();
            break;
        default:
            break;
    }
}

States PCVMachine::get_last_state()
{
    return lastState;
}

void PCVMachine::setup()
{
    // Initial state with default motor as the stepper motor
    state = States::ST_STARTUP;
    // if(firstInit == true)
    // {
    //     motor = Motors::STEPPER;
    //     firstInit = false;
    // }
    p_alarm_manager->begin();
}

// const char* PCVMachine::get_current_state_string()               // NEED TO FIX LATER, REDEFINITION ERRORS
// {
//     return state_string[(int) state];
// }

// const char** PCVMachine::get_state_list(uint8_t* size)
// {
//     *size = sizeof(state_string) / sizeof(char*);
//     return state_string;
// }

States PCVMachine::get_current_state()
{
    return state;
}

void PCVMachine::change_state(States st)
{
    // Change the state.
    set_state(st);
}

// void Machine::change_motor(Motors m)  // Change motor
// {
//     set_motor(m);
// }

// void Machine::change_mode(ControlModes cm)  // Change motor
// {
//     set_mode(cm);
// }

void PCVMachine::handle_errors()
{
    // These pressure alarms only make sense after homing
    if (state_first_entry && state == States::ST_INSPR) {
        p_alarm_manager->badPlateau(false);
        p_alarm_manager->lowPressure(false);
        p_alarm_manager->noTidalPres(false);
        p_alarm_manager->highPressure(p_gauge_pressure->get_pressure(units_pressure::cmH20) > PRESSURE_MAX);
    }

    p_alarm_manager->update();
}

void PCVMachine::set_fault(Fault id)
{
    // Set the fault_id. It will get picked up at the right states.

    // Service only the actuator fault for now.
    if (id == Fault::FT_ACTUATOR_FAULT) {
        // Set the special debug fault flag
        actuator_force_fault_debug = true;
    }
}