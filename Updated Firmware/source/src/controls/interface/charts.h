#ifndef UVENT_CHARTS_H
#define UVENT_CHARTS_H

#include <lvgl.h>
#include "interface.h"

/**
 * Chart that gets its data from a polled sensor
 */
struct SensorChart {
    const char* name{};
    lv_obj_t* chart = nullptr;
    int32_t range_min{};
    int32_t range_max{};
    uint32_t chart_points{};
    uint32_t refresh_time{};
    uint32_t last_refreshed = 0;
    bool use_dots = false;      /**< Whether or not to use actual points on the data screen, or just a line */
    lv_coord_t dot_size{};
    lv_coord_t line_width{};

    SensorChart() = default;
    SensorChart(const char* name, int32_t min_val, int32_t max_val, uint32_t chart_points, uint32_t refresh_time,
            bool use_dots, lv_coord_t dot_size, lv_coord_t line_width);

    /**
     * Creates the chart based on obj parameters. The chart pointer is stored as `chart`
     *
     * @param parent The parent this chart should be added to
     */
    void generate_chart(lv_obj_t* parent, AdjValueType tracked_type);
    void add_data_point(double data) const;
    /**
     * @return If enough time has passed to allow a refresh of the chart screen
     */
    bool should_refresh();
    void refresh_chart() const;
};

#endif //UVENT_CHARTS_H
