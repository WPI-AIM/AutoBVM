#ifndef UVENT_SCREEN_H
#define UVENT_SCREEN_H

#include <lvgl.h>
#include <controls/interface/charts.h>

#define MAIN_SCREEN_CHART_COUNT 2

class Screen {
public:
    Screen() = default;
    ~Screen();
    virtual void init();
    virtual void setup();
    void cleanup();
    void select_screen();
protected:
    lv_obj_t* screen = nullptr;
};

class MainScreen : public Screen {
public:
    MainScreen();
    void init() override;
    void setup() override;

    void open_config();
    void attach_settings_cb();
    const SensorChart* get_chart(uint8_t idx);
    void try_refresh_charts();
private:
    void generate_charts();
    SensorChart charts[MAIN_SCREEN_CHART_COUNT]{};
};

class StartupScreen : public Screen {
public:
    StartupScreen();
    void init() override;
    void setup() override;

    void (* on_complete)(lv_event_t* event);
};

#endif //UVENT_SCREEN_H
