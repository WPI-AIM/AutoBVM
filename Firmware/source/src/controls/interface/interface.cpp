#include "interface.h"
#include "../config/uvent_conf.h"

static const char* cmh20 = "cmH2O";

/**
 * Adjustable values
 */
AdjustableValue adjustable_values[AdjValueType::ADJ_VALUE_COUNT];

const AdjValueParams adj_value_settings[] = {
        {"Tidal Volume",     "vT",         "%ld",  "%ld",  "mL",    MIN_BAG_VOL_ML, MAX_BAG_VOL_ML,
                                                                                                  DEF_BAG_VOL_ML, 20,  palette_color_1},
        {"Respiration Rate", "Resp. Rate", "%ld",  "%ld",  "/min",  BPM_MIN,        BPM_MAX,      DEF_BPM,        2,   palette_color_1},
        {"PEEP",             "PEEP",       "%.1f", "%ld",  cmh20,   PEEP_MIN,       PEEP_MAX,     DEF_PEEP,       1,   palette_color_1},
        {"PIP",              "PIP",        "%.1f", "%ld",  cmh20,   PIP_MIN,        PIP_MAX,      DEF_PIP,        1,   palette_color_1},
        {"Plateau Time",     "Plateau",    "%.1f", "%ld",  "ms",    PLATEAU_MIN,    PLATEAU_MAX,  DEF_PLATEAU,    50,  palette_color_1},
        {"Plateau Pressure", nullptr,      "%.2f", "%.2f", cmh20,   PEEP_MIN,       PIP_MAX, 5,                   1,   palette_color_1},
        {"I:E Ratio",        "",           "%.1f", "%.1f", nullptr, IE_MIN,         IE_MAX,       DEF_IE,         0.1, palette_color_1},
        {"I:E Ratio",        "",           "%.1f", "%.1f", nullptr, IE_MIN,         IE_MAX,       DEF_IE,         0.1, palette_color_1},
        {"Flow",             "",           "%.1f", "%.1f", "L/m",   FLOW_MIN,       FLOW_MAX,     FLOW_MIN,       0.1, palette_color_1},
        {"Pressure",         "",           "%.2f", "%.2f", "cmH2O", PRESSURE_MIN,   PRESSURE_MAX, PRESSURE_MIN,   0.1, palette_color_1},

};

void AdjustableValue::on_control_button_press(lv_event_t* evt)
{
    // TODO generic on control press code
    if (control_press_cb) {
        control_press_cb(this, evt);
    }
}

void AdjustableValue::on_readout_update(lv_event_t* evt)
{
    // TODO generic on control press code

    // TODO LV_EVENT_REFRESH
    if (readout_update_cb) {
        readout_update_cb(this, evt);
    }
}

void AdjustableValue::refresh_readout()
{
    if (!lv_obj_measured) {
        return;
    }
    lv_event_send(lv_obj_measured, LV_EVENT_REFRESH, this);
}

AdjValueParams AdjustableValue::get_settings() const
{
    if (value_type >= ADJ_VALUE_COUNT) {
        return adj_value_settings[0];
    }
    return adj_value_settings[value_type];
}

double AdjustableValue::get_step() const
{
    return get_settings().step;
}

/**
     * The holder for all the content that displays the sensor-measured value
     * @return The base object
     */
lv_obj_t* AdjustableValue::get_obj_measured()
{
    return lv_obj_measured;
}

/**
 * The holder for all the controls to set the target value for the mechanics
 * @return The base object
 */
lv_obj_t* AdjustableValue::get_obj_target()
{
    return lv_obj_target;
}

AdjustableValue AdjustableValue::set_obj_measured(lv_obj_t* obj)
{
    if (!obj) return *this;
    lv_obj_measured = obj;
    lv_obj_measured->user_data = this;
    return *this;
}

AdjustableValue AdjustableValue::set_obj_target(lv_obj_t* obj)
{
    if (!obj) return *this;
    lv_obj_target = obj;
    lv_obj_target->user_data = this;
    return *this;
}

/**
 * @return The last measured value
 */
double* AdjustableValue::get_value_measured()
{
    return &measured;
}

/**
 * @return The current set target value
 */
double* AdjustableValue::get_value_target()
{
    return &target;
}

AdjustableValue AdjustableValue::set_value_measured(double value)
{
    dirty = true;
    measured = value;
    return *this;
}

AdjustableValue AdjustableValue::set_value_target(double value)
{
    AdjValueParams settings = get_settings();
    target = clamp(value, settings.min_value, settings.max_value);
    return *this;
}
