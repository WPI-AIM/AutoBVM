#include "stepper.h"
#include "../config/uvent_conf.h"
#include "utilities/logging.h"
#include <Arduino.h>

/* The 5718L stepper moves counter-clockwise for positive speeds
 * and clockwise for negative speeds.
 */

// Initialize the stepper motor.
void Stepper::init()
{
#if DEBUG_STEPPER
    serial_printf("Stepper Details\n");
    serial_printf("Max speed\t: %d steps per second\n", STEPPER_MAX_STEPS_PER_SECOND);
    serial_printf("Max homing speed: %d steps per second\n", STEPPER_HOMING_SPEED_IN_STEPS_SEC);
    serial_printf("Angle per step\t: %f degrees\n", STEPPER_ANGLE_DEG_PER_STEP);
    serial_printf("Multiplier\t: %d\n", STEPPER_MULTIPLIER_MODE);
    serial_printf("Steps per rev\t: %f\n", STEPPER_STEPS_PER_REV);
    serial_printf("Teeth ratio\t: %d:%d\n", TEETH_ON_PINION, TEETH_ON_TIMING_PULLEY);
    serial_printf("Gear ratio\t: %f\n", GEAR_RATIO);
    serial_printf("Timing pulley, steps per rev: %d\n", uint32_t(TIMING_PULLEY_STEPS_PER_REV));
#endif
    // Setup stepper pins
    pinMode(STEPPER_DIRECTION_PIN, OUTPUT);
    pinMode(STEPPER_STEP_PIN, OUTPUT);
    pinMode(STEPPER_DISABLE_PIN, OUTPUT);

    // Max speed in steps per second.
    stepper_5718L.setMaxSpeed(STEPPER_MAX_STEPS_PER_SECOND);

    // Stop the motor.
    stepper_5718L.setSpeed(0);
}

// Service the Accelstepper driver.
bool Stepper::run()
{
    return stepper_5718L.runSpeedToPosition();
}

// Run the stepper at a set speed
void Stepper::set_speed(float steps_per_second)
{
    stepper_5718L.setSpeed(steps_per_second);
}

void Stepper::home()
{
    /* Move the paddle through a full rev.
     * Home should be within this distance.
     */
    stepper_5718L.move(TIMING_PULLEY_STEPS_PER_REV);
    stepper_5718L.setSpeed(STEPPER_HOMING_SPEED_IN_STEPS_SEC);
}

bool Stepper::is_moving()
{
    return stepper_5718L.isRunning();
}

bool Stepper::remaining_steps_to_go()
{
    return stepper_5718L.distanceToGo();
}

void Stepper::set_position(double steps)
{
    stepper_5718L.moveTo(steps);
}

void Stepper::set_position_relative(double steps)
{
    stepper_5718L.move(steps);
}

void Stepper::set_position_as_home(int32_t position)
{
    stepper_5718L.setCurrentPosition(position);
}

bool Stepper::target_reached()
{
    int32_t distance_to_go = stepper_5718L.distanceToGo();
    return (distance_to_go == 0);
}

void Stepper::set_enable(bool en)
{
    digitalWrite(STEPPER_DISABLE_PIN, !(en));
}

uint32_t Stepper::get_current_position()
{
    return stepper_5718L.currentPosition();
}