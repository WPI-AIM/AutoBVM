#include "../main_display.h"

/**
* ComponentType::READOUT
*/
STYLE_INIT_FUNC(READOUT)
{
    lv_style_t* style = STYLE_PTR_CM(READOUT);
    // Colors & Borders
    lv_style_set_radius(style, 8 px);
    // lv_style_set_bg_color(style, palette_color_1);
    lv_style_set_bg_opa(style, LV_OPA_COVER);
    lv_style_set_pad_all(style, 0 px);
    // Shadow
    lv_style_set_shadow_width(style, 8 px);
    lv_style_set_shadow_color(style, lv_color_black());
    lv_style_set_shadow_ofs_x(style, 4 px);
    lv_style_set_shadow_ofs_y(style, 4 px);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_START);
    lv_style_set_pad_column(style, 0 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    // Inside Flex
    lv_style_set_flex_grow(style, FLEX_GROW);
}

/**
* ComponentType::READOUT_NAME_CONTAINER
*/
STYLE_INIT_FUNC(READOUT_NAME_CONTAINER)
{
    lv_style_t* style = STYLE_PTR_CM(READOUT_NAME_CONTAINER);
    // Colors & Borders
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    lv_style_set_pad_left(style, 2 px);
    lv_style_set_pad_right(style, 0 px);
    lv_style_set_pad_top(style, 0 px);
    lv_style_set_pad_bottom(style, 4 px);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_END);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_START);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    // Inside Flex
    lv_style_set_flex_grow(style, FLEX_GROW);
}

/**
* ComponentType::READOUT_NAME_TEXT
*/
STYLE_INIT_FUNC(READOUT_NAME_TEXT)
{
    lv_style_t* style = STYLE_PTR_CM(READOUT_NAME_TEXT);
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Font Properties
    lv_style_set_text_color(style, lv_color_white());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_LEFT);
    lv_style_set_text_font(style, &lv_font_montserrat_18);
    // Flexbox
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
}

/**
* ComponentType::READOUT_VALUE_CONTAINER
*/
STYLE_INIT_FUNC(READOUT_VALUE_CONTAINER)
{
    lv_style_t* style = STYLE_PTR_CM(READOUT_VALUE_CONTAINER);
    // Sizing
    lv_style_set_min_width(style, LV_PCT(35));
    // Colors & Borders
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    lv_style_set_pad_left(style, 0 px);
    lv_style_set_pad_right(style, 2 px);
    lv_style_set_pad_top(style, 2 px);
    lv_style_set_pad_bottom(style, 2 px);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_END);
    lv_style_set_flex_track_place(style, LV_FLEX_ALIGN_END);
    lv_style_set_pad_row(style, 0 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
}

/**
* ComponentType::READOUT_VALUE_UNIT_TEXT
*/
STYLE_INIT_FUNC(READOUT_VALUE_UNIT_TEXT)
{
    lv_style_t* style = STYLE_PTR_CM(READOUT_VALUE_UNIT_TEXT);
    // Size and color
    lv_style_set_width(style, LV_PCT(100));
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Font Properties
    lv_style_set_text_color(style, lv_color_white());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_RIGHT);
    lv_style_set_text_font(style, &lv_font_montserrat_16);
    // Flexbox
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
}

/**
* ComponentType::READOUT_VALUE_AMOUNT_TEXT
*/
STYLE_INIT_FUNC(READOUT_VALUE_AMOUNT_TEXT)
{
    lv_style_t* style = STYLE_PTR_CM(READOUT_VALUE_AMOUNT_TEXT);
    // Size and color
    lv_style_set_width(style, LV_PCT(100));
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Font Properties
    lv_style_set_text_color(style, lv_color_white());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_RIGHT);
    lv_style_set_text_font(style, &lv_font_montserrat_38);
    // Flexbox
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
}

/**
* ComponentType::OPTION_BUTTON
*/
STYLE_INIT_FUNC(OPTION_BUTTON)
{
    lv_style_t* style = STYLE_PTR_CM(OPTION_BUTTON);
    // Colors & Borders
    lv_style_set_bg_color(style, LV_COLOR_MAKE(215, 215, 215));
    lv_style_set_radius(style, 8 px);
    // Inside Flex
    lv_style_set_flex_grow(style, FLEX_GROW);
}

/**
* ComponentType::OPTION_BUTTON_TEXT
*/
STYLE_INIT_FUNC(OPTION_BUTTON_TEXT)
{
    lv_style_t* style = STYLE_PTR_CM(OPTION_BUTTON_TEXT);
    // Font Properties
    lv_style_set_text_color(style, lv_color_black());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(style, &lv_font_montserrat_24);
}

/**
* ComponentType::CONFIG_OPTION_BUTTON_TEXT
*/
STYLE_INIT_FUNC(CONFIG_OPTION_BUTTON_TEXT)
{
    lv_style_t* style = STYLE_PTR_CM(CONFIG_OPTION_BUTTON_TEXT);
    // Font Properties
    lv_style_set_text_color(style, lv_color_black());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(style, DEFAULT_CONFIG_BUTTON_FONT);
}

/**
* ComponentType::CONTROL
*/
STYLE_INIT_FUNC(CONTROL)
{
    lv_style_t* style = STYLE_PTR_CM(CONTROL);
    // Colors & Borders
    lv_style_set_bg_opa(style, LV_OPA_COVER);
    lv_style_set_pad_all(style, 5 px);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_column(style, 2 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    // Inside Flex
    lv_style_set_flex_grow(style, FLEX_GROW);
}

/**
* ComponentType::CONTROL_TEXT_CONTAINER
*/
STYLE_INIT_FUNC(CONTROL_TEXT_CONTAINER)
{
    lv_style_t* style = STYLE_PTR_CM(CONTROL_TEXT_CONTAINER);
    // Colors & Borders
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_row(style, 0 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    // Inside Flex
    lv_style_set_flex_grow(style, FLEX_GROW);
}

/**
* ComponentType::CONTROL_TEXT_CONTAINER_TOP
*/
STYLE_INIT_FUNC(CONTROL_TEXT_CONTAINER_TOP)
{
    lv_style_t* style = STYLE_PTR_CM(CONTROL_TEXT_CONTAINER_TOP);
    // Colors & Borders
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_END);
    lv_style_set_pad_column(style, 4 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_width(style, LV_SIZE_CONTENT);
    lv_style_set_height(style, LV_SIZE_CONTENT);
}

/**
* ComponentType::CONTROL_TEXT_CONTAINER_IE_TOP
*/
STYLE_INIT_FUNC(CONTROL_TEXT_CONTAINER_IE_TOP)
{
    lv_style_t* style = STYLE_PTR_CM(CONTROL_TEXT_CONTAINER_IE_TOP);
    // Colors & Borders
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_column(style, 6 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_width(style, LV_SIZE_CONTENT);
    lv_style_set_height(style, LV_SIZE_CONTENT);
//    lv_style_set_height(style,LV_PCT(100));
}

/**
* ComponentType::CONTROL_TEXT_VALUE
*/
STYLE_INIT_FUNC(CONTROL_TEXT_VALUE)
{
    lv_style_t* style = STYLE_PTR_CM(CONTROL_TEXT_VALUE);
    // Font Properties
    lv_style_set_text_color(style, lv_color_black());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(style, &lv_font_montserrat_28);
}

/**
* ComponentType::CONTROL_TEXT_NAME
*/
STYLE_INIT_FUNC(CONTROL_TEXT_NAME)
{
    lv_style_t* style = STYLE_PTR_CM(CONTROL_TEXT_NAME);
    // Font Properties
    lv_style_set_text_color(style, lv_color_black());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(style, &lv_font_montserrat_18);
}

/**
* ComponentType::CONTROL_BUTTON
*/
STYLE_INIT_FUNC(CONTROL_BUTTON)
{
    lv_style_t* style = STYLE_PTR_CM(CONTROL_BUTTON);
    // Colors & Borders
    lv_style_set_bg_color(style, palette_color_1);
    lv_style_set_radius(style, 50 px);
    lv_style_set_min_width(style, 45 px);
    lv_style_set_min_height(style, 45 px);
}

/**
* ComponentType::CONTROL_BUTTON_TEXT
*/
STYLE_INIT_FUNC(CONTROL_BUTTON_TEXT)
{
    lv_style_t* style = STYLE_PTR_CM(CONTROL_BUTTON_TEXT);
    // Font Properties
    lv_style_set_text_color(style, lv_color_white());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(style, &lv_font_montserrat_24);
}

/**
* ComponentType::DIVIDER
*/
STYLE_INIT_FUNC(DIVIDER)
{
    lv_style_t* style = STYLE_PTR_CM(DIVIDER);
    // Line properties
    lv_style_set_line_width(style, 2 px);
    lv_style_set_line_color(style, lv_color_black());
    lv_style_set_line_rounded(style, true);
}

/**
* ComponentType::POPUP_WINDOW
*/
STYLE_INIT_FUNC(POPUP_WINDOW)
{
    lv_style_t* style = STYLE_PTR_CM(POPUP_WINDOW);

    // Colors & Borders
    lv_style_set_max_width(style, 500 px);
    lv_style_set_max_height(style, 250 px);
    //lv_style_set_min_height(style, 100 px);
    //lv_style_set_height(style, LV_SIZE_CONTENT);
    lv_style_set_outline_width(style, 4);
    lv_style_set_outline_color(style, lv_color_black());

    // Shadow
    lv_style_set_shadow_width(style, 8 px);
    lv_style_set_shadow_color(style, lv_color_black());
    lv_style_set_shadow_ofs_x(style, 4 px);
    lv_style_set_shadow_ofs_y(style, 4 px);
}

/**
* ComponentType::CONFIG_BUTTON_HOLDER
*/
STYLE_INIT_FUNC(CONFIG_BUTTON_HOLDER)
{
    lv_style_t* style = STYLE_PTR_CM(CONFIG_BUTTON_HOLDER);

    // Colors & Borders
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_all(style, 10 px);
    lv_style_set_pad_row(style, 10 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    // Inside Flex
    lv_style_set_flex_grow(style, FLEX_GROW);
}

/**
* ComponentType::PAGINATION_GROUP_HOLDER
*/
STYLE_INIT_FUNC(PAGINATION_GROUP_HOLDER)
{
    lv_style_t* style = STYLE_PTR_CM(PAGINATION_GROUP_HOLDER);

    // Colors & Borders
    lv_style_set_pad_gap(style, 0);
    lv_style_set_clip_corner(style, true);
//    lv_style_set_radius(style, LV_RADIUS_CIRCLE);
    lv_style_set_radius(style, 0 px);
    lv_style_set_max_height(style, 50 px);
    lv_style_set_width(style, LV_PCT(100));
}

/**
* ComponentType::PAGINATION_BUTTON_ITEM
*/
STYLE_INIT_FUNC(PAGINATION_BUTTON_ITEM)
{
    lv_style_t* style = STYLE_PTR_CM(PAGINATION_BUTTON_ITEM);

    // Colors & Borders
    lv_style_set_text_font(style, &lv_font_montserrat_20);
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, LV_OPA_50);
    lv_style_set_border_color(style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_border_side(style, LV_BORDER_SIDE_INTERNAL);
    lv_style_set_radius(style, 0);
}

/**
* ComponentType::CHART_HOLDER
*/
STYLE_INIT_FUNC(CHART_HOLDER)
{
    lv_style_t* style = STYLE_PTR_CM(CHART_HOLDER);

    lv_coord_t min_max_height = 250 px;

    // Size
    lv_style_set_pad_left(style, 10 px);
    lv_style_set_pad_right(style, 10 px);
    lv_style_set_pad_top(style, 10 px);
    lv_style_set_max_height(style, min_max_height);
    lv_style_set_min_height(style, min_max_height);
    lv_style_set_width(style, LV_PCT(100));
    // Colors & Borders
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    //lv_style_set_border_width(style, 1 px);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_START);
    lv_style_set_pad_row(style, 4 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    // Inside Flex
    lv_style_set_flex_grow(style, FLEX_GROW);
}

/**
* ComponentType::ALERT_BOX
*/
STYLE_INIT_FUNC(ALERT_BOX)
{
    lv_style_t* style = STYLE_PTR_CM(ALERT_BOX);

    // Size
    lv_style_set_max_height(style, 60 px);
    lv_style_set_width(style, LV_PCT(100));
    // Colors & Borders
    lv_style_set_bg_color(style, lv_color_white());
//    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    lv_style_set_border_width(style, 1 px);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_END);
    lv_style_set_flex_track_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_all(style, 5 px);
    lv_style_set_pad_row(style, 4 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
}

/**
* ComponentType::SPACER
*/
STYLE_INIT_FUNC(SPACER)
{
    lv_style_t* style = STYLE_PTR_CM(SPACER);

    // Size
    lv_style_set_width(style, LV_PCT(100));

    // Colors & Borders

    // Debug in case we need to visualize the spacers
    // lv_style_set_bg_color(style, LV_COLOR_MAKE(0x00, 0xff, 0xaa));
    // lv_style_set_border_width(style, 1 px);

    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_START);
    lv_style_set_pad_all(style, 0 px);
    lv_style_set_pad_column(style, 0 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    // Inside Flex
    lv_style_set_flex_grow(style, 2);
}

STYLE_INIT_FUNC(READOUT_EXTRA)
{
    lv_style_t* style = STYLE_PTR_CM(READOUT_EXTRA);

    // Size
    lv_style_set_width(style, LV_PCT(100));
    lv_style_set_max_height(style, 90 px);
    lv_style_set_height(style, LV_PCT(100));

    // Colors & Borders

    // Debug in case we need to visualize the spacers
    // lv_style_set_bg_color(style, LV_COLOR_MAKE(0x00, 0xff, 0xaa));
    // lv_style_set_border_width(style, 1 px);

    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_START);
    lv_style_set_pad_all(style, 4 px);
    lv_style_set_pad_left(style, 6 px);
    lv_style_set_pad_right(style, 6 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    // Inside Flex
    lv_style_set_flex_grow(style, 1);
}