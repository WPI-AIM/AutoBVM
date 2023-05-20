/* Note that the Arduino due framework does not support the
 * Arduino tone() function.
 * Tone driver is implemented with details from RedRussianBear, whose
 * PR(https://github.com/arduino/ArduinoCore-sam/pull/108)
 * is still waiting acceptance. Attributing here.
 *
 * Modified to work with DueTimer library as other parts
 * of the software use it.
 */

#include "tone_driver.h"
#include <Arduino.h>
#include <DueTimer.h>

static uint32_t last_output_pin = -1;

static volatile Pio* port_pio_registers;
static volatile uint32_t port_bitmask;
static volatile int64_t toggleCount;
static volatile bool tone_is_active = false;

// Timer Counter names, Interrupt Request handlers, channel
#define TONE_TC TC1
#define TONE_TC_IRQn TC5_IRQn
#define TONE_TC_PMC ID_TC5
#define TONE_TC_CHANNEL 2

void tone_interrupt(void)
{
    if (toggleCount != 0) {
        // Toggle the ouput pin
        if (port_pio_registers->PIO_ODSR & port_bitmask) {
            // If high, go low
            port_pio_registers->PIO_CODR = port_bitmask;
        }
        else {
            // Else, go high
            port_pio_registers->PIO_SODR = port_bitmask;
        }

        if (toggleCount > 0)
            toggleCount--;
    }
    else {
        Timer2.stop();
        port_pio_registers->PIO_CODR = port_bitmask;// Take pin low
        tone_is_active = false;
    }
}

void tone_rrb(uint32_t outputPin, uint32_t frequency, uint32_t duration)
{
    // Avoid divide by zero error by calling 'noTone' instead
    if (frequency == 0) {
        noTone_rrb(outputPin);
        return;
    }

    // If swapping pins, disable tone on old pin
    if (tone_is_active && (outputPin != last_output_pin))
        noTone_rrb(last_output_pin);

    // If swapping pins, record and set initial state low
    if (outputPin != last_output_pin) {
        last_output_pin = outputPin;
        digitalWrite(outputPin, LOW);
        pinMode(outputPin, OUTPUT);
        tone_is_active = true;
    }

    // Compute total number of toggles to finish duration, if duration is zero or less set to -1 (indefinite)
    toggleCount = duration > 0 ? duration * frequency / 1000 : -1;

    // Retrieve PIO registers for required output pin, store in globals
    port_pio_registers = g_APinDescription[outputPin].pPort;
    port_bitmask = g_APinDescription[outputPin].ulPin;

    Timer2.attachInterrupt(tone_interrupt);
    Timer2.start(1000);
}

void noTone_rrb(uint32_t outputPin)
{
    // TC_Stop(TONE_TC, TONE_TC_CHANNEL);
    Timer2.stop();
    Timer2.detachInterrupt();
    digitalWrite(outputPin, LOW);
    tone_is_active = false;
}
