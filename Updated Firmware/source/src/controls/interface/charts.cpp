#include <utilities/util.h>
#include <display/main_display.h>
#include "charts.h"

static void on_graph_readout_update(lv_event_t* evt)
{
    lv_obj_t* label = lv_event_get_target(evt);
    auto* value_ptr = (AdjustableValue*) lv_event_get_user_data(evt);
    if (!label || !value_ptr) {
        return;
    }

    double val = *value_ptr->get_value_measured();
    const char* formatter = value_ptr->get_settings().measured_formatter;
    if (strcmp("%ld", formatter) == 0 || strcmp("%d", formatter) == 0 || strcmp("%u", formatter) == 0) {
        lv_label_set_text_fmt(label, formatter, (int32_t) val);
    }
    else {
        lv_label_set_text_fmt(label, formatter, val);
    }
}

SensorChart::SensorChart(const char* name, int32_t min_val, int32_t max_val, uint32_t chart_points,
        uint32_t refresh_time,
        bool use_dots, lv_coord_t dot_size, lv_coord_t line_width)
        : name(name), range_min(min_val), range_max(max_val), chart_points(chart_points), refresh_time(refresh_time),
          use_dots(use_dots), dot_size(dot_size), line_width(line_width) { }

void SensorChart::generate_chart(lv_obj_t* parent, AdjValueType tracked_type = UNKNOWN)
{
    if (chart != nullptr) {
        LV_LOG_ERROR("User attempted to create chart %s that already exists, aborting...", name);
        return;
    }

    lv_obj_t* label = lv_label_create(parent);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_height(label, LV_SIZE_CONTENT);

    chart = lv_chart_create(parent);
    lv_obj_set_flex_grow(chart, FLEX_GROW);
    lv_obj_set_height(chart, LV_PCT(100));
    lv_obj_set_style_border_width(chart, 2 px, LV_PART_MAIN);
    lv_obj_set_style_border_color(chart, lv_color_black(), LV_PART_MAIN);

    if (!use_dots) {
        /*Do not display points on the data*/
        lv_obj_set_style_size(chart, 0 px, LV_PART_INDICATOR);
    }
    else {
        lv_obj_set_style_size(chart, dot_size, LV_PART_INDICATOR);
    }

    lv_obj_set_style_line_width(chart, line_width, LV_PART_ITEMS);

    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, range_min, range_max);
    lv_chart_set_div_line_count(chart, 2, 11); /*Divide the graph area into 10(11 lines) equal parts*/


    /*Add data series*/
    lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_point_count(chart, chart_points);
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
    lv_obj_t* readout_label = lv_label_create(chart);
    lv_obj_set_style_text_font(readout_label, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_label_set_text_fmt(readout_label, "--");
    lv_obj_align(readout_label, LV_ALIGN_TOP_LEFT, 0, -5);

    if (tracked_type < ADJ_VALUE_COUNT) {
        AdjustableValue* value = &adjustable_values[tracked_type];
        value->set_obj_measured(readout_label);
        lv_obj_add_event_cb(readout_label, on_graph_readout_update, LV_EVENT_REFRESH, value);
    }
}

void SensorChart::add_data_point(double data) const
{
    if (!chart) {
        return;
    }
    lv_chart_series_t* series = lv_chart_get_series_next(chart, nullptr);

    uint16_t point_count = lv_chart_get_point_count(chart);
    series->y_points[series->start_point] = data;
    series->start_point = (series->start_point + 1) % point_count;
}

bool SensorChart::should_refresh()
{
    return has_time_elapsed(&last_refreshed, refresh_time);
}

void SensorChart::refresh_chart() const
{
    if (!chart) {
        return;
    }
    lv_chart_refresh(chart);
}
