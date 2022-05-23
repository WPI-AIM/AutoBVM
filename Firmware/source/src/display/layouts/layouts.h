#ifndef UVENT_LAYOUTS_H
#define UVENT_LAYOUTS_H

#include "../config/uvent_conf.h"
#include <controls/interface/interface.h>

/************************************************/
/*              Defines and Macros              */
/************************************************/
// abs does some weird stuff when defined by arduino
#undef abs

LV_IMG_DECLARE(be_tm_tagline_logo);

typedef void (* ButtonCreateFunc)();
typedef void (* ConfirmChoiceCb)(lv_event_t* evt);
typedef void (* WindowConfigCb)(lv_obj_t* window);
typedef void (* LabelConfigCb)(lv_obj_t* label, uint8_t label_idx);

lv_obj_t* open_option_dialog(const char* title, bool enable_close_button = false);

/*****************************************************/

// Main Screen
void setup_readouts();
void setup_controls();
void setup_buttons();
void setup_visual_2();

// Main screen readout functions
/**
 * Adds a readout to the left side of the screen
 * @param type The adjustable value type this readout will be tied to
 * @param override_str Any string that should represent the readout value instead of the adjustable value
 */
void setup_adjustable_readout(AdjValueType type, const char* override_str = nullptr);
void setup_adjustable_readout(AdjValueType type, lv_obj_t* parent_container);
void setup_adjustable_readout(AdjValueType type, lv_obj_t* parent_container, const char* override_str);
/**
 * Sets up an adjustable control on the right side of the screen (unless the config screen is open)
 * @param type The type of adjustable readout this will control
 */
void setup_adjustable_control(AdjValueType type);
// I:E Ratio is special
void setup_ie_readout();
void setup_ie_controls();
// Alert Box
void setup_alert_box();
lv_obj_t* get_alert_box();
void set_alert_box_visible(bool visible);
void set_alert_count_visual(uint16_t alert_count);
void set_alert_text(const char* message);
void set_alert_text(String* messages, uint16_t count, uint16_t buf_size);
// Button functions
void add_start_button();
void add_mute_button();
void disable_start_button();
void enable_start_button();
lv_obj_t* get_start_button();
lv_obj_t* get_mute_button();
lv_obj_t* get_settings_config_button();
lv_obj_t* add_settings_button(const char* title, lv_obj_t* parent = SCR_C(CONTROL_AREA_2));
lv_obj_t* add_settings_toggle_button(const char* title, lv_obj_t* parent = SCR_C(CONTROL_AREA_2));

/*****************************************************/

extern lv_obj_t* active_floating_window;

/************************************************/
/*            Prototypes for Windows            */
/************************************************/
lv_obj_t*
open_yes_no_dialog(const char* title, bool enable_close_button, const char* confirm_text, const char* decline_text,
        ConfirmChoiceCb confirm_cb, WindowConfigCb window_config_cb = nullptr);

void open_control_confirm_dialog(lv_event_t* evt, ConfirmChoiceCb confirm_cb, LabelConfigCb label_config_cb = nullptr);

// Config Screen
lv_obj_t* get_config_button_container();
void setup_config_window();

#endif //UVENT_LAYOUTS_H
