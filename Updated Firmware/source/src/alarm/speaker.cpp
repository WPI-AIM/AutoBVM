
/**
 * Code contains snippets from the MIT Emergency Ventilator Controller
 * poject: https://github.com/mit-drl/e-vent
 * 
 */

#include "speaker.h"

void Speaker::begin()
{
    // snooze_button_.begin();
    pinMode(speaker_pin_, OUTPUT);
}

void Speaker::play(const AlarmLevel& alarm_level)
{
    for (int i = 0; i < NUM_LEVELS; i++) {
        if (i != alarm_level) {
            tones_[i].stop();
        }
    }
    tones_[alarm_level].play();
}

void Speaker::stop()
{
    for (int i = 0; i < NUM_LEVELS; i++) {
        tones_[i].stop();
    }
}

void Speaker::update(const AlarmLevel& alarm_level)
{
    if (snoozeButtonPressed()) {
        toggleSnooze();
    }
    // check if snooze time is up
    if (snoozed_ && millis() - snooze_time_ > kSnoozeTime) {
        snoozed_ = false;
        if (snooze_complete_cb) {
            snooze_complete_cb();
        }
    }
    if (snoozed_) {
        stop();
    }
    else {
        play(alarm_level);
    }
    play(alarm_level);
}

bool Speaker::snoozeButtonPressed()
{
    return snooze_button_;
}

void Speaker::toggleSnooze()
{
    snooze_button_ = false;
    if (snoozed_) {
        snoozed_ = false;

        Serial.println("Snooze false");
    }
    else {
        snoozed_ = true;

        snooze_time_ = millis();
        Serial.println("Snooze true");
    }
}

void Speaker::snooze_set(bool status)
{
    snooze_button_ = status;
}