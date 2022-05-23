#include "TftTouch.h"
#include "utilities/logging.h"

volatile bool has_new_touch = false;

void handle_interrupt()
{
    has_new_touch = true;
}

TftTouch::TftTouch(uint8_t int_pin, uint8_t rst_pin)
{
    interrupt_pin = int_pin;
    reset_pin = rst_pin;
}

void TftTouch::init()
{

    Serial.println("Setting touchscreen pin modes & interrupts");

    pinMode(interrupt_pin, INPUT);
    pinMode(reset_pin, OUTPUT);
    reset();
    delay(100);

#ifdef digitalPinToInterrupt
    attachInterrupt(digitalPinToInterrupt(interrupt_pin), handle_interrupt, FALLING);
#else
    attachInterrupt(0, handleInterrupt, FALLING);
#endif

    Serial.println("Beginning I2C Wire");
    Wire.begin();
    Wire.beginTransmission(FT_I2C_ADDRESS);
    Wire.write(FT_DEVICE_MODE);
    Wire.write(0);
    Wire.endTransmission(FT_I2C_ADDRESS);
    Serial.println("Touchscreen setup done.");
}

bool TftTouch::touched()
{
    if (has_new_touch) {
        has_new_touch = false;
        return true;
    }
    return false;
}

void TftTouch::print_info()
{
    byte registers[FT_REG_COUNT];
    memset(registers, 0, FT_REG_COUNT);
    Wire.beginTransmission(FT_I2C_ADDRESS);
    Wire.write(0x00);
    Wire.endTransmission(FT_I2C_ADDRESS);
    Wire.requestFrom(FT_I2C_ADDRESS, FT_REG_COUNT);
    int register_number = 0;
    // get all register bytes when available
    Serial.println("[");
    while (Wire.available()) {
        registers[register_number] = Wire.read();
        Serial.print("\t");
        Serial.println(registers[register_number]);
        delay(5);
        register_number++;
    }
    Serial.println("]");
    delay(10);
    // Might be that the interpretation of high/low bit is not same as major/minor version...
    Serial.print("Library version: ");
    Serial.print(registers[FT_TOUCH_LIB_VERSION_H]);
    Serial.print(".");
    Serial.print(registers[FT_TOUCH_LIB_VERSION_L]);
    Serial.println(".");
}

void TftTouch::reset() const
{
    digitalWrite(reset_pin, LOW);
    delay(100);
    digitalWrite(reset_pin, HIGH);
}

void TftTouch::read_num_touch_points(uint8_t& points)
{
    Wire.beginTransmission(FT_I2C_ADDRESS);
    Wire.write(FT_TOUCH_POINTS);
    Wire.endTransmission(FT_I2C_ADDRESS);
    Wire.requestFrom(FT_I2C_ADDRESS, 1);
    if (Wire.available()) {
        points = Wire.read();
    }
}

void TftTouch::read_touch_registers(uint8_t len)
{
    int array_length = len * NUM_BYTES_PER_INPUT+1;
    uint8_t raw_data[array_length];
    memset(raw_data, 0, array_length);

    Wire.beginTransmission(FT_I2C_ADDRESS);
    Wire.write(FT_TOUCH1_BEGIN);
    Wire.endTransmission(FT_I2C_ADDRESS);
    Wire.requestFrom(FT_I2C_ADDRESS, array_length);
    int idx = 0;
    while (Wire.available()) {
        raw_data[idx] = Wire.read();
        idx++;
    }

    for (idx = 0; idx < len; idx++) {
        read_touch(new_touch_data+idx, raw_data, idx);
    }
}

void TftTouch::read_touch(TsData* data, const uint8_t* raw_data, uint8_t reg)
{
    uint8_t values[USED_BYTES_PER_INPUT];
    int event_flag;

    for (int i = 0; i < USED_BYTES_PER_INPUT; i++) {
        values[i] = raw_data[(reg * NUM_BYTES_PER_INPUT)+i];
    }

    event_flag = constrain(
            ((values[0] & FT_STATUS_MASK) >> 6),
            0,
            static_cast<int>(TouchState::IDLE)
    ); // Clamp event to TouchState enum

    data->state = static_cast<TouchState>(event_flag);

    if (data->state == TouchState::RESERVED) {
        return;         // Ignore Reserved Events
    }

    memcpy(data->raw, values, USED_BYTES_PER_INPUT);
    data->event_flag = event_flag;
    data->x = word(data->raw[0] & FT_XH_MASK, data->raw[1]);
    data->y = word(data->raw[2] & FT_XH_MASK, data->raw[3]);
    data->weight = data->raw[4];

    TsData last_data = last_touch_data[reg];

    /**
     * It seems like for some reason the release data is always the last coordinates exactly reversed.
     * I've had some weird data where this will be true but the event flag won't always equal released (0x01).
     * This might not be permanent but it seems to help
     */
    if (last_data.x == data->y && last_data.y == data->x && data->state == PRESSED) {
        data->state = RELEASED;
    }

    /**
     * Same thing as above, just if the event happens to be released we still need to reverse the coordinates.
     * LVGL gets very upset if you don't
     */
    if (last_data.x == data->y && last_data.y == data->x && data->state == RELEASED) {
        uint16_t temp_y = data->y;
        data->y = data->x;
        data->x = temp_y;
    }

    /**
     * Depending on the pressure & movement, sometimes a 'Touch Down' event is never fired, and the first event received
     * is TouchState::HELD (0x02)
     * If we're idle or we released last, and the state now is 'Held', assume we have another touch down event
     */
    if ((last_data.state == RELEASED || last_data.state == IDLE) && data->state == HELD) {
        data->state = PRESSED;
    }

    /**
     * Another fix for a small regression.
     * Some swipe gestures are faster than the polling speed, so the final coordinates might not be an exact mirror of the last coordinates.
     */
    if (data->state == RELEASED && data->y > SCREEN_HEIGHT) {
        uint16_t temp_y = data->y;
        data->y = data->x;
        data->x = temp_y;
    }
}

void TftTouch::print_touch_data(TsData data)
{
    Serial.println("[");
    Serial.print("\t0b");
    Serial.print(data.raw[0], BIN);
    Serial.print("\t0b");
    Serial.println(data.raw[1], BIN);
    Serial.print("\t0b");
    Serial.print(data.raw[2], BIN);
    Serial.print("\t\t0b");
    Serial.println(data.raw[3], BIN);
    serial_printf("\tX: %d\t\tY: %d\n", data.x, data.y);
    serial_printf("\tWeight: %d\tEvent: %x\tPoints: %d\n", data.weight, data.event_flag, data.num_points);
    Serial.println("]\n");
}

void print_touch_release(TsData& data)
{
    if (data.state == PRESSED) {
        serial_printf("Pressed at\t [%d, %d]\n", data.x, data.y);
    }
    else if (data.state == RELEASED) {
        serial_printf("Released at\t [%d, %d]\n\n", data.x, data.y);
    }
    else if (data.state == HELD) {
        serial_printf("Held at\t\t [%d, %d]\n", data.x, data.y);
    }
}
