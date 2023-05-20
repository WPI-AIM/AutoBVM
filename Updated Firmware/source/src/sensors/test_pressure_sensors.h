#ifndef TEST_PRESSURE_SENSORS
#define TEST_PRESSURE_SENSORS

#include "../config/uvent_conf.h"
#include "pressure_sensor.h"

// Output gauge and differential pressure. Pins are set in uvent_conf.h
void test_sensors_read_pressure(int delay_time = 1000, bool zero = false, Units_pressure units_gauge = Units_pressure::cmH20, Units_pressure units_diff = Units_pressure::mbar);

// Output gauge pressure. Pins are set in uvent_conf.h
void test_sensors_read_gauge(int delay_time = 1000, bool zero = false, Units_pressure units_gauge = Units_pressure::cmH20);

// Output differential pressure. Pins are set in uvent_conf.h
void test_sensors_read_differential(int delay_time = 1000, bool zero = false, Units_pressure units_diff = Units_pressure::mbar);

// Output the flow based on the curve fit equation
void test_sensors_read_flow(int delay_time = 1000, bool zero = false, Order_type order = Order_type::second, Units_flow units = Units_flow::lpm);

#endif//TEST_PRESSURE_SENSORS
