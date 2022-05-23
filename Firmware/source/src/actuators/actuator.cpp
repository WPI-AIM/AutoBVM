#include "actuator.h"
#include "controls/control.h"
#include "utilities/logging.h"

void Actuator::init()
{
#if USE_AMS_FEEDBACK
    stepper_fb.begin();
#endif
    stepper.init();

    // Enable the drive
    stepper.set_enable(true);

    // Set the current position as home.
    stepper.set_position_as_home(0);
}

void Actuator::run()
{
    stepper.run();
}

void Actuator::set_enable(bool en)
{
    stepper.set_enable(en);
}

void Actuator::set_speed(Tick_Type tt, float speed)
{
    if (tt == Tick_Type::TT_DEGREES) {
        /* Degree to steps calculation.
        * In reference to the actuator pulley,
        * 360 degrees(1 rev) is TIMING_PULLEY_STEPS_PER_REV steps.
        * X degrees is X * TIMING_PULLEY_STEPS_PER_REV/360
        */
        double steps_per_sec = TIMING_PULLEY_DEGREES_TO_STEPS(speed);
        stepper.set_speed(float(steps_per_sec));
    }
    else {
        stepper.set_speed(speed);
    }

    // Store previous position value. Used to detect movement.
    prev_position = get_position();
}

void Actuator::set_position(Tick_Type tt, double value)
{
    if (tt == Tick_Type::TT_DEGREES) {
        /* Degree to steps calculation.
        * In reference to the actuator pulley,
        * 360 degrees is TIMING_PULLEY_STEPS_PER_REV steps.
        * X degrees is TIMING_PULLEY_STEPS_PER_REV/360
        */
        double angle_to_steps = TIMING_PULLEY_DEGREES_TO_STEPS(value);
        stepper.set_position(angle_to_steps);
    }
    else {
        stepper.set_position(value);
    }
}

void Actuator::set_position_relative(Tick_Type tt, double value)
{
    if (tt == Tick_Type::TT_DEGREES) {
        /* Degree to steps calculation.
        * In reference to the actuator pulley,
        * 360 degrees is TIMING_PULLEY_STEPS_PER_REV steps.
        * X degrees is TIMING_PULLEY_STEPS_PER_REV/360
        */
        double angle_to_steps = TIMING_PULLEY_DEGREES_TO_STEPS(value);
        stepper.set_position_relative(angle_to_steps);
    }
    else {
        stepper.set_position_relative(value);
    }
}

void Actuator::set_position_as_home()
{
    stepper.set_position_as_home(0);
}

void Actuator::home()
{
    stepper.home();
}

bool Actuator::is_home()
{
    // double current_position = stepper.get_position();
    double current_position = get_position();

/* Allow a degree of dither to detect home.
     * The stepper motor is setup to perform full steps.
     * The teeth ratio is 60/14, which is 1:4.29
     * which means, 4.29 revs of the stepper is 1 rev of the main wheel.
     * So 1 degree, full step on the stepper would give 1.8 *(1/4.29)
     * = 0.419~0.42 degrees on the wobbler shaft.
     * Check within a degree while homing.
     */
#if USE_AMS_FEEDBACK
    // The feedback sensor has a dither and sometimes rolls over. Add a forward band.
    return (((current_position >= 0.2) && (current_position <= 1.0)));
#else
    // For no feedback, the stepper steps are counted and does not roll over.
    return (((current_position >= 0.0) && (current_position <= 1.0)));
#endif
}

bool Actuator::is_running()
{
    return stepper.is_moving();
}

bool Actuator::target_reached()
{
    return stepper.target_reached();
}

bool Actuator::is_moving()
{
    /* The stepper drive reports movement if steps are being
     * issued. It does not mean the stepper is actually moving.
     * Eg. When steps are incoming, but the power stage is off.
     * Instead use the angle sensor to detect movement.
     */
    double current_position = get_position();

    // Steps are being issued. Check the angle sensor for movement.
    double abs_position = fabs(current_position - prev_position);

    prev_position = current_position;

    /* Look for a change in half a degree.
     * Checks are done every 20ms. If the homing speed is 200 steps/sec,
     * there is a step every 5ms. A full step of the stepper, moves the timing
     * pulley 1.8 *(1/4.29)=0.42 degrees
     * At the 20ms rate, the pulley would have moved (20/5) * 0.42 = 1.68
     * Look for a change of at least 0.5 degrees, since that will 
     * indicate a movement.
     */
    return (abs_position > 0.5);
}

float Actuator::get_current_speed()
{
    // [TODO]
    return 0;
}

double Actuator::get_position()
{
#if USE_AMS_FEEDBACK
    double angle;

    //Guard position query with noInterrupt/Interrupt() guards as the touch interrupt may interfered with position capture. They are on the same bus.
    noInterrupts();
    if (stepper_fb.angleR(angle, U_DEG, true) != -1) {
        // No I2C error
        interrupts();
        return angle;
    }
    else {
        interrupts();
        return 0;
    }
#else
    double current_pos_deg = TIMING_PULLEY_STEPS_TO_DEGREES(stepper.get_current_position());
    if (current_pos_deg > 360.0)
        return (current_pos_deg - 360.0);
    return current_pos_deg;
#endif
}

