#include <controls/control.h>
#include <utilities/logging.h>
#include "layouts.h"

#define CONFIG_BUTTONS_PER_PAGE     4
#define CONFIG_BUTTON_COUNT         8
// Don't add an extra page if we're evenly divisible
#if CONFIG_BUTTON_COUNT % CONFIG_BUTTONS_PER_PAGE == 0
#define CONFIG_PAGES            (CONFIG_BUTTON_COUNT / CONFIG_BUTTONS_PER_PAGE)
#else
#define CONFIG_PAGES            ((CONFIG_BUTTON_COUNT / CONFIG_BUTTONS_PER_PAGE) + 1)
#endif
#define ACTUATOR_TEXT_ENABLED       "Disable Actuator"
#define ACTUATOR_TEXT_DISABLED      "Enable Actuator"

lv_obj_t* active_floating_window = nullptr;
lv_point_t divider_line_points[2];
/************************************************/
/*       Button Registry for Pagination         */
/************************************************/
uint8_t registered_button_cnt = 0;
ButtonCreateFunc button_create_func_list[CONFIG_BUTTON_COUNT];
/************************************************/
/*              Pagination Buffers              */
/************************************************/
uint16_t cur_page_index = 0;
bool buffer_filled[CONFIG_PAGES];
char page_buffers[CONFIG_PAGES][8];
const char* pagination_button_map[] = {LV_SYMBOL_LEFT, "00000000", LV_SYMBOL_RIGHT, ""};

/************************************************/
/*              Static Prototypes               */
/************************************************/

static void open_sensor_select_dialog(lv_event_t* evt);
static void open_about_dialog(lv_event_t* evt);
static void open_reset_eeprom_dialog(lv_event_t* evt, ConfirmChoiceCb confirm_cb);

static void register_button(ButtonCreateFunc func);

/************************************************/
/*               Button Creation                */
/************************************************/

