#include "../main_display.h"

CONTAINER_INIT_FUNC(VISUAL_MAIN)
{
    lv_obj_t* container = SCR_C(VISUAL_MAIN);
    SIZEANDALIGN(0, 0, 600, 480, LV_ALIGN_TOP_LEFT);

    // Style init & bind
    lv_style_t* style_ptr = STYLE_PTR_CT(VISUAL_MAIN);
    lv_style_set_border_color(style_ptr, lv_color_black());
    lv_obj_add_style(container, style_ptr, LV_PART_MAIN);
}

CONTAINER_INIT_FUNC(VISUAL_AREA_1)
{
    // Container Properties
    lv_obj_t* container = SCR_C(VISUAL_AREA_1);
    SIZEANDALIGN(0, 0, 200, 480, LV_ALIGN_TOP_LEFT);

    // Style init & bind
    lv_style_t* style_ptr = STYLE_PTR_CT(VISUAL_AREA_1);
    // Colors & Borders
    lv_style_set_bg_color(style_ptr, color_gray);
    lv_style_set_pad_all(style_ptr, 10 px);
    // Flexbox
    lv_style_set_flex_flow(style_ptr, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style_ptr, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_style_set_flex_cross_place(style_ptr, LV_FLEX_ALIGN_CENTER);
    lv_style_set_layout(style_ptr, LV_LAYOUT_FLEX);

    lv_obj_add_style(container, style_ptr, LV_PART_MAIN);
}

CONTAINER_INIT_FUNC(VISUAL_G_C)
{
    lv_obj_t* container = SCR_C(VISUAL_G_C);
    SIZEANDALIGN(200, 0, 400, 480, LV_ALIGN_TOP_LEFT);

    // Style init & bind
    lv_style_t* style_ptr = STYLE_PTR_CT(VISUAL_G_C);
    lv_style_set_border_color(style_ptr, lv_color_black());
    lv_obj_add_style(container, style_ptr, LV_PART_MAIN);
}

CONTAINER_INIT_FUNC(VISUAL_AREA_2)
{
    lv_obj_t* container = SCR_C(VISUAL_AREA_2);
    SIZEANDALIGN(0, 0, 400, 400, LV_ALIGN_TOP_LEFT);

    // Style init & bind
    lv_style_t* style_ptr = STYLE_PTR_CT(VISUAL_AREA_2);
    // Colors & Borders
    lv_style_set_bg_color(style_ptr, color_gray);
    lv_style_set_pad_all(style_ptr, 0 px);
    // Flexbox
    lv_style_set_flex_flow(style_ptr, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_cross_place(style_ptr, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_row(style_ptr, 4 px);
    lv_style_set_layout(style_ptr, LV_LAYOUT_FLEX);

    lv_obj_add_style(container, style_ptr, LV_PART_MAIN);
    lv_obj_set_width(container, LV_PCT(100));
}

CONTAINER_INIT_FUNC(VISUAL_AREA_3)
{
    lv_obj_t* container = SCR_C(VISUAL_AREA_3);
    SIZEANDALIGN(0, 400, 400, 80, LV_ALIGN_TOP_LEFT);

    // Style init & bind
    lv_style_t* style_ptr = STYLE_PTR_CT(VISUAL_AREA_3);
    // Colors & Borders
    lv_style_set_bg_color(style_ptr, color_gray);
    lv_style_set_pad_all(style_ptr, 10 px);
    // Flexbox
    lv_style_set_flex_flow(style_ptr, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_cross_place(style_ptr, LV_FLEX_ALIGN_CENTER);
    lv_style_set_layout(style_ptr, LV_LAYOUT_FLEX);

    lv_obj_add_style(container, style_ptr, LV_PART_MAIN);
}