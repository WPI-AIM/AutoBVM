
#ifndef UVENT_ALARM_H
#define UVENT_ALARM_H

/**
 * Code modified from MIT Emergency Ventilator Controller
 */

#include "speaker.h"

/** 
 * Alarm
 * Keeps track of the state of a specific alarm.
 */
class Alarm {
public:
    Alarm(){};

    Alarm(const String& default_text, const int& min_bad_to_trigger,
          const int& min_good_to_clear, const AlarmLevel& alarm_level);

    // Reset to default state
    void reset();

    // Set the ON value of this alarm, but only turn ON if `bad == true` for at least
    // `min_bad_to_trigger_` consecutive calls with different `seq` and OFF if `bad == false`
    // for at least `min_good_to_clear_` consecutive calls with different `seq`.
    void setCondition(const bool& bad, const unsigned long& seq);

    // Check if this alarm is on
    inline const bool& isON() const { return on_; }

    // Get the text of this alarm
    inline String text() const { return text_; }

    // Get the alarm level of this alarm
    inline AlarmLevel alarmLevel() const { return alarm_level_; }

private:
    String text_;
    uint8_t min_bad_to_trigger_;
    uint8_t min_good_to_clear_;
    AlarmLevel alarm_level_;
    bool on_ = false;
    uint8_t consecutive_bad_ = 0;
    uint8_t consecutive_good_ = 0;
    uint8_t last_bad_seq_ = 0;
    uint8_t last_good_seq_ = 0;
};

// Indices for the different alarms
enum Indices {
    HIGH_PRESSU = 0,
    LOW_PRESSUR,
    BAD_PLATEAU,
    UNMET_VOLUM,
    NO_TIDAL_PR,
    OVER_CURREN,
    MECH_FAILUR,
    NOT_CONFIRM,
    TURNING_OFF,
    NUM_ALARMS
};
/**
 * AlarmManager
 * Manages multple alarms on the same screen space.
 * If there is one alarm on, its text blinks in a designated portion of the screen,
 * if there are more, each one blinks for `kDisplayTime` milliseconds at a time.
 */
class AlarmManager {

    // Time each alarm is displayed if multiple, in milliseconds
    static const unsigned long kDisplayTime = 2 * 1000UL;

public:
    AlarmManager(const int& speaker_pin,
                 unsigned long const* cycle_count) : speaker_(speaker_pin),
                                                     cycle_count_(cycle_count)
    {
        alarms_[HIGH_PRESSU] = Alarm("HIGH PRESSURE", 1, 2, EMERGENCY);
        alarms_[LOW_PRESSUR] = Alarm("LOW PRES DISCONNECT", 1, 1, EMERGENCY);
        alarms_[BAD_PLATEAU] = Alarm("HIGH RESIST PRESSURE", 1, 1, NOTIFY);
        alarms_[UNMET_VOLUM] = Alarm("UNMET TIDAL VOLUME", 1, 1, EMERGENCY);
        alarms_[NO_TIDAL_PR] = Alarm("NO TIDAL PRESSURE", 2, 1, EMERGENCY);
        alarms_[OVER_CURREN] = Alarm("OVER CURRENT FAULT", 1, 2, EMERGENCY);
        alarms_[MECH_FAILUR] = Alarm("MECHANICAL FAILURE", 1, 1, EMERGENCY);
    }

    // Setup during arduino setup()
    void begin();

    // Update alarms, should be called every loop
    void update();

    // Clear all alarms
    void allOff();

    // Pressure too high alarm
    inline void highPressure(const bool& value)
    {
        alarms_[HIGH_PRESSU].setCondition(value, *cycle_count_);
    }

    // Pressure too low alarm
    inline void lowPressure(const bool& value)
    {
        alarms_[LOW_PRESSUR].setCondition(value, *cycle_count_);
    }

    // Bad plateau alarm
    inline void badPlateau(const bool& value)
    {
        alarms_[BAD_PLATEAU].setCondition(value, *cycle_count_);
    }

    // Tidal volume not met alarm
    inline void unmetVolume(const bool& value)
    {
        alarms_[UNMET_VOLUM].setCondition(value, *cycle_count_);
    }

    // Tidal pressure not detected alarm
    inline void noTidalPres(const bool& value)
    {
        alarms_[NO_TIDAL_PR].setCondition(value, *cycle_count_);
    }

    // Current too high alarm
    inline void overCurrent(const bool& value)
    {
        alarms_[OVER_CURREN].setCondition(value, *cycle_count_);
    }

    // Mechanical Failure alarm
    inline void mechanicalFailure(const bool& value)
    {
        alarms_[MECH_FAILUR].setCondition(value, *cycle_count_);
    }

    inline void turningOFF(const bool& value)
    {
        alarms_[TURNING_OFF].setCondition(value, *cycle_count_);
    }

    // Get current state of each alarm
    inline const bool& getHighPressure() { return alarms_[HIGH_PRESSU].isON(); }

    inline const bool& getLowPressure() { return alarms_[LOW_PRESSUR].isON(); }

    inline const bool& getBadPlateau() { return alarms_[BAD_PLATEAU].isON(); }

    inline const bool& getUnmetVolume() { return alarms_[UNMET_VOLUM].isON(); }

    inline const bool& getNoTidalPres() { return alarms_[NO_TIDAL_PR].isON(); }

    inline const bool& getOverCurrent() { return alarms_[OVER_CURREN].isON(); }

    inline const bool& getMechanicalFailure() { return alarms_[MECH_FAILUR].isON(); }

    inline void set_snooze_cb(void (*snooze_cb)())
    {
        speaker_.snooze_complete_cb = snooze_cb;
    }

    void toggle_snooze();
    void snooze();
    void disable_snooze();

    // Get text to display
    String getText() const;

    // Get number of alarms that are ON
    int numON() const;

    inline Alarm* getAlarmList()
    {
        return alarms_;
    }

private:
    Speaker speaker_;

    Alarm alarms_[NUM_ALARMS];
    unsigned long const* cycle_count_;

    // Get highest priority level of the alarms that are ON
    AlarmLevel getHighestLevel() const;
};

#endif//UVENT_ALARM_H