static lv_obj_t* add_config_button(const char* title)
{
    lv_obj_t* button = lv_btn_create(get_config_button_container());
    lv_obj_set_width(button, LV_PCT(100));
    lv_obj_add_style(button, STYLE_PTR_CM(OPTION_BUTTON), LV_PART_MAIN);
    lv_obj_set_style_pad_top(button, 4 px, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(button, 4 px, LV_PART_MAIN);

    lv_obj_t* label = lv_label_create(button);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_border_width(label, 0, LV_PART_MAIN);
    lv_label_set_text_fmt(label, "%s", title);
    lv_obj_set_width(label, LV_PCT(100));
    lv_obj_add_style(label, STYLE_PTR_CM(CONFIG_OPTION_BUTTON_TEXT), LV_PART_MAIN);

    lv_obj_align_to(label, button, LV_ALIGN_CENTER, 0, 0);
    lv_obj_center(label);

    return button;
}

/************************************************/
/*      Buttons & Actions for config screen     */
/*                                              */
/************************************************/

static void add_pressure_select_button()
{

    lv_obj_t* button = add_config_button("Select Sensor");
    auto on_press = [](lv_event_t* evt) {
        Serial.println("Pressed Diff. Sensor Button");
        open_sensor_select_dialog(evt);
    };

    lv_obj_add_event_cb(button, on_press, LV_EVENT_PRESSED, nullptr);
}

static void add_stepper_home_button()
{

    lv_obj_t* button = add_config_button("Home Actuator");

    auto event_cb = [](lv_event_t* evt) {

        auto home_actuator_cb = [](lv_event_t* evt) {
            Serial.println("Homing actuator");
            disable_start_button();
            control_change_state(States::ST_ACTUATOR_HOME);
        };

        open_control_confirm_dialog(evt, home_actuator_cb);
    };
    lv_obj_add_event_cb(button, event_cb, LV_EVENT_RELEASED, nullptr);
}

static void add_actuator_zero_button()
{

    lv_obj_t* button = add_config_button("Zero Actuator");
    auto event_cb = [](lv_event_t* evt) {

        LabelConfigCb label_configurator = [](lv_obj_t* label, uint8_t idx) {
            switch (idx) {
                case 0:
                    lv_label_set_text(label, "WARNING");
                    break;
                case 1:
                    lv_label_set_text(
                            label,
                            "This operation will stop the unit and overwrite values stored in EEPROM.\n"
                            "Are you sure you wish to continue?"
                    );
                    break;
                default:
                    break;
            }
        };

        open_control_confirm_dialog(
                evt,
                [](lv_event_t* evt) {
#if ENABLE_CONTROL
                    control_zero_actuator_position();
#endif
                },
                label_configurator
        );
    };
    lv_obj_add_event_cb(button, event_cb, LV_EVENT_RELEASED, nullptr);
}

static void add_disable_actuator_button()
{

    lv_obj_t* button = add_config_button("Disable Actuator");
    lv_obj_add_flag(button, LV_OBJ_FLAG_CHECKABLE);
    // Set Actuator enabled by default
    lv_obj_add_state(button, LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(button, LV_COLOR_MAKE(175, 175, 175), LV_STATE_CHECKED);
    auto event_cb = [](lv_event_t* evt) {
        lv_obj_t* target = lv_event_get_target(evt);
        if (!target) {
            return;
        }
        lv_obj_t* label = lv_obj_get_child(target, 0);

        bool new_state_enabled = lv_obj_has_state(target, LV_STATE_CHECKED);
        if (new_state_enabled) {
            lv_label_set_text_fmt(label, ACTUATOR_TEXT_ENABLED);
            LV_LOG_USER("Enabling Actuator");
        }
        else {
            lv_label_set_text_fmt(label, ACTUATOR_TEXT_DISABLED);
            LV_LOG_USER("Disabling Actuator");
        }

#if ENABLE_CONTROL
        control_actuator_set_enable(new_state_enabled);
#endif
    };
    lv_obj_add_event_cb(button, event_cb, LV_EVENT_VALUE_CHANGED, nullptr);
}

static void add_dump_eeprom_button()
{

    lv_obj_t* button = add_config_button("Dump EEPROM");

    auto event_cb = [](lv_event_t* evt) {
#if ENABLE_CONTROL
        control_display_storage();
        serial_printf("CRC OK: %s\n", control_is_crc_ok() ? "True" : "False");
#else
        LV_LOG_USER("Dumping EEPROM...");
#endif
    };
    lv_obj_add_event_cb(button, event_cb, LV_EVENT_RELEASED, nullptr);
}

static void add_display_waveform_button()
{

    lv_obj_t* button = add_config_button("Dump Waveform");

    auto event_cb = [](lv_event_t* evt) {
#if ENABLE_CONTROL
        control_waveform_display_details();
#else
        LV_LOG_USER("Dumping Waveform...");
#endif
    };
    lv_obj_add_event_cb(button, event_cb, LV_EVENT_RELEASED, nullptr);
}

static void add_alarm_off_button()
{
    lv_obj_t* button = add_config_button("Alarms Off");

    auto event_cb = [](lv_event_t* evt) {
#if ENABLE_CONTROL
        control_set_alarm_all_off();
#endif
        LV_LOG_USER("Setting all alarms to off...");
    };
    lv_obj_add_event_cb(button, event_cb, LV_EVENT_RELEASED, nullptr);
};

static void add_about_button()
{
    lv_obj_t* button = add_config_button("About");
    lv_obj_add_event_cb(button, open_about_dialog, LV_EVENT_RELEASED, nullptr);
}

static void add_reset_eeprom_button()
{

    lv_obj_t* button = add_config_button("Reset EEPROM");

    auto event_cb = [](lv_event_t* evt) {

        open_reset_eeprom_dialog(
                evt,
                [](lv_event_t* evt) {
#if ENABLE_CONTROL
                    control_eeprom_write_default();
#else
                    LV_LOG_ERROR("EEPROM FORCIBLY RESET.");
#endif
                }
        );

    };

    lv_obj_add_event_cb(button, event_cb, LV_EVENT_RELEASED, nullptr);
}

/************************************************/
/*      Helper functions for Config Styles      */
/*                                              */
/************************************************/

static void quick_flex_obj(lv_obj_t* obj, lv_flex_flow_t flow = LV_FLEX_FLOW_COLUMN, uint8_t grow = 1)
{
    lv_obj_set_style_flex_flow(obj, flow, LV_PART_MAIN);
    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN);
    if (flow == LV_FLEX_FLOW_COLUMN) {
        lv_obj_set_style_pad_row(obj, 4 px, LV_PART_MAIN);
    }
    else {
        lv_obj_set_style_pad_column(obj, 4 px, LV_PART_MAIN);
    }
    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN);
    if (grow) {
        lv_obj_set_style_flex_grow(obj, grow, LV_PART_MAIN);
    }
}

