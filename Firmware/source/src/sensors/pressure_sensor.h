#ifndef PRESSURE_SENSOR
#define PRESSURE_SENSOR

#include "../config/uvent_conf.h"
#include <Arduino.h>

typedef enum class units_pressure {
    psi,
    mbar,
    bar,
    mmHg,
    inHg,
    cmH20,
    inH20,
    atm,
    kPa
} Units_pressure;

typedef enum class units_flow {
    lpm,
    lps,
    mlpm,
    mlps,
    cfm
} Units_flow;

typedef enum class zero_type {
    DONT_ZERO,
    gauge,
    diff
} Zero_type;

typedef enum class order_type {
    first,
    second,
    third
} Order_type;

class PressureSensor {
public:
    // Definition: This version of the constructor requires the init function to be called.
    // Arguments ->
    // analog_pin: the pin the the pressure sensor is connected to.
    PressureSensor(int analog_pin) : analog_pin(analog_pin){};

    // Arguments ->
    // analog_pin: the pin the the pressure sensor is connected to.
    // max_psi: the maximum pressure of the sensor in units psi
    // min_psi: the minimum pressure of the sensor in units psi
    // resistance_ohms_1: the resistance of the first resistor in the circuit in units ohms
    // resistance_ohms_2: the resistance of the second resistor in the circuit in units ohms
    PressureSensor(int analog_pin, double max_psi, double min_psi, int resistance_ohms_1, int resistance_ohms_2);

    // Definition: Set up the pressure sensor with the correct values
    // Arguments ->
    // max_psi: the maximum pressure of the sensor in units psi
    // min_psi: the minimum pressure of the sensor in units psi
    // resistance_ohms_1: the resistance of the first resistor in the circuit in units ohms
    // resistance_ohms_2: the resistance of the second resistor in the circuit in units ohms
    // pressure_offset_adc_counts: the analog offset of the pressure sensor
    void init(double max_psi, double min_psi, int resistance_ohms_1, int resistance_ohms_2, int32_t pressure_offset_adc_counts);

    // Definition: Returns the pressure from the gauge sensor.
    // Arguments ->
    // units: units of measurement requested, default is in psi
    // zero: whether or not to zero the pressure sensor
    double get_pressure(Units_pressure units = Units_pressure::psi, bool zero = false);

    // Definition: returns the flow calculated from a curve fit in excel that compared differential pressure to flow
    // Argument ->
    // units: units of measurement requested, default is in lpm
    // zero: whether or not to zero the pressure sensor
    // order: Whether to use the 3rd order, 2nd order, or 1st order flow equation
    double get_flow(Units_flow units = Units_flow::lpm, bool zero = false, Order_type order = Order_type::second);

    // Definition: zero_value will be reset to 0
    // Arguments ->
    // pressure_offset_adc_counts: the analog offset of the pressure sensor
    void reset_zero(int32_t& pressure_offset_adc_counts);

    // Definition: Set a new zeroed value
    // Arguments ->
    // pressure_offset_adc_counts: the analog offset of the pressure sensor
    // zero_type: depends on the specific sensor in use.
    void calculate_zero(int32_t& pressure_offset_adc_counts, Zero_type zero_type);

private:
    // CONVERSION TABLE
    // pressure
    double const PSI_TO_MBAR = 68.9476;
    double const PSI_TO_BAR = 0.0689476;
    double const PSI_TO_MMHG = 51.7149;
    double const PSI_TO_INHG = 2.03602;
    double const PSI_TO_CMH20 = 70.307;
    double const PSI_TO_INH20 = 27.7076;
    double const PSI_TO_ATM = 0.068046;
    double const PSI_TO_KPA = 6.89476;
    // flow
    double const LPM_TO_LPS = 0.01667;
    double const LPM_TO_MLPM = 1000;
    double const LPM_TO_MLPS = 16.67;
    double const LPM_TO_CFM = 0.03531;

    // Coefficents for 1st order equation for Flow vs Differential Pressure. Created using a fluke flow and inputing values from that device into excel
    // Equation: y = 45.089x
    // R value: 0.9975
    // Standard Deviation of values calculated with curve fit vs recorded from fluke flow: 1.36 lpm
    double const COEF_A_1ST_ORDER = 45.089;

    // Coefficents for 2nd order equation for Flow vs Differential Pressure. Created using a fluke flow and inputing values from that device into excel
    // Equation: y = -1.4452x^2 + 44.573x
    // R value: 0.9996
    // Standard Deviation of values calculated with curve fit vs recorded from fluke flow: 1.04 lpm
    double const COEF_A_2ND_ORDER = -1.4452;
    double const COEF_B_2ND_ORDER = 44.573;

    // Coefficents for 3nd order equation for Flow vs Differential Pressure. Created using a fluke flow and inputing values from that device into excel
    // Equation: -0.4608x^3 - 1.5564x^2 + 45.477x;
    // R value: 0.9996
    // Standard Deviation of values calculated with curve fit vs recorded from fluke flow: 1.17 lpm
    double const COEF_A_3RD_ORDER = -0.4608;
    double const COEF_B_3RD_ORDER = -1.5564;
    double const COEF_C_3RD_ORDER = 45.477;

    // Voltage Supply
    // Units: Volts
    double const VOLTAGE_SUPPLY = 5.0;
    double const VOLTAGE_ADC_REF = 3.3;

    // Pin on the arduino connecting the sensor
    int analog_pin;

    // Calculations for determining the gauge or differential pressure
    double constant_A;
    double constant_B;

    // value that holds the pressure_offset_adc_counts
    int32_t offset_adc_counts;

    // the differential resolution is half the maximum resolution
    int diff_zero_resolution;

    // Definition: Modifies the measured pressure in the users choosen units of measurement
    // Arguments ->
    // pressure: the pressure measured in units psi
    // units: units of measurement requested
    void determine_units_pressure(double& pressure, Units_pressure units);

    // Definition: modifies measured flow in the users choosen units of measurement
    // Arguments ->
    // flow: the flow measured in units lpm
    // units: units of measurement requested
    void determine_units_flow(double& flow, Units_flow units);
};

#endif// PRESSURE_SENSOR