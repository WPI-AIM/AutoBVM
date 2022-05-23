#ifndef UVENT_INTERFACE_H
#define UVENT_INTERFACE_H

#include <lvgl.h>
#include <variant.h>
#include <display/main_display.h>

#define clamp(n, low, high) max(low, min(n, high))

#define CHART_IDX_FLOW 0
#define CHART_IDX_PRESSURE 1

typedef enum AdjValueType {
    TIDAL_VOLUME = 0,
    RESPIRATION_RATE,
    PEEP,
    PIP,
    PLATEAU_TIME,
    PLAT_PRESSURE,
    IE_RATIO_LEFT,
    IE_RATIO_RIGHT,
    FLOW,
    CUR_PRESSURE,
    ADJ_VALUE_COUNT,
    UNKNOWN,
} AdjValueType;

typedef struct AdjValueParams {
    AdjValueParams(const char* title, const char* control_title, const char* measured_formatter,
            const char* target_formatter, const char* unit, double min_val, double max_val, double default_value,
            float step, lv_color_t main_color)
            : title(title), control_title(control_title), measured_formatter(measured_formatter),
              target_formatter(target_formatter), unit(unit),
              min_value(min_val), max_value(max_val), default_value(default_value), step(step),
              main_color(main_color) { }

    const char* title;
    const char* control_title;
    const char* measured_formatter;
    const char* target_formatter;
    const char* unit;
    double min_value;
    double max_value;
    double default_value;
    double step;
    lv_color_t main_color;
} AdjValueParams;

extern const AdjValueParams adj_value_settings[ADJ_VALUE_COUNT];

class AdjustableValue {
public:
    AdjustableValue() = default;

    explicit AdjustableValue(double val)
            : target(val) { }

    AdjValueType value_type = UNKNOWN;

    void (* control_press_cb)(AdjustableValue* this_ptr, lv_event_t* evt) = nullptr;
    void (* readout_update_cb)(AdjustableValue* this_ptr, lv_event_t* evt) = nullptr;

    /**
     * Sets up the object for use. Must be called before calling get_settings
     * @param type The on-screen measured type
     */
    inline void init(AdjValueType type)
    {
        value_type = type;
    }

    void refresh_readout();
    void on_control_button_press(lv_event_t* evt);
    void on_readout_update(lv_event_t* evt);
    AdjValueParams get_settings() const;
    double get_step() const;

    /**
     * The holder for all the content that displays the sensor-measured value
     * @return The base object
     */
    lv_obj_t* get_obj_measured();

    /**
     * The holder for all the controls to set the target value for the mechanics
     * @return The base object
     */
    lv_obj_t* get_obj_target();
    AdjustableValue set_obj_measured(lv_obj_t* obj);
    AdjustableValue set_obj_target(lv_obj_t* obj);

    /**
     * @return The last measured value
     */
    double* get_value_measured();

    /**
     * @return The current set target value
     */
    double* get_value_target();
    AdjustableValue set_value_measured(double value);
    AdjustableValue set_value_target(double value);

    inline bool is_selected() const
    {
        return selected;
    }

    inline void set_selected(bool state)
    {
        selected = state;
    }

    inline void toggle_selected()
    {
        set_selected(!selected);
    }

    inline bool is_dirty()
    {
        return dirty;
    }

    inline void clear_dirty()
    {
        dirty = false;
    };

private:
    lv_obj_t* lv_obj_measured = nullptr;
    lv_obj_t* lv_obj_target = nullptr;
    double measured = -1;
    double target = 0;
    /**
     * Only used if this is part of a composite value like I:E Ratio
     */
    bool selected = true;
    bool dirty = false;
};

extern AdjustableValue adjustable_values[AdjValueType::ADJ_VALUE_COUNT];

#endif //UVENT_INTERFACE_H
