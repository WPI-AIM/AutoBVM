#include "../main_display.h"

CONTAINER_INIT_FUNC(CONTROL_MAIN)
{
    lv_obj_t* container = SCR_C(CONTROL_MAIN);
    SIZEANDALIGN(600, 0, 200, 480, LV_ALIGN_TOP_LEFT);

    // Style init & bind
    lv_style_t* style_ptr = STYLE_PTR_CT(CONTROL_MAIN);
    // Colors & Borders
    lv_style_set_border_color(style_ptr, lv_color_black());
    // Flexbox
    lv_style_set_flex_flow(style_ptr, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_cross_place(style_ptr, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_row(style_ptr, 0 px);
    lv_style_set_layout(style_ptr, LV_LAYOUT_FLEX);
    lv_obj_add_style(container, style_ptr, LV_PART_MAIN);
}

CONTAINER_INIT_FUNC(CONTROL_AREA_1)
{
    lv_obj_t* container = SCR_C(CONTROL_AREA_1);
    SIZEANDALIGN(0, 0, 200, 340, LV_ALIGN_TOP_LEFT);

    // Style init & bind
    lv_style_t* style_ptr = STYLE_PTR_CT(CONTROL_AREA_1);
    // Colors & Borders
    // Flexbox
    lv_style_set_flex_flow(style_ptr, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_cross_place(style_ptr, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_row(style_ptr, 4 px);
    lv_style_set_layout(style_ptr, LV_LAYOUT_FLEX);
    // Inside Flex
    lv_style_set_flex_grow(style_ptr, FLEX_GROW);

    lv_obj_add_style(container, style_ptr, LV_PART_MAIN);
    lv_obj_set_height(container, LV_PCT(100));
}

CONTAINER_INIT_FUNC(CONTROL_AREA_2)
{
    lv_obj_t* container = SCR_C(CONTROL_AREA_2);
    SIZEANDALIGN(0, 340, 200, 140, LV_ALIGN_TOP_LEFT);

    // Style init & bind
    lv_style_t* style_ptr = STYLE_PTR_CT(CONTROL_AREA_2);
    // Colors & Borders
    lv_style_set_pad_all(style_ptr, 8 px);
    // Flexbox
    lv_style_set_flex_flow(style_ptr, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style_ptr, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style_ptr, LV_FLEX_ALIGN_CENTER);
    lv_style_set_layout(style_ptr, LV_LAYOUT_FLEX);

    lv_obj_add_style(container, style_ptr, LV_PART_MAIN);
    lv_obj_set_height(container, LV_SIZE_CONTENT);
}