int8_t Actuator::get_position_raw(double& angle)
{
    return (stepper_fb.angleR(angle, U_RAW, true));
}

double Actuator::degrees_to_volume(C_Stat compliance)
{
    double position = get_position();

    // The actuator can only move to MAX_ACT_POS_DEG, before going back. Limit it here.
    if (position > MAX_ACT_POS_DEG) {
        position = MAX_ACT_POS_DEG;
    }

    double volume = 0.0;

    switch (compliance) {
        case C_Stat::NONE:
            volume = D2V_COEF_A_NO_LUNG * pow(position, 4) + D2V_COEF_B_NO_LUNG * pow(position, 3) + D2V_COEF_C_NO_LUNG * pow(position, 2) + D2V_COEF_D_NO_LUNG * position;
            break;
        case C_Stat::TWENTY:
            volume = D2V_COEF_A_COMP_20 * pow(position, 4) + D2V_COEF_B_COMP_20 * pow(position, 3) + D2V_COEF_C_COMP_20 * pow(position, 2) + D2V_COEF_D_COMP_20 * position;
            break;
        case C_Stat::FIFTY:
            volume = D2V_COEF_A_COMP_50 * pow(position, 4) + D2V_COEF_B_COMP_50 * pow(position, 3) + D2V_COEF_C_COMP_50 * pow(position, 2) + D2V_COEF_D_COMP_50 * position;
            break;
        default:
            // Unknown complaince.
            return -1;
    }

    // Cage to max volume.
    if (volume > (MAX_BAG_VOL_L)) {
        volume = (MAX_BAG_VOL_L);
    }

    return volume;
}

double Actuator::volume_to_degrees(C_Stat compliance, double volume)
{
    double degrees = 0.0;

    // Check if volume requested is within limit
    if (volume < MIN_BAG_VOL_L && volume > MAX_BAG_VOL_L) {
        return -1;
    }

    switch (compliance) {
        case C_Stat::NONE:
            degrees = (V2D_COEF_A_NO_LUNG * pow(volume, 3)) + (V2D_COEF_B_NO_LUNG * pow(volume, 2)) + (V2D_COEF_C_NO_LUNG * volume) + V2D_COEF_D_NO_LUNG;
            break;
        case C_Stat::TWENTY:
            degrees = (V2D_COEF_A_COMP_20 * pow(volume, 3)) + (V2D_COEF_B_COMP_20 * pow(volume, 2)) + (V2D_COEF_C_COMP_20 * volume) + V2D_COEF_D_COMP_20;
            break;
        case C_Stat::FIFTY:
            degrees = (V2D_COEF_A_COMP_50 * pow(volume, 3)) + (V2D_COEF_B_COMP_50 * pow(volume, 2)) + (V2D_COEF_C_COMP_50 * volume) + V2D_COEF_D_COMP_50;
            break;
        default:
            // Unknown compliance
            return -1;
    }

    // The actuator can only move to MAX_ACT_POS_DEG, before going back. Limit it here.
    if (degrees > MAX_ACT_POS_DEG) {
        degrees = MAX_ACT_POS_DEG;
    }

    return degrees;
}

/* Set the current reading of the angle sensor as zero.
 * and return the zero value.
 */
uint16_t Actuator::set_current_position_as_zero()
{
#if USE_AMS_FEEDBACK
    // Zero the zero register first, then write the actual value.
    stepper_fb.zeroRegW(0);
    uint16_t new_zero = stepper_fb.angleRegR();
    stepper_fb.zeroRegW(new_zero);

    return new_zero;
#else
    // Write to stepper position
    stepper.set_position_as_home(0);

    // No feedback used.
    return 0;
#endif
}

/* Sets the zero offset to the angle sensor.
 * This is done at startup.
 */
void Actuator::set_zero_position(uint16_t new_zero)
{
    // Zero the zero register first, then write the actual value.
    stepper_fb.zeroRegW(0);
    stepper_fb.zeroRegW(new_zero);
}

