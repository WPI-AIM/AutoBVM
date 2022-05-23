#include <display/layouts/layouts.h>
#include <controls/machine.h>
#include <controls/control.h>
#include "screen.h"

MainScreen::MainScreen()
        : Screen()
{

    charts[CHART_IDX_FLOW] = SensorChart(
            "Flow (Lpm)",
            FLOW_CHART_MIN_VALUE,
            FLOW_CHART_MAX_VALUE,
            FLOW_CHART_MAX_POINTS,
            FLOW_CHART_REFRESH_TIME,
            FLOW_CHART_LINE_MODE,
            FLOW_CHART_DOT_SIZE,
            FLOW_CHART_LINE_WIDTH
    );

    charts[CHART_IDX_PRESSURE] = SensorChart(
            "Gauge Pressure (cmH2O)",
            GAUGE_PRESSURE_CHART_MIN_VALUE,
            GAUGE_PRESSURE_CHART_MAX_VALUE,
            GAUGE_PRESSURE_CHART_MAX_POINTS,
            GAUGE_PRESSURE_CHART_REFRESH_TIME,
            GAUGE_PRESSURE_CHART_LINE_MODE,
            GAUGE_PRESSURE_CHART_DOT_SIZE,
            GAUGE_PRESSURE_CHART_LINE_WIDTH
    );
}

void MainScreen::init()
{
    Screen::init();
}

/**
 * Setup loads all of the widgets and elements into the already loaded screen.
 * Because of the way LVGL handles adding things, this will most likely not go well if run while another screen is loaded.
 * This is also the place where operations that happen once-per-load can occur. This will be run every time the screen is being set up.
 */
void MainScreen::setup()
{
    Screen::setup();

    setup_readouts();
    setup_controls();

    // FULL SCREEN
    add_dividers();

    // VISUAL_AREA_2
    setup_visual_2();
    generate_charts();

    setup_buttons();
    attach_settings_cb();
}

void MainScreen::attach_settings_cb()
{
    // Add callback to settings button to setup the screen
    lv_obj_t* settings_button_container = SCR_C(CONTROL_AREA_2);
    lv_obj_t* settings_button = lv_obj_get_child(settings_button_container, 0);

    if (settings_button) {
        auto on_settings_check = [](lv_event_t* evt) {
            lv_obj_t* target = lv_event_get_target(evt);
            lv_state_t state = lv_obj_get_state(target);

            // No No
            if (active_floating_window) {
                lv_obj_add_state(target, LV_STATE_CHECKED);
                return;
            }

            if (!lv_obj_has_flag(target, LV_OBJ_FLAG_CHECKABLE)) {
                return;
            }

            auto* screen_ptr = (MainScreen*) evt->user_data;

            if ((state & LV_STATE_CHECKED) != 0) {
                screen_ptr->open_config();
                lv_obj_t* start = get_start_button();
                lv_obj_t* mute = get_mute_button();

                if (start) {
                    lv_obj_add_state(start, LV_STATE_DISABLED);
                }
                if (mute) {
                    lv_obj_add_state(mute, LV_STATE_DISABLED);
                }
            }
            else {
                setup_controls();
                lv_obj_t* start = get_start_button();
                lv_obj_t* mute = get_mute_button();

                if (start && (control_get_state() != States::ST_ACTUATOR_HOME)) {
                    lv_obj_clear_state(start, LV_STATE_DISABLED);
                }
                if (mute) {
                    lv_obj_clear_state(mute, LV_STATE_DISABLED);
                }
            }
        };

        lv_obj_add_event_cb(settings_button, on_settings_check, LV_EVENT_VALUE_CHANGED, this);
    }
}

void MainScreen::open_config()
{
    setup_config_window();
}

void MainScreen::generate_charts()
{
    lv_obj_t* screen_container = SCR_C(VISUAL_AREA_2);
    lv_obj_t* chart_container = lv_obj_get_child(screen_container, 0);

    charts[CHART_IDX_FLOW].generate_chart(chart_container, FLOW);
    charts[CHART_IDX_PRESSURE].generate_chart(chart_container, CUR_PRESSURE);
}

const SensorChart* MainScreen::get_chart(uint8_t idx)
{
    if (idx >= MAIN_SCREEN_CHART_COUNT) {
        return nullptr;
    }
    return &charts[idx];
}

void MainScreen::try_refresh_charts()
{
    for (auto& chart : charts) {

        // Chart operations have their own timers as well,
        // refreshing the large portions of the screen can be expensive
        if (chart.should_refresh()) {
            chart.refresh_chart();
        }
    }
}

