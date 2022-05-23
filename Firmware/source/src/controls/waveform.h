#ifndef UVENT_WAVEFORM_H
#define UVENT_WAVEFORM_H

#include "../config/uvent_conf.h"

struct waveform_params {
    float tCycleTimer;// Absolute time (s) at start of each breathing cycle
    float tIn;        // Calculated time (s) since tCycleTimer for end of IN_STATE
    float tHoldIn;    // Calculated time (s) since tCycleTimer for end of HOLD_IN_STATE
    float tEx;        // Calculated time (s) since tCycleTimer for end of EX_STATE
    float tPeriod;    // Calculated time (s) since tCycleTimer for end of cycle

    uint16_t bpm;         // Breaths per minute
    float volume_ml;      // Tidal volume
    float ie_i;           // Inspiration of IE ratio
    float ie_e;           // Expiration of IE ratio
    uint16_t pip;         // Peak inspiratory pressure (cmH2O)
    uint16_t peep;        // Peak end expiratory pressure (cmH2O)
    uint16_t plateau_time;// Plateau(time to hold after inspiration) in ms

    // Measured
    float m_pip;          // Measured pip
    float m_peep;         // Measured peep
    float m_plateau_press;// Measured plateau pressure
    float m_rr;           // Measured respiration rate as breaths per minute
    float m_ie_i;         // Measured I of IE ratio
    float m_ie_e;         // Measured E of IE ratio
    float m_tidal_volume; // Estimated volume during inspiration.
};

class Waveform {
public:
    waveform_params* get_params();
    int8_t calculate_waveform();
    bool is_inspiration_done();
    bool is_inspiration_hold_done();
    bool is_expiration_done();
    bool is_peep_pause_done();
    void display_details() const;
    void set_pip_peak_and_reset();
    void set_current_pip(float pip_value);
    void reset_measured_params();
    void calculate_respiration_rate();
    void mark_inspiration_time(float now);
    void mark_expiration_time(float now);
    void calculate_current_parameters();

private:
    const float MIN_PEEP_PAUSE = 0.05;// Time (s) to pause after exhalation / before watching for an assisted inhalation

    float m_pip_peak;        // Max of pip.
    float current_m_pip_peak;// Keeps track of max measured pip in a breath cycle.
    float inspiration_time;
    float expiration_time;

    waveform_params params = {
            .tCycleTimer = 0,
            .tIn = 0.0,
            .tHoldIn = 0.0,
            .tEx = 0.0,
            .tPeriod = 0.0,
            .bpm = DEF_BPM,
            .volume_ml = DEF_BAG_VOL_ML,
            .ie_i = DEF_IE,
            .ie_e = DEF_IE,
            .pip = DEF_PIP,
            .peep = DEF_PEEP,
            .plateau_time = DEF_PLATEAU};
};
#endif//UVENT_WAVEFORM_H