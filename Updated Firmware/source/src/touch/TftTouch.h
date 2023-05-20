#ifndef UVENT_TFTTOUCH_H
#define UVENT_TFTTOUCH_H

#include <cstdint>
#include "Wire.h"
#include "../../config/uvent_conf.h"

#define FT_I2C_ADDRESS              0x38
#define FT_STATUS_MASK              0b11000000
#define FT_XH_MASK                  0x0F
#define NUM_TOUCH_REGISTERS         10
#define NUM_BYTES_PER_INPUT         6
#define USED_BYTES_PER_INPUT        5
#define READ_INPUTS_LENGTH          NUM_TOUCH_REGISTERS * NUM_BYTES_PER_INPUT
#define FT_REG_COUNT                0xFE

#define FT_DEVICE_MODE              0x00

#define FT_GESTURE_ID               0x01
#define FT_GESTURE_ID_MOVE_UP       0x10
#define FT_GESTURE_ID_MOVE_LEFT     0x14
#define FT_GESTURE_ID_MOVE_DOWN     0x18
#define FT_GESTURE_ID_MOVE_RIGHT    0x1c
#define FT_GESTURE_ID_ZOOM_IN       0x48
#define FT_GESTURE_ID_ZOOM_OUT      0x49
#define FT_GESTURE_ID_NO_GESTURE    0x00

#define FT_TOUCH_POINTS             0x02

#define FT_TOUCH1_XH                0x03
#define FT_TOUCH1_XL                0x04
#define FT_TOUCH1_YH                0x05
#define FT_TOUCH1_YL                0x06
#define FT_TOUCH1_WEIGHT            0x07
#define FT_TOUCH1_MISC              0x08
#define FT_TOUCH1_BEGIN             FT_TOUCH1_XH

#define FT_TOUCH2_XH                0x09
#define FT_TOUCH2_XL                0x0a
#define FT_TOUCH2_YH                0x0b
#define FT_TOUCH2_YL                0x0c
#define FT_TOUCH2_WEIGHT            0x0d
#define FT_TOUCH2_MISC              0x0e
#define FT_TOUCH2_BEGIN             FT_TOUCH2_XH

#define FT_TOUCH3_XH                0x0f
#define FT_TOUCH3_XL                0x10
#define FT_TOUCH3_YH                0x11
#define FT_TOUCH3_YL                0x12
#define FT_TOUCH3_WEIGHT            0x13
#define FT_TOUCH3_MISC              0x14
#define FT_TOUCH3_BEGIN             FT_TOUCH3_XH

#define FT_TOUCH4_XH                0x15
#define FT_TOUCH4_XL                0x16
#define FT_TOUCH4_YH                0x17
#define FT_TOUCH4_YL                0x18
#define FT_TOUCH4_WEIGHT            0x19 // Not sure about this one, it's not listed in the datasheet
#define FT_TOUCH4_MISC              0x1a
#define FT_TOUCH4_BEGIN             FT_TOUCH4_XH

#define FT_TOUCH5_XH                0x1b
#define FT_TOUCH5_XL                0x1c
#define FT_TOUCH5_YH                0x1d
#define FT_TOUCH5_YL                0x1e
#define FT_TOUCH5_WEIGHT            0x1f
#define FT_TOUCH5_MISC              0x20
#define FT_TOUCH5_BEGIN             FT_TOUCH5_XH

#define FT_TOUCH6_XH                0x21
#define FT_TOUCH6_XL                0x22
#define FT_TOUCH6_YH                0x23
#define FT_TOUCH6_YL                0x24
#define FT_TOUCH6_WEIGHT            0x25
#define FT_TOUCH6_MISC              0x26
#define FT_TOUCH6_BEGIN             FT_TOUCH6_XH

#define FT_TOUCH7_XH                0x27
#define FT_TOUCH7_XL                0x28
#define FT_TOUCH7_YH                0x29
#define FT_TOUCH7_YL                0x2a
#define FT_TOUCH7_WEIGHT            0x2b
#define FT_TOUCH7_MISC              0x2c
#define FT_TOUCH7_BEGIN             FT_TOUCH7_XH

#define FT_TOUCH8_XH                0x2d
#define FT_TOUCH8_XL                0x2e
#define FT_TOUCH8_YH                0x2f
#define FT_TOUCH8_YL                0x30
#define FT_TOUCH8_WEIGHT            0x31
#define FT_TOUCH8_MISC              0x32
#define FT_TOUCH8_BEGIN             FT_TOUCH8_XH

#define FT_TOUCH9_XH                0x33
#define FT_TOUCH9_XL                0x34
#define FT_TOUCH9_YH                0x35
#define FT_TOUCH9_YL                0x36
#define FT_TOUCH9_WEIGHT            0x37
#define FT_TOUCH9_MISC              0x38
#define FT_TOUCH9_BEGIN             FT_TOUCH9_XH

#define FT_TOUCH10_XH               0x39
#define FT_TOUCH10_XL               0x3a
#define FT_TOUCH10_YH               0x3b
#define FT_TOUCH10_YL               0x3c
#define FT_TOUCH10_WEIGHT           0x3d
#define FT_TOUCH10_MISC             0x3e
#define FT_TOUCH10_BEGIN            FT_TOUCH10_XH

#define FT_ID_G_MC_THGROUP          0x80
#define FT_ID_G_MC_THPEAK           0x81
#define FT_ID_G_THDIFF              0x85
#define FT_ID_G_CTRL                0x86
#define FT_ID_G_PERIODACTIVE        0x88
#define FT_ID_G_PERIODMONITOR       0x89

#define FT_TOUCH_LIB_VERSION_H      0xa1
#define FT_TOUCH_LIB_VERSION_L      0xa2
#define FT_ID_G_CHIPER_HIGH         0xa3
#define FT_ID_G_MODE                0xa4
#define FT_ID_G_PMODE               0xa5
#define FT_ID_G_FIRMID              0xa6
#define FT_ID_G_VENODRID            0xa8
#define FT_ID_G_GLOVE_MODE_EN       0xc0
#define FT_ID_G_COVER_MODE_EN       0xc1

typedef enum {
    PRESSED = 0,                // 0
    RELEASED,                   // 1
    HELD,                       // 2
    RESERVED,                   // 3
    IDLE,                       // 4
} TouchState;

struct TsData {
    uint16_t x = 0;
    uint16_t y = 0;
    uint8_t event_flag = 0;
    uint8_t weight = 0;
    TouchState state = IDLE;
    uint8_t num_points = 0;
    uint8_t raw[5] = {0, 0, 0, 0, 0};

    bool operator==(const TsData& other) const
    {
        return x == other.x &&
                y == other.y &&
                event_flag == other.event_flag;
    }
};

void handle_interrupt();
void print_touch_release(TsData& data);

class TftTouch {
public:

    static void print_touch_data(TsData data);
    static uint8_t get_touch_state(TsData data);

    TftTouch(uint8_t int_pin, uint8_t rst_pin);
    ~TftTouch() = default;

    void init();
    bool touched();
    void read_touch_registers(uint8_t len);
    void print_info();

    TsData new_touch_data[NUM_TOUCH_REGISTERS];
    TsData last_touch_data[NUM_TOUCH_REGISTERS];
protected:

    void reset() const;
    void read_touch(TsData* new_touch, const uint8_t* raw_data, uint8_t reg);
    void read_num_touch_points(uint8_t& points);
    uint8_t interrupt_pin;
    uint8_t reset_pin;
};

#endif //UVENT_TFTTOUCH_H
