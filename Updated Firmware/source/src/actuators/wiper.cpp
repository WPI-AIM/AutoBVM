#include "wiper.h"
#include "../config/uvent_conf.h"
#include "utilities/logging.h"
#include <Arduino.h>

// Initialize the wiper motor.
void Wiper::init()
{

    // Setup wiper relay pins
    pinMode(RELAY1_CONTROL_PIN, OUTPUT);
    pinMode(RELAY2_CONTROL_PIN, OUTPUT);
    
    // Set pins to low to ensure the motor starts unpowered and doesn't move
    digitalWrite(RELAY1_CONTROL_PIN, LOW);
    digitalWrite(RELAY2_CONTROL_PIN, LOW);

}

// void Wiper::set_interval(float value)  // Set timing of wiper rotations with this and set_pause
// {
//     interval = value;
// }

// void Wiper::set_pause(float value)
// {
//     pause = value;
// }

bool Wiper::run()
{   
    // Turn on Relay 1
    if (firstCycle == true)
    {
        digitalWrite(RELAY1_CONTROL_PIN, HIGH);
        time1 = millis();
        relayOn = true;
        firstCycle = false;
    }

    timeDiff = millis() - time1;
    if (timeDiff <= interval && firstLoop == false) // Run the wiper motor
    {
        return false;
    } 
    
    else  // Wiper motor should do last cycle now
    {
        firstLoop = true;

        // Turn off Relay 1
        digitalWrite(RELAY1_CONTROL_PIN, LOW);
        // Turn on Relay 2, wiper will finish current revolution due to park feature
        digitalWrite(RELAY2_CONTROL_PIN, HIGH);

        timeDiff = millis() - time1;

        if (timeDiff <= interval)
        {
            return false;
        }
        
        else  // Turn wiper motor relays off, prepare for next cycle
        {
            digitalWrite(RELAY2_CONTROL_PIN, LOW);
            firstCycle = true;
            firstLoop = false;
            relayOn = false;
            if(doLastCycle == true)
            {
                wiperMotorOn = false;
            }
            return true;

        }
    }
}

bool Wiper::get_relayOn()
{
    return relayOn;
}

bool Wiper::get_wiperMotorOn()
{
    return wiperMotorOn;
}

void Wiper::home()
{
    doLastCycle = true;
}

void Wiper::shutdown()
{
    digitalWrite(RELAY1_CONTROL_PIN, LOW);
    digitalWrite(RELAY2_CONTROL_PIN, LOW);
}

/*        !!!!!!!!!!!!!!!!!!!!!! EVERYTHING BELOW HERE IS FROM STEPPER.H AND CURRENTLY UNEDITED/UNUSED FOR THE WIPER MOTOR !!!!!!!!!!!!!!!!!!!!!!!

// Run the stepper at a set speed
void Wiper::set_speed(float steps_per_second)
{
        To change speed, the external power supply voltage needs to be changed

        In general, wiper motors run at 65 rpm on fast speed and 45 rpm on slow, so 12V = 65 rpm for ours since it's on high speed when it moves

        Maybe can analog read the external voltage somehow? Relay pins are digital so that doesn't work, maybe where the wiper wires are fed into?
        Not sure what pin numbers those are though, need to get wiper wired up right before I can tell for sure I think

        Formula? https://electricscooterparts.com/motor-overvoltage-rpm-calculator.html got me a function APPROXIMATELY like RPM = 5.5 * Voltage
        (could find no specific formula or voltage documentation on our specific Cardone Remaunfactured 40-169 Wiper Motor)

        Changing power supply voltage directly influences motor speed, NOT CODE, but this can be used to get correct RPM for certain pressures, tidal volumes,
        etc, the display can update with the correct values to ensure the speed is correct

    

//    currVolts = analogRead(PIN?);
//    RPM = 5.5 * currVolts;
//    RPM usable for other calculations
}

// void Wiper::home_expiration()
// {
//     // Should be unneeded
// }

bool Wiper::is_moving()
{
    // return stepper_5718L.isRunning();
}

bool Wiper::remaining_steps_to_go()
{
    // No steps, maybe can use cycle time? So 'remaining_time_to_go()'
}

void Wiper::set_position(double steps)
{
    // Not sure if possible
}

void Wiper::set_position_relative(double steps)
{
   // Not sure if possible
}

void Wiper::set_position_as_home(int32_t position)
{
    // Not sure if possible
}

bool Wiper::target_reached()
{
    // Not sure if possible
}

void Wiper::set_enable(bool en)
{
    // A function like this is not needed I don't think
}

uint32_t Wiper::get_current_position()
{
    // Not sure if possible
}

*/