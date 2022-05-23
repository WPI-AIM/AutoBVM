#include <Arduino.h>
#include "utilities/util.h"
#include "utilities/logging.h"
#include "waveform.h"

waveform_params* Waveform::get_params(void)
{
    return &params;
}

int8_t Waveform::calculate_waveform()
{
    // Store the current time
    params.tCycleTimer = now_s();

    // Seconds in each breathing cycle period
    params.tPeriod = 60 / params.bpm;

    // Calculate the total inspiration time, including holdin.
    params.tHoldIn = (params.ie_i * params.tPeriod) / (params.ie_i + params.ie_e);

    // Calculate the time for the paddle to profile an inspiration.
    params.tIn = params.tHoldIn - (params.plateau_time / 1000.0);// plateau time is in milliseconds.

    // The remaining is expiration.
    params.tEx = params.tPeriod - params.tHoldIn;

    if ((params.tIn < 0.0) || (params.tEx < 0.0)) {
        // tIn Cannot be negative.
        return -1;
    }

    return 0;
}

bool Waveform::is_inspiration_done()
{
    return ((now_s() - params.tCycleTimer) > params.tIn);
}

bool Waveform::is_inspiration_hold_done()
{
    return ((now_s() - params.tCycleTimer) > params.tHoldIn);
}

bool Waveform::is_expiration_done()
{
    return ((now_s() - params.tCycleTimer) > params.tPeriod);
}

bool Waveform::is_peep_pause_done()
{
    return ((now_s() - params.tCycleTimer) > (params.tEx + MIN_PEEP_PAUSE));
}

void Waveform::display_details() const
{
    serial_printf("----Waveform Details----\n");
    serial_printf("tPeriod:\t %0.2f\n", params.tPeriod);
    serial_printf("tHoldIn:\t %0.2f\n", params.tHoldIn);
    serial_printf("tIn:\t\t %0.2f\n", params.tIn);
    serial_printf("tEx:\t\t %0.2f\n", params.tEx);
    serial_printf("bpm:\t\t %d\n", params.bpm);
    serial_printf("ie:\t\t %0.1f:%0.1f\n", params.ie_i, params.ie_e);
    serial_printf("Vt:\t\t %0.1f\n", params.volume_ml);
    serial_printf("pip:\t\t %d\n", params.pip);
    serial_printf("peep:\t\t %d\n", params.peep);
    serial_printf("plateau:\t %d\n", params.plateau_time);
}

void Waveform::set_pip_peak_and_reset()
{
    m_pip_peak = current_m_pip_peak;
    current_m_pip_peak = 0.0;
}

void Waveform::set_current_pip(float pip_value)
{
    // Set measured pip. Compare the incoming with previous value, to get the max.
    params.m_pip = max(pip_value, current_m_pip_peak);
    current_m_pip_peak = params.m_pip;
}

void Waveform::reset_measured_params()
{
    params.m_pip = 0.0;
    params.m_peep = 0.0;
    params.m_plateau_press = 0.0;
    params.m_rr = 0.0;
    params.m_ie_i = 0.0;
    params.m_ie_e = 0.0;
    params.m_tidal_volume = 0.0;
}

void Waveform::calculate_respiration_rate()
{
    float breath_time_s = now_s() - params.tCycleTimer;

    // Convert to breaths per minute
    params.m_rr = 60.0 / breath_time_s;
}

void Waveform::mark_inspiration_time(float now)
{
    inspiration_time = now - params.tCycleTimer;
}

void Waveform::mark_expiration_time(float now)
{
    // Substract inspiration time.
    expiration_time = now - params.tCycleTimer - inspiration_time;
}

void Waveform::calculate_current_parameters()
{
    // Current respiration rate in breaths per minute
    params.m_rr = round(60.0 / (now_s() - params.tCycleTimer));

    float total_i_e_time = inspiration_time + expiration_time;

    // Calculate slice of each part from the total ie time.
    float i_slice = inspiration_time / total_i_e_time;
    float e_slice = expiration_time / total_i_e_time;

    // Calculated IE ratios change on which time was larger.
    if (expiration_time > inspiration_time) {
        // Current i in ie
        params.m_ie_i = 1;// i_slice / i_slice;

        // Current e in ie
        params.m_ie_e = e_slice / i_slice;
    }
    else {
        // Current i in ie
        params.m_ie_i = i_slice / e_slice;// i_slice / i_slice;

        // Current e in ie
        params.m_ie_e = 1;// e_slice / e_slice;
    }
}