/* Note that the Arduino due framework does not support the
 * Arduino tone() function.
 * Tone driver is implemented with details from RedRussianBear, whose
 * PR(https://github.com/arduino/ArduinoCore-sam/pull/108)
 * is still waiting acceptance. Attributing here.
 */

#ifndef UVENT_TONE_DRIVER_H
#define UVENT_TONE_DRIVER_H

#include "Arduino.h"

/* _rrb stands for RedRussionBear, whose PR(https://github.com/arduino/ArduinoCore-sam/pull/108)
 * is still waiting acceptance.
 */
void tone_rrb(uint32_t _pin, uint32_t frequency, uint32_t duration = 0);

void noTone_rrb(uint32_t _pin);

#endif /* UVENT_TONE_DRIVER_H */