static void default_window_container_styles(lv_obj_t* obj)
{
    lv_obj_set_style_pad_all(obj, 0 px, LV_PART_MAIN);
    lv_obj_set_style_radius(obj, 0 px, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj, 0 px, LV_PART_MAIN);
    lv_obj_set_style_border_color(obj, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_width(obj, LV_PCT(100));
}

static void close_floating_window(lv_obj_t* window)
{
    active_floating_window = nullptr;

    if (!window) {
        return;
    }

    LV_LOG_TRACE("Closing option dialog...");
    lv_obj_del(window);

    lv_obj_t* settings_btn = get_settings_config_button();
    if (settings_btn) {
        lv_obj_clear_state(settings_btn, LV_STATE_DISABLED);
    }
}

static void close_floating_window_evt_cb(lv_event_t* evt)
{
    auto* window = (lv_obj_t*) lv_event_get_user_data(evt);
    close_floating_window(window);
}

/************************************************/
/*      Dialog and option floating screens      */
/*                                              */
/************************************************/

lv_obj_t*
open_yes_no_dialog(const char* title, bool enable_close_button, const char* confirm_text, const char* decline_text,
        ConfirmChoiceCb confirm_cb, WindowConfigCb window_config_cb)
{
    if (active_floating_window) {
        LV_LOG_USER("Can't open a second window");
        return nullptr;
    }
    LV_LOG_TRACE("Opened confirm dialog...");
    // TODO disable start button, settings buttons, etc until home is complete
    lv_obj_t* window = open_option_dialog(title, enable_close_button);
    if (window_config_cb) {
        window_config_cb(window);
    }
    lv_obj_t* main_area = lv_win_get_content(window);
    lv_obj_set_style_pad_left(main_area, 0 px, LV_PART_MAIN);
    lv_obj_set_style_pad_right(main_area, 0 px, LV_PART_MAIN);
    lv_obj_set_style_pad_top(main_area, 4 px, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(main_area, 4 px, LV_PART_MAIN);
    quick_flex_obj(main_area);

    lv_obj_t* content_container = lv_obj_create(main_area);
    quick_flex_obj(content_container);
    default_window_container_styles(content_container);

    lv_obj_t* button_container = lv_obj_create(main_area);
    quick_flex_obj(button_container, LV_FLEX_FLOW_ROW, false);
    default_window_container_styles(button_container);
    lv_obj_set_height(button_container, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_column(button_container, 8 px, LV_PART_MAIN);
    lv_obj_set_style_pad_left(button_container, 4 px, LV_PART_MAIN);
    lv_obj_set_style_pad_right(button_container, 4 px, LV_PART_MAIN);

    if (decline_text) {
        lv_obj_t* no_button = lv_btn_create(button_container);
        lv_obj_set_style_bg_color(no_button, LV_COLOR_MAKE(215, 215, 215), LV_PART_MAIN);
        lv_obj_set_width(no_button, LV_SIZE_CONTENT);
        lv_obj_set_style_flex_grow(no_button, FLEX_GROW, LV_PART_MAIN);
        lv_obj_add_event_cb(no_button, close_floating_window_evt_cb, LV_EVENT_CLICKED, window);
        lv_obj_t* no_label = lv_label_create(no_button);
        lv_label_set_text(no_label, decline_text);
        lv_obj_add_style(no_label, STYLE_PTR_CM(OPTION_BUTTON_TEXT), LV_PART_MAIN);
        lv_obj_align_to(no_label, no_button, LV_ALIGN_RIGHT_MID, 0, 0);
        lv_obj_center(no_label);
    }

    if (confirm_text) {
        lv_obj_t* yes_button = lv_btn_create(button_container);
        lv_obj_set_width(yes_button, LV_SIZE_CONTENT);
        lv_obj_set_style_flex_grow(yes_button, FLEX_GROW, LV_PART_MAIN);
        if (confirm_cb != nullptr) {
            lv_obj_add_event_cb(
                    yes_button,
                    confirm_cb,
                    LV_EVENT_CLICKED,
                    window
            );
        }
        lv_obj_add_event_cb(yes_button, close_floating_window_evt_cb, LV_EVENT_CLICKED, window);

        lv_obj_t* yes_label = lv_label_create(yes_button);
        lv_label_set_text(yes_label, confirm_text);
        lv_obj_set_style_text_align(yes_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_set_style_text_font(yes_label, &lv_font_montserrat_24, LV_PART_MAIN);
        lv_obj_align_to(yes_label, yes_button, LV_ALIGN_CENTER, 0, 0);
        lv_obj_center(yes_label);
    }

    return content_container;
}

void open_control_confirm_dialog(lv_event_t* evt, ConfirmChoiceCb confirm_cb, LabelConfigCb label_config_cb)
{
    lv_obj_t* label_container = open_yes_no_dialog("Confirm Action", true, "Yes", "No", confirm_cb);
    if (!label_container) {
        LV_LOG_WARN("Unable to create confirm/deny window. Another window could be open");
        return;
    }

    lv_obj_t* warning_label_1 = lv_label_create(label_container);
    lv_obj_set_style_text_align(warning_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(warning_label_1, &lv_font_montserrat_28, LV_PART_MAIN);

    // CB to allow for easier configuration of labels
    if (label_config_cb != nullptr) {
        label_config_cb(warning_label_1, 0);
    }
    else {
        lv_label_set_text(warning_label_1, "WARNING");
    }

    lv_obj_t* warning_label_2 = lv_label_create(label_container);
    lv_label_set_long_mode(warning_label_2, LV_LABEL_LONG_WRAP);

    if (label_config_cb != nullptr) {
        label_config_cb(warning_label_2, 1);
    }
    else {
        lv_label_set_text(
                warning_label_2,
                "This operation will completely stop the unit before proceeding.\n"
                "Are you sure you wish to continue?"
        );
    }

    lv_obj_set_style_text_align(warning_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(warning_label_2, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_width(warning_label_2, LV_PCT(100));
}

static void open_sensor_select_dialog(lv_event_t* evt)
{
    lv_obj_t* option_button_container = open_yes_no_dialog("Select Pressure Sensor", false, "Confirm", "Cancel",
            nullptr);
    if (!option_button_container) {
        LV_LOG_WARN("Unable to create confirm/deny window. Another window could be open");
        return;
    }
}

static void open_about_dialog(lv_event_t* evt)
{
    if (active_floating_window) {
        LV_LOG_USER("Can't open a second window");
        return;
    }

    lv_obj_t* window = open_option_dialog("About [WIP]", true);
    lv_obj_set_style_max_width(window, 425 px, LV_PART_MAIN);
    lv_obj_set_style_max_height(window, 250 px, LV_PART_MAIN);

    lv_obj_invalidate(window);
    lv_obj_align_to(window, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* main_area = lv_win_get_content(window);
    lv_obj_set_style_pad_left(main_area, 2 px, LV_PART_MAIN);
    lv_obj_set_style_pad_right(main_area, 4 px, LV_PART_MAIN);
    lv_obj_set_style_pad_top(main_area, 8 px, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(main_area, 4 px, LV_PART_MAIN);
    quick_flex_obj(main_area, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(main_area, 12 px, LV_PART_MAIN);

    lv_obj_t* img_holder = lv_obj_create(main_area);
    default_window_container_styles(img_holder);
    quick_flex_obj(img_holder, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_size(img_holder, LV_SIZE_CONTENT, LV_PCT(100));
    lv_obj_set_style_flex_main_place(img_holder, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_flex_track_place(img_holder, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN);

    lv_obj_t* img = lv_img_create(img_holder);
    lv_img_set_src(img, &be_tm_tagline_logo);
    lv_obj_t* be_label = lv_label_create(img_holder);
    lv_label_set_text(be_label, "Boston\nEngineering");
    lv_obj_set_style_text_font(be_label, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_obj_set_style_text_align(be_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_center(be_label);

    lv_obj_t* be_website = lv_label_create(img_holder);
    lv_label_set_text(be_website, "boston-engineering.com");
    lv_obj_set_style_text_font(be_website, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_align(be_website, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_center(be_website);

    lv_coord_t line_x = lv_obj_get_x2(img_holder);
    divider_line_points[0] = {line_x, 0};
    divider_line_points[1] = {line_x, 150};

    lv_obj_t* line = lv_line_create(main_area);
    lv_line_set_points(line, divider_line_points, 2);
    lv_obj_set_style_line_width(line, 2 px, LV_PART_MAIN);
    lv_obj_set_style_line_color(line, lv_color_black(), LV_PART_MAIN);

    lv_obj_t* text_holder = lv_obj_create(main_area);
    default_window_container_styles(text_holder);
    quick_flex_obj(text_holder, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_size(text_holder, LV_SIZE_CONTENT, LV_PCT(100));
    lv_obj_set_style_flex_track_place(text_holder, LV_FLEX_ALIGN_START, LV_PART_MAIN);

    lv_obj_t* spangroup = lv_spangroup_create(text_holder);
    default_window_container_styles(spangroup);
    lv_spangroup_set_indent(spangroup, 0);
    lv_obj_set_style_text_font(spangroup, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_BREAK);
    lv_spangroup_set_align(spangroup, LV_TEXT_ALIGN_LEFT);

    char buf[13];
    lv_span_t* version_title = lv_spangroup_new_span(spangroup);
    lv_span_set_text(version_title, "Version: ");
    lv_snprintf(buf, 13, "%d.%d.%d\n", UVENT_VERSION_MAJOR, UVENT_VERSION_MINOR, UVENT_VERSION_PATCH);
    lv_span_t* version_number = lv_spangroup_new_span(spangroup);
    lv_span_set_text(version_number, buf);
    lv_style_set_text_font(&version_number->style, &lv_font_montserrat_20);

    lv_span_t* serial_title = lv_spangroup_new_span(spangroup);
    lv_span_set_text(serial_title, "Serial Number: ");
    control_get_serial(buf);
    lv_span_t* serial_number = lv_spangroup_new_span(spangroup);
    lv_span_set_text(serial_number, buf);
    lv_style_set_text_font(&serial_number->style, &lv_font_montserrat_20);

    lv_spangroup_refr_mode(spangroup);
}

static void open_reset_eeprom_dialog(lv_event_t* evt, ConfirmChoiceCb confirm_cb)
{

    auto window_cb = [](lv_obj_t* window) {
        lv_obj_set_style_max_width(window, 550 px, LV_PART_MAIN);
        lv_obj_set_style_max_height(window, 350 px, LV_PART_MAIN);

        lv_obj_invalidate(window);
        lv_obj_update_layout(window);
        lv_obj_align_to(window, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
    };

    lv_obj_t* label_container = open_yes_no_dialog("Confirm Action", true, "Yes", "No", confirm_cb, window_cb);
    if (!label_container) {
        LV_LOG_WARN("Unable to create confirm/deny window. Another window could be open");
        return;
    }

    lv_obj_t* warning_label_1 = lv_label_create(label_container);
    lv_obj_set_style_text_align(warning_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(warning_label_1, &lv_font_montserrat_28, LV_PART_MAIN);

    lv_label_set_text(warning_label_1, "WARNING");

    lv_obj_t* warning_label_2 = lv_label_create(label_container);
    lv_label_set_long_mode(warning_label_2, LV_LABEL_LONG_WRAP);

    lv_label_set_text(
            warning_label_2,
            "This operation will completely overwrite all stored EEPROM values and reset to the default.\n"
            "This shouldn't be attempted unless you are absolutely sure of what you're doing.\n"
            "Are you sure you want to continue?"
    );

    lv_obj_set_style_text_align(warning_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(warning_label_2, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_width(warning_label_2, LV_PCT(100));
}

lv_obj_t* open_option_dialog(const char* title, bool enable_close_button)
{
    lv_obj_t* window = lv_win_create(lv_scr_act(), 60);
    lv_obj_add_style(window, STYLE_PTR_CM(POPUP_WINDOW), LV_PART_MAIN);

    lv_obj_t* label = lv_win_add_title(window, title);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN);

    if (enable_close_button) {
        lv_obj_t* close_button = lv_win_add_btn(window, LV_SYMBOL_CLOSE, 40);
        lv_obj_add_event_cb(close_button, close_floating_window_evt_cb, LV_EVENT_CLICKED, window);
    }

    lv_obj_align_to(window, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
    active_floating_window = window;
    lv_obj_t* settings_btn = get_settings_config_button();
    if (settings_btn) {
        lv_obj_add_state(settings_btn, LV_STATE_DISABLED);
    }
    return window;
}

/************************************************/
/*    Main Config area, right side of screen    */
/*                                              */
/************************************************/

lv_obj_t* get_config_button_container()
{
    lv_obj_t* control_area_1 = SCR_C(CONTROL_AREA_1);
    return lv_obj_get_child(control_area_1, 0);
}

static void create_config_button_container()
{
    lv_obj_t* button_container = lv_obj_create(SCR_C(CONTROL_AREA_1));
    // TODO set up flex and other control parts
    lv_obj_set_width(button_container, LV_PCT(100));
    lv_obj_add_style(button_container, STYLE_PTR_CM(CONFIG_BUTTON_HOLDER), LV_PART_MAIN);
    //lv_obj_set_style_border_width(button_container, 1 px, LV_PART_MAIN);
}

static void register_button(ButtonCreateFunc func)
{
    button_create_func_list[registered_button_cnt++] = func;
}

static void draw_cur_page_buttons()
{
    lv_obj_clean(get_config_button_container());

    for (uint8_t i = 0; i < CONFIG_BUTTONS_PER_PAGE; i++) {
        uint8_t idx = (CONFIG_BUTTONS_PER_PAGE * cur_page_index) + i;
        if (idx >= CONFIG_BUTTON_COUNT) {
            break;
        }
        ButtonCreateFunc button_func = button_create_func_list[idx];
        button_func();
    }
}

static void create_pagination()
{

    for (uint8_t i = 0; i < CONFIG_PAGES; i++) {
        if (buffer_filled[i]) {
            continue;
        }

        buffer_filled[i] = true;
        lv_snprintf(page_buffers[i], 8, "%d / %d", (i + 1), CONFIG_PAGES);
    }

    pagination_button_map[1] = page_buffers[cur_page_index];

    lv_obj_t* pagination_matrix = lv_btnmatrix_create(SCR_C(CONTROL_AREA_1));
    lv_btnmatrix_set_map(pagination_matrix, pagination_button_map);
    lv_obj_add_style(pagination_matrix, STYLE_PTR_CM(PAGINATION_GROUP_HOLDER), LV_PART_MAIN);
    lv_obj_add_style(pagination_matrix, STYLE_PTR_CM(PAGINATION_BUTTON_ITEM), LV_PART_ITEMS);
    lv_obj_add_event_cb(
            pagination_matrix,
            [](lv_event_t* evt) {

                lv_obj_t* obj = lv_event_get_target(evt);
                uint32_t id = lv_btnmatrix_get_selected_btn(obj);
                auto* page_index = (uint16_t*) lv_event_get_user_data(evt);
                uint16_t old_index = *page_index;

                int8_t mod = 0;
                if (id == 0) {
                    mod = -1;
                }
                else if (id == 2) {
                    mod = 1;
                }

                *page_index = constrain((*page_index + mod), 0, (CONFIG_PAGES - 1));
                if (old_index != *page_index) {
                    pagination_button_map[1] = page_buffers[*page_index];
                    lv_btnmatrix_set_map(obj, pagination_button_map);
                    draw_cur_page_buttons();
                }
            },
            LV_EVENT_VALUE_CHANGED,
            &cur_page_index
    );

    lv_btnmatrix_set_btn_width(pagination_matrix, 0, 1);
    lv_btnmatrix_set_btn_width(pagination_matrix, 1, 2);
    lv_btnmatrix_set_btn_width(pagination_matrix, 2, 1);
}

void setup_config_window()
{
    lv_obj_t* parent = SCR_C(CONTROL_AREA_1);
    lv_obj_clean(parent);
    registered_button_cnt = 0;     // Clear registered buttons
    lv_obj_set_style_pad_row(parent, 0 px, LV_PART_MAIN);
    lv_obj_refresh_style(parent, LV_PART_MAIN, LV_STYLE_PROP_ANY);

    create_config_button_container();

    // register_button(add_pressure_select_button);
    register_button(add_stepper_home_button);
    register_button(add_actuator_zero_button);
    register_button(add_disable_actuator_button);
    register_button(add_dump_eeprom_button);
    register_button(add_display_waveform_button);
    register_button(add_alarm_off_button);
    register_button(add_about_button);
    register_button(add_reset_eeprom_button);

    create_pagination();

    draw_cur_page_buttons();
}
