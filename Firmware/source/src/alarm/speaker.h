/**
 * Code contains snippets from the MIT Emergency Ventilator Controller
 * poject: https://github.com/mit-drl/e-vent
 * 
 */

#ifndef SPEAKER_H_
#define SPEAKER_H_

#include <Arduino.h>
#include "tone.h"

// Alarm levels in order of increasing priority
enum AlarmLevel {
    NO_ALARM,
    NOTIFY,
    EMERGENCY,
    OFF_LEVEL,
    NUM_LEVELS
};

class Speaker {

    // Time during which alarms are silenced, in milliseconds
    // static const unsigned long kSnoozeTime = 2 * 60 * 1000UL;
    static const unsigned long kSnoozeTime = 30 * 1000UL;

public:
    Speaker(const int& speaker_pin)
            : speaker_pin_(speaker_pin)
    {
        const int notify_notes_length = sizeof(kNotifyNotes) / sizeof(kNotifyNotes[0]);
        tones_[NOTIFY] = Tone(kNotifyNotes, notify_notes_length, &speaker_pin_);

        const int emergency_notes_length = sizeof(kEmergencyNotes) / sizeof(kEmergencyNotes[0]);
        tones_[EMERGENCY] = Tone(kEmergencyNotes, emergency_notes_length, &speaker_pin_);

        const int off_notes_length = sizeof(kOffNotes) / sizeof(kOffNotes[0]);
        tones_[OFF_LEVEL] = Tone(kOffNotes, off_notes_length, &speaker_pin_);
    }

    // Setup during arduino setup()
    void begin();

    // Update during arduino loop()
    void update(const AlarmLevel& alarm_level);

    // Snooze control
    void snooze_set(bool status);

    // Callback for snooze complete
    void (* snooze_complete_cb)();

    void toggleSnooze();
private:
    const int speaker_pin_;
    bool snooze_button_;
    Tone tones_[NUM_LEVELS];

    unsigned long snooze_time_ = 0;
    bool snoozed_ = false;

    bool snoozeButtonPressed();

    void play(const AlarmLevel& alarm_level);

    void stop();

};

#endif//SPEAKER_H_