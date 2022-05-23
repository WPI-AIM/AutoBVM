/**
 * Code modified from MIT Emergency Ventilator Controller
 */

#include <Arduino.h>
#include "alarm.h"

Alarm::Alarm(const String& default_text, const int& min_bad_to_trigger,
             const int& min_good_to_clear, const AlarmLevel& alarm_level) : text_(default_text),
                                                                            min_bad_to_trigger_(min_bad_to_trigger),
                                                                            min_good_to_clear_(min_good_to_clear),
                                                                            alarm_level_(alarm_level)
{}

void Alarm::reset()
{
    // Reset all internal alarm parameters.
    on_ = false;
    consecutive_bad_ = 0;
    consecutive_good_ = 0;
    last_bad_seq_ = 0;
    last_good_seq_ = 0;
}

void Alarm::setCondition(const bool& bad, const unsigned long& seq)
{
    if (bad) {
        consecutive_bad_ += (seq != last_bad_seq_);
        last_bad_seq_ = seq;
        if (!on_) {
            on_ = consecutive_bad_ >= min_bad_to_trigger_;
        }
        consecutive_good_ = 0;
    }
    else {
        consecutive_good_ += (seq != last_good_seq_);
        last_good_seq_ = seq;
        if (on_) {
            on_ = consecutive_good_ < min_good_to_clear_;
        }
        consecutive_bad_ = 0;
    }
}

String AlarmManager::getText() const
{
    const int num_on = numON();
    String text = "";
    if (num_on > 0) {
        // determine which of the on alarms to display
        const int index = millis() % (num_on * kDisplayTime) / kDisplayTime;
        int count_on = 0;
        int i;
        for (i = 0; i < NUM_ALARMS; i++) {
            if (alarms_[i].isON()) {
                if (count_on++ == index)
                    break;
            }
        }
        text = alarms_[i].text();
    }
    return text;
}

void AlarmManager::begin()
{
    speaker_.begin();
}

void AlarmManager::update()
{
    // displ_->setAlarmText(getText());
    AlarmLevel highest_level = getHighestLevel();
    speaker_.update(highest_level);
}

void AlarmManager::allOff()
{
    for (int i = 0; i < NUM_ALARMS; i++) {
        alarms_[i].reset();
    }
}

int AlarmManager::numON() const
{
    int num = 0;
    for (int i = 0; i < NUM_ALARMS; i++) {
        num += (int) alarms_[i].isON();
    }
    return num;
}

AlarmLevel AlarmManager::getHighestLevel() const
{
    AlarmLevel alarm_level = NO_ALARM;
    for (int i = 0; i < NUM_ALARMS; i++) {
        if (alarms_[i].isON()) {
            alarm_level = max(alarm_level, alarms_[i].alarmLevel());
        }
    }
    return alarm_level;
}

void AlarmManager::toggle_snooze()
{
    speaker_.toggleSnooze();
}

void AlarmManager::snooze()
{
    speaker_.snooze_set(true);
}

void AlarmManager::disable_snooze()
{
    speaker_.snooze_set(false);
}