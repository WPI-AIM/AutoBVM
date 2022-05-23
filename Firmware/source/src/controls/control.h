#ifndef UVENT_CONTROL_H
#define UVENT_CONTROL_H

#include <display/screens/screen.h>
#include "controls/machine.h"
#include "interface/interface.h"

/**
 * Set all the adjustable values to their last target, or load defaults if no last target exists.
 * This function will recalculate the waveform.
 */
void init_adjustable_values();

/**
 * Dummy timer function to be used in testing.
 * Functionally should do the same thing as loop_update_readouts, but prints dummy values to the screen.
 * @param timer The LVGL timer that controls this loop. Contains user data with a screen pointer for the cur screen.
 */
void loop_test_readout(lv_timer_t* timer);
/**
 * Timer function to poll sensors and update values accordingly.
 * This should not be called manually under normal circumstances
 * @param timer The LVGL timer that controls this loop. Contains user data with a screen pointer for the cur screen.
 */
void loop_update_readouts(lv_timer_t* timer);

/**
 * Handles showing/hiding the alert box.
 * If the alarm count has changed, calculate the alarms which are now active and use them to set the alarm text.
 * If alarms are zero, set the box display to false
 */
void handle_alerts();

/**
 * Get the target value for a parameter
 * @param type The type of parameter we're searching for
 * @return The target value
 */
double get_control_target(AdjValueType type);
void set_readout(AdjValueType type, double val);
double get_readout(AdjValueType type);

/**
 * Translate a button press on a control to a change in the vent waveform.
 * This function will recalculate the waveform when complete, no other action needs to be taken.
 * @param type The type being adjusted
 * @param new_value The new value to be set
 */
void control_update_waveform_param(AdjValueType type, float new_value);

void control_init();
void control_service();
double control_get_actuator_position();
int8_t control_get_actuator_position_raw(double& angle);
void control_eeprom_write_default();
void control_zero_actuator_position();
void control_write_ventilator_params();
void control_get_serial(char* serial_buffer);
void control_change_state(States);
void control_actuator_manual_move(Tick_Type tt, double angle, double speed);
States control_get_state();
const char* control_get_state_string();
const char** control_get_state_list(uint8_t*);
void control_display_storage();
bool control_is_crc_ok();
double control_get_degrees_to_volume(C_Stat compliance = C_Stat::FIFTY);
double control_get_degrees_to_volume_ml(C_Stat compliance = C_Stat::FIFTY);
double control_calc_volume_to_degrees(C_Stat compliance, double volume);
void control_actuator_set_enable(bool en);
waveform_params* control_get_waveform_params(void);
void control_calculate_waveform();
void control_waveform_display_details();
double control_get_gauge_pressure();
double control_get_diff_pressure();
void control_setup_alarm_cb();
void control_alarm_snooze();
void control_toggle_alarm_snooze();
String control_get_alarm_text();
int16_t control_get_alarm_count();
void control_set_alarm_all_off();
Alarm* control_get_alarm_list();
void control_alarm_test();
void control_set_fault(Fault);
#endif