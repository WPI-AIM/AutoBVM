#include <display/layouts/layouts.h>
#include "screen.h"

Screen::~Screen()
{
    if (screen) {
        lv_obj_del(screen);
    }
}

void Screen::init()
{
    screen = lv_obj_create(nullptr);
}

void Screen::select_screen()
{
    lv_scr_load(screen);
}

void Screen::setup()
{

}

void Screen::cleanup()
{
    if (screen) {
        lv_obj_clean(screen);
    }
}