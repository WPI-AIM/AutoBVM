#include <utilities/waves.h>
#include <utilities/logging.h>
#include "main_display.h"

lv_obj_t* containers[DisplayContainer::CONTAINER_COUNT];
lv_style_t container_styles[DisplayContainer::CONTAINER_COUNT];
lv_style_t component_styles[ComponentType::COMPONENT_COUNT];

const lv_color_t palette_color_1 = LV_COLOR_MAKE(109, 68, 197);
const lv_color_t palette_color_2 = LV_COLOR_MAKE(59, 125, 185);
lv_color_t color_start_button = LV_COLOR_MAKE(49, 230, 133);
lv_color_t color_button_alert = LV_COLOR_MAKE(255, 172, 53);
lv_color_t color_text_disabled = LV_COLOR_MAKE(130, 130, 130);
lv_color_t color_gray = LV_COLOR_MAKE(248, 248, 248);

lv_point_t divider_1_points[] = {
        {200, 15},
        {200, 465}
};
lv_point_t divider_2_points[] = {
        {600, 0},
        {600, 480}
};

ButtonData button_type_data[ButtonType::BUTTON_TYPE_COUNT];

void init_styles()
{
    lv_style_t* style;
    // Style for main containers
    for (auto& i : container_styles) {
        style = &i;
        lv_style_init(style);
        lv_style_remove_prop(style, LV_STYLE_BG_COLOR);
        lv_style_set_border_width(style, 0 px);
        lv_style_set_border_color(style, lv_color_black());
        lv_style_set_outline_width(style, 0 px);
        lv_style_set_pad_all(style, 0 px);
        lv_style_set_radius(style, 0 px);
    }

    // Style for components
    for (auto& i : component_styles) {
        style = &i;
        lv_style_init(style);
        lv_style_remove_prop(style, LV_STYLE_BG_COLOR);
        lv_style_set_border_width(style, 0 px);
        lv_style_set_border_color(style, lv_color_black());
        lv_style_set_outline_width(style, 0 px);
        lv_style_set_pad_all(style, 0 px);
        lv_style_set_radius(style, 0 px);
    }
}

void add_dividers()
{
    lv_obj_t* line = lv_line_create(lv_scr_act());
    lv_obj_add_flag(line, LV_OBJ_FLAG_FLOATING);
    lv_line_set_points(line, divider_1_points, 2);
    lv_obj_add_style(line, STYLE_PTR_CM(DIVIDER), LV_PART_MAIN);

    line = lv_line_create(lv_scr_act());
    lv_obj_add_flag(line, LV_OBJ_FLAG_FLOATING);
    lv_line_set_points(line, divider_2_points, 2);
    lv_obj_add_style(line, STYLE_PTR_CM(DIVIDER), LV_PART_MAIN);
}