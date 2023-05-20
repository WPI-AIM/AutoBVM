#include <utilities/util.h>
#include <function_timings.h>
#include <utilities/logging.h>
#include "TftDisplay.h"
#include "../../config/uvent_conf.h"

uint32_t debug_toggle_timer = 0;

void wrapped_flush_display(struct _lv_disp_drv_t* lv_disp_drv, const lv_area_t* area, lv_color_t* color_p)
{
    static_cast<TftDisplay*>(lv_disp_drv->user_data)->flush_display(lv_disp_drv, area, color_p);
}

void wrapped_read_inputs(struct _lv_indev_drv_t* lv_indev_drv, lv_indev_data_t* data)
{
    static_cast<TftDisplay*>(lv_indev_drv->user_data)->read_inputs(lv_indev_drv, data);
}

TftDisplay::TftDisplay(uint8_t cs_pin, uint8_t rst_pin, uint8_t touch_int_pin, uint8_t touch_rst_pin)
        : tft_display(cs_pin, rst_pin, true)
{
    touch_driver = TftTouch(touch_int_pin, touch_rst_pin);
}

bool TftDisplay::init()
{

    Serial.println("Initializing...");

#if USE_DMA_INTERRUPT
    NVIC_EnableIRQ(DMAC_IRQn);
#endif

    if (!tft_display.begin(RA8875_800x480)) {
        Serial.println("Failed to init display, RA8875 did not setup correctly");
        return false;
    }

    tft_display.setClockSpeed(SPI_CLK_SPEED);                   // Manually set clock speed to something faster
    tft_display.displayOn(true);
    tft_display.GPIOX(true);                                // Enable TFT - display enable tied to GPIOX
    tft_display.PWM1config(true, RA8875_PWM_CLK_DIV1024);   // PWM output for backlight
    tft_display.PWM1out(255);
    tft_display.graphicsMode();

    Serial.println("Display connected, starting touchscreen setup...");

    touch_driver.init();

    Serial.println("Touchscreen init finished, starting LVGL...");
    lv_init();

#if USE_DMA_INTERRUPT
    Serial.println("Compiled with DMA & Interrupts, allocating second pixel buffer...");
    lv_disp_draw_buf_init(&lv_screen_buffer, pixel_buffer_1, pixel_buffer_2, BUFFER_SIZE);
    lv_screen_buffer.buf1 = pixel_buffer_1;
    lv_screen_buffer.buf2 = pixel_buffer_2;
    lv_screen_buffer.buf_act = lv_screen_buffer.buf1;
    lv_screen_buffer.size = BUFFER_SIZE;
#else
    Serial.println("Compiled without DMA & Interrupts, no additional buffer required...");
    lv_disp_draw_buf_init(&lv_screen_buffer, pixel_buffer_1, nullptr, BUFFER_SIZE);
    lv_screen_buffer.buf1 = pixel_buffer_1;
    lv_screen_buffer.buf2 = nullptr;
    lv_screen_buffer.buf_act = lv_screen_buffer.buf1;
    lv_screen_buffer.size = BUFFER_SIZE;
#endif

    lv_disp_drv_init(&lv_display_driver);                 // Initialize the display
    lv_display_driver.user_data = this;                   // Save `this` for callback functions
    lv_display_driver.hor_res = SCREEN_WIDTH;             // Set Resolution
    lv_display_driver.ver_res = SCREEN_HEIGHT;
    lv_display_driver.flush_cb = wrapped_flush_display;     // Callback for display writing
    lv_display_driver.draw_buf = &lv_screen_buffer;
    lv_disp_drv_register(&lv_display_driver);             // register Display

    Serial.println("Finished registering lvgl display and drivers");
    Serial.println("Registering touchscreen...");

    lv_indev_drv_init(&lv_input_driver);
    lv_input_driver.type = LV_INDEV_TYPE_POINTER;
    lv_input_driver.user_data = this;
    lv_input_driver.read_cb = wrapped_read_inputs;
    lv_input_driver.long_press_time = 750;
    lv_input_driver.long_press_repeat_time = 250;
    lv_indev_drv_register(&lv_input_driver);

    Serial.println("DONE.");

    return true;
}

void TftDisplay::flush_display(struct _lv_disp_drv_t* lv_disp_drv, const lv_area_t* area, lv_color_t* color_p)
{

    lv_coord_t width = lv_area_get_width(area);
    lv_coord_t height = lv_area_get_height(area);

#if USE_DMA_INTERRUPT

    tft_display.drawPixelsAreaDMA((uint16_t*) color_p, width * height, area->x1, area->y1, width,
            &lv_display_driver,
            [](void* cb_data) {
                lv_disp_flush_ready((lv_disp_drv_t*) cb_data);
            });

#else

    tft_display.drawPixelsArea((uint16_t*) color_p, width * height, area->x1, area->y1, width);
    flush_display_complete();

#endif

}

void TftDisplay::flush_display_complete()
{
    lv_disp_flush_ready(&lv_display_driver);
}

void TftDisplay::onDMAInterrupt()
{
#if USE_DMA_INTERRUPT
    tft_display.onDMAInterrupt();
#endif
}

void TftDisplay::read_inputs(struct _lv_indev_drv_t* lvIndevDrv, lv_indev_data_t* data)
{
    bool is_pressed = touch_driver.touched();
    if (is_pressed) {
        touch_driver.read_touch_registers(1);

        TsData touch_data = *(touch_driver.new_touch_data);

        data->point.x = touch_data.x;
        data->point.y = touch_data.y;
        switch (touch_data.state) {
            case PRESSED:
            case HELD:      // Fall-through case
                data->state = LV_INDEV_STATE_PR;
                break;
            case RELEASED:  //
            case RESERVED:  //
            case IDLE:      // Fall-through cases
            default:        //
                data->state = LV_INDEV_STATE_REL;
                break;
        }

        memcpy(touch_driver.last_touch_data, touch_driver.new_touch_data, sizeof(TsData) * NUM_TOUCH_REGISTERS);
    }
}

void TftDisplay::update()
{
    //TODO add sleep handler code
    if (!RA_GET_DEBUG_STATE() && has_time_elapsed(&debug_toggle_timer, 10000)) {
        RA_SET_DEBUG(true);
    }
    lv_task_handler();
}