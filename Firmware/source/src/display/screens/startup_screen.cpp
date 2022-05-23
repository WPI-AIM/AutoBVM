#include "screen.h"
#include <display/layouts/layouts.h>

StartupScreen::StartupScreen()
        : Screen()
{

}

void StartupScreen::init()
{
    Screen::init();
}

void StartupScreen::setup()
{
    Screen::setup();

    lv_obj_t* container = open_yes_no_dialog("", false, "Continue", nullptr, on_complete);
    if (!container) {
        LV_LOG_WARN("Unable to create confirm/deny window. Another window could be open");

        if (on_complete) {
            on_complete(nullptr);
            return;
        }
    }

    lv_obj_set_style_pad_all(container, 10 px, LV_PART_MAIN);

    lv_obj_t* label_1 = lv_label_create(container);
    lv_obj_set_style_text_decor(label_1, LV_TEXT_DECOR_UNDERLINE, LV_PART_MAIN);
    lv_obj_set_style_text_align(label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(label_1, &lv_font_montserrat_28, LV_PART_MAIN);

    lv_label_set_text(label_1, "Welcome\n");

    lv_obj_t* label_2 = lv_label_create(container);
    lv_label_set_long_mode(label_2, LV_LABEL_LONG_WRAP);

    lv_label_set_text(label_2,
            "To ensure best function of this unit, please perform the following actions before use.\n");

    lv_obj_set_style_text_align(label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(label_2, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_width(label_2, LV_PCT(100));

    lv_obj_t* label_3 = lv_label_create(container);
    lv_label_set_long_mode(label_3, LV_LABEL_LONG_WRAP);

    lv_label_set_text(
            label_3,
            "• Home the stepper motor\n"
    );

    lv_obj_set_style_text_align(label_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(label_3, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_width(label_3, LV_PCT(100));
}
