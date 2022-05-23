#include "test_pressure_sensors.h"

PressureSensor gauge_sensor_test = {PRESSURE_GAUGE_PIN, MAX_GAUGE_PRESSURE, MIN_GAUGE_PRESSURE, RESISTANCE_1, RESISTANCE_2};
PressureSensor diff_sensor_test = {PRESSURE_DIFF_PIN, MAX_DIFF_PRESSURE_TYPE_0, MIN_DIFF_PRESSURE_TYPE_0, RESISTANCE_1, RESISTANCE_2};
int zero_flag = 1;

void test_sensors_read_pressure(int delay_time, bool zero, Units_pressure units_gauge, Units_pressure units_diff)
{
    Serial.print("Gauge Pressure:        ");
    if (zero && zero_flag == 1) {
        int32_t offset = 0;
        gauge_sensor_test.calculate_zero(offset, Zero_type::gauge);

        Serial.println(gauge_sensor_test.get_pressure(units_gauge, zero), 6);
    }
    else {
        Serial.println(gauge_sensor_test.get_pressure(units_gauge, zero), 6);
    }

    Serial.print("Differential Pressure: ");
    if (zero && zero_flag == 1) {
        int32_t offset = 0;
        diff_sensor_test.calculate_zero(offset, Zero_type::diff);
        zero_flag = 0;

        Serial.println(diff_sensor_test.get_pressure(units_diff, zero), 6);
        Serial.println("Zeroing, This should print only once.");
    }
    else {
        Serial.println(diff_sensor_test.get_pressure(units_diff, zero), 6);
    }

    Serial.println("");
    Serial.println("------------------------");
    Serial.println("");

    delay(delay_time);
}

void test_sensors_read_gauge(int delay_time, bool zero, Units_pressure units_gauge)
{
    Serial.print("Gauge Pressure:        ");
    if (zero && zero_flag == 1) {
        int32_t offset = 0;
        gauge_sensor_test.calculate_zero(offset, Zero_type::gauge);
        zero_flag = 0;

        Serial.println(gauge_sensor_test.get_pressure(units_gauge, zero), 6);
        Serial.println("Zeroing, This should print only once. If not, set ENABLE_TEST_PRESSURE_SENSORS to 1");
    }
    else {
        Serial.println(gauge_sensor_test.get_pressure(units_gauge, zero), 6);
    }

    delay(delay_time);
}

void test_sensors_read_differential(int delay_time, bool zero, Units_pressure units_diff)
{
    Serial.print("Differential Pressure: ");
    if (zero && zero_flag == 1) {
        int32_t offset = 0;
        diff_sensor_test.calculate_zero(offset, Zero_type::diff);
        zero_flag = 0;

        Serial.println(diff_sensor_test.get_pressure(units_diff, zero), 6);
        Serial.println("Zeroing, This should print only once. If not, set ENABLE_TEST_PRESSURE_SENSORS to 1");
    }
    else {
        Serial.println(diff_sensor_test.get_pressure(units_diff, zero), 6);
    }

    delay(delay_time);
}

void test_sensors_read_flow(int delay_time, bool zero, Order_type order, Units_flow units)
{
    if (zero && zero_flag == 1) {
        int32_t offset = 0;
        diff_sensor_test.calculate_zero(offset, Zero_type::diff);
        zero_flag = 0;

        Serial.println(diff_sensor_test.get_flow(units, true, order));
        Serial.println("Zeroing, This should print only once. If not, set ENABLE_TEST_PRESSURE_SENSORS to 1");
    }
    else if (zero && zero_flag == 0) {
        Serial.println(diff_sensor_test.get_flow(units, true, order));
    }
    else {
        Serial.println(diff_sensor_test.get_flow(units, false, order));
    }

    delay(delay_time);
}