bool Actuator::add_correction()
{
    double current_position = get_position();

    /* Home for the actuator is between HOME_MIN_DEG and HOME_MAX_DEG
     * This is because, leaving the home at 0.0 deg causes, the angle sensor
     * to overflow to 359.*, causing an error with waveform calculation.
     * 
     * Use DEBUG_CORRECTION to see debug statements during correction.
     * 
     * If the actuator is between 0.0 and HOME_MIN_DEG, it needs to be
     * nudged forward(+) in HOME_CORR_MOVE_DEG degrees.
     * 
     * If the actuaor is between HOME_MAX_DEG and HOME_CORRECTION_MAX_DEG,
     * it is out of home range and needs to be nudged back(-)
     * 
     * If the actuaor is between 0.0 and HOME_CORRECTION_MIN_DEG,
     * it is out of home range and needs to be nudged forward(+)
     * ------------------------------------------------------------------------------------------
     *            |                    |          |               |                   |
     *            |                    |          |--Happy Place--|                   |
     *            |                    |          |               |                   |
     * (HOME_CORRECTION_MIN_DEG)       0   (HOME_MIN_DEG)  (HOME_MAX_DEG) (HOME_CORRECTION_MAX_DEG)
     */

    if (current_position >= 0.0 && current_position < HOME_MIN_DEG) {
        /* ------------------------------------------------------------------------------------------
        *            |                    |            |               |                   |
        *            |                    |In this slot|--Happy Place--|                   |
        *            |                    | MOVE---->  |               |                   |
        *            |                    |            |               |                   |
        * (HOME_CORRECTION_MIN_DEG)       0   (HOME_MIN_DEG)  (HOME_MAX_DEG) (HOME_CORRECTION_MAX_DEG)
        */
        set_position_relative(Tick_Type::TT_STEPS, TIMING_PULLEY_DEGREES_TO_STEPS(HOME_CORR_MOVE_DEG));
        set_speed(Tick_Type::TT_STEPS, HOME_CORRECTION_SPEED_DEG_P_SEC);
#if DEBUG_CORRECTION
        serial_printf("Correcting! %0.2f Move +%0.2f\n", current_position, HOME_CORR_MOVE_DEG);
#endif
    }

    if (current_position > HOME_MAX_DEG && current_position <= HOME_CORRECTION_MAX_DEG) {
        /* ------------------------------------------------------------------------------------------
        *            |                    |            |               |                   |
        *            |                    |            |--Happy Place--|   In this slot    |
        *            |                    |            |               | <-----MOVE        |
        *            |                    |            |               |                   |
        * (HOME_CORRECTION_MIN_DEG)       0   (HOME_MIN_DEG)  (HOME_MAX_DEG) (HOME_CORRECTION_MAX_DEG)
        */
        set_position_relative(Tick_Type::TT_STEPS, TIMING_PULLEY_DEGREES_TO_STEPS(-HOME_CORR_MOVE_DEG));
        set_speed(Tick_Type::TT_STEPS, HOME_CORRECTION_SPEED_DEG_P_SEC);
#if DEBUG_CORRECTION
        serial_printf("Correcting! %0.2f Move -%0.2f\n", current_position, HOME_CORR_MOVE_DEG);
#endif
    }

    else if ((current_position > HOME_CORRECTION_MIN_DEG && current_position < 360.0)) {
        /* ------------------------------------------------------------------------------------------
        *            |                    |            |               |                   |
        *            |   In this slot     |            |               |                   |
        *            |       MOVE----->   |            |               |                   |
        *            |                    |            |               |                   |
        * (HOME_CORRECTION_MIN_DEG)       0   (HOME_MIN_DEG)  (HOME_MAX_DEG) (HOME_CORRECTION_MAX_DEG)
        */
        set_position_relative(Tick_Type::TT_STEPS, TIMING_PULLEY_DEGREES_TO_STEPS(HOME_CORR_MOVE_DEG));
        set_speed(Tick_Type::TT_STEPS, HOME_CORRECTION_SPEED_DEG_P_SEC);
#if DEBUG_CORRECTION
        serial_printf("Correcting! %0.2f Move +%0.2f\n", current_position, HOME_CORR_MOVE_DEG);
#endif
    }
    else if (HOME_CORRECTION_MAX_DEG > 5.0 && HOME_CORRECTION_MIN_DEG < 355.0) {
        // Unable to correct
        return false;
    }

    return true;
}

void Actuator::calculate_trajectory(const float& duration_s, const float& goal_pos_deg, float& vel_deg)
{
    // Get the current position of the actuator
    const float cur_pos_deg = (float) get_position();

    // Calculate the distance to move.
    const float distance_deg = abs(goal_pos_deg - cur_pos_deg);

    // Calculate velocity of travel in degrees/second.
    vel_deg = distance_deg / duration_s;

    if (TIMING_PULLEY_DEGREES_TO_STEPS(vel_deg) > STEPPER_MAX_STEPS_PER_SECOND) {
        serial_printf("Max velocity requested! %.2f, clipping to %0.2f!\n", vel_deg, TIMING_PULLEY_STEPS_TO_DEGREES(STEPPER_MAX_STEPS_PER_SECOND));

        // Cap to max velocity
        vel_deg = TIMING_PULLEY_STEPS_TO_DEGREES(STEPPER_MAX_STEPS_PER_SECOND);
    }

#if DEBUG_WAVEFORM
    serial_printf("Pos: %f, Goal:%f, Speed: %f\n", cur_pos_deg, goal_pos_deg, vel_deg);
#endif
}
