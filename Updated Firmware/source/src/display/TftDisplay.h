#ifndef UVENT_TFTDISPLAY_H
#define UVENT_TFTDISPLAY_H

#include "SPI.h"
#include "Wire.h"
#include "Adafruit_I2CDevice.h"
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include "lvgl.h"
#include "touch/TftTouch.h"
#include "../../config/uvent_conf.h"

// This is calibration data for the raw touch data to the screen coordinates
// In the future, we should calibrate the display rather than hardcode and store the values in eeprom
/*
*                 TS_MAXX
*           --------------------
*           |                  |
*           |                  |
*           |                  |
*           |                  |
*           |                  |
*  TS_MINY  |                  |  TS_MAXY
*           |                  |
*           |                  |
*           |                  |
*           |                  |
*           |                  |
*           --------------------
*                 TS_MINX
*/

#define TS_MINX 300
#define TS_MINY 150
#define TS_MAXX 3700
#define TS_MAXY 3900

#define MINPRESSURE 40
#define MAXPRESSURE 1000

#define BUFFER_SIZE (SCREEN_WIDTH * 15)

void wrapped_flush_display(struct _lv_disp_drv_t* lv_disp_drv, const lv_area_t* area, lv_color_t* color_p);

void wrapped_read_inputs(struct _lv_indev_drv_t* lv_indev_drv, lv_indev_data_t* data);

/**
* LVGL Callback to handle logging on different platforms.
* @param buf The char sequence to be printed
*/
void lv_log_function(const char* buf);

class TftDisplay {
public:
    TftDisplay(uint8_t cs_pin, uint8_t rst_pin, uint8_t touch_int_pin, uint8_t touch_rst_pin);

    ~TftDisplay() = default;

    /**
     * Starts the RA8875 display, then LVGL, then registers handlers.
     *
     * @return True if successful, False otherwise.
     */
    bool init();

    /**
     * <p>
     * Runs update functions and other interval-based code <br>
     * Should be run on a timer or Arduino <code>loop()</code> function. Does not set its own delay.
     * </p>
     */
    void update();

    /**
    * <p>
    * Registered with LVGL on init.
    * Will be periodically called with block updates to the display, leaving implementation up to the user.
    * Uses Adafruit RA8875 to write out lines
    * </p>
     *
    * @param lv_disp_drv The LVGL display driver, written to when done to signal finished.
    * @param area      The area that needs updating (x, y, x1, y1)
    * @param color_p   An array of colors, either in RGB or uin16_t. Requires <code>COLOR_DEPTH=16</code> in LVGL config.
    */
    void flush_display(struct _lv_disp_drv_t* lv_disp_drv, const lv_area_t* area, lv_color_t* color_p);

    /**
     * <p>
     * Registered with LVGL on init.
     * Callback function to read input from connected touchscreen
     * Uses Adafruit RA8875 to listen for data
     * </p>
     *
     * @param lvIndevDrv The LVGL Touchscreen Driver
     * @param data Position data for the touch
     */
    void read_inputs(struct _lv_indev_drv_t* lvIndevDrv, lv_indev_data_t* data);

    void flush_display_complete();

    void onDMAInterrupt();

protected:
    Adafruit_RA8875 tft_display{0, 0};
    TftTouch touch_driver{0, 0};

private:
    lv_disp_drv_t lv_display_driver{};
    lv_disp_draw_buf_t lv_screen_buffer{};
    lv_color_t pixel_buffer_1[BUFFER_SIZE]{};
#if USE_DMA_INTERRUPT
    lv_color_t pixel_buffer_2[BUFFER_SIZE]{};
#endif
    lv_indev_drv_t lv_input_driver{};
};

#endif //UVENT_TFTDISPLAY_H