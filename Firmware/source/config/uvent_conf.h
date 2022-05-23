#ifndef UVENT_CONF_H
#define UVENT_CONF_H

// Define the (default) serial number to compile, if not provided
#ifndef UVENT_SERIAL_IDENTIFIER
#define UVENT_SERIAL_IDENTIFIER "210000A0000\0"
#endif

// Version numbers
#define UVENT_VERSION_MAJOR 1
#define UVENT_VERSION_MINOR 0
#define UVENT_VERSION_PATCH 0

// Screen Dimensions
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480

// Debug LED
#define DEBUG_LED 13

// Display and touch pins
#define TOUCH_INT 23// 19
#define TOUCH_RST 22// 2
#define TFT_CS 10   // 4
#define TFT_RST 26  // 3

// Speaker and snooze pins
#define SPEAKER_PIN 40

// Draw borders around flexbox components for debugging
#define DEBUG_BORDER_READOUTS 0
#define DEBUG_BORDER_CONTROLS 0

// Pressure sensor testing
#define ENABLE_TEST_PRESSURE_SENSORS 0

// Settings address on external eeprom
#define EXT_EEPROM_SETTINGS_LOC 100

// CRC address on external eeprom
#define EXT_EEPROM_CRC_LOC 0

// Test EEPROM
#define ENABLE_TEST_EEPROM 0

// Test EEPROM Memory positions
#define EEPROM_TEST_1_MEM_1 63000
#define EEPROM_TEST_1_MEM_2 63010
#define EEPROM_TEST_1_MEM_3 63020
#define EEPROM_TEST_1_MEM_4 63040
#define EEPROM_TEST_1_MEM_5 63050
#define EEPROM_TEST_2_MEM_1 63100

// Serial baud rate
#define SERIAL_BAUD_RATE 115200

#define SPI_CLK_SPEED 22000000L

#ifndef ENABLE_CONTROL
#define ENABLE_CONTROL 1
#endif

// Control handler  period in microsec.
#define CONTROL_HANDLER_PERIOD_US 20000

// Actuator handler period in microsec.
#define ACTUATOR_HANDLER_PERIOD_US 50

// Pressure Sensor pins
#define PRESSURE_GAUGE_PIN 1
#define PRESSURE_DIFF_PIN 0

// Pressure Sensor resistor resistance in ohms
#define RESISTANCE_1 100000
#define RESISTANCE_2 196000

// Gauge Pressure sensor max and min pressure values, units psi
// Gauge unit number: 001PG
// data sheet link: https://sensing.honeywell.com/honeywell-sensing-trustability-ssc-series-standard-accuracy-board-mount-pressure-sensors-50099533-a-en.pdf
#define MAX_GAUGE_PRESSURE 1
#define MIN_GAUGE_PRESSURE 0

// Differential Pressure sensor max and min pressure values, units psi
// Differential unit number: 001PD
// data sheet link: https://sensing.honeywell.com/honeywell-sensing-trustability-ssc-series-standard-accuracy-board-mount-pressure-sensors-50099533-a-en.pdf
#define PRESSURE_SENSOR_TYPE_0 0
#define MAX_DIFF_PRESSURE_TYPE_0 0.07
#define MIN_DIFF_PRESSURE_TYPE_0 -0.07

#define PRESSURE_SENSOR_TYPE_1 1
#define MAX_DIFF_PRESSURE_TYPE_1 0.09
#define MIN_DIFF_PRESSURE_TYPE_1 -0.09

// Stepper: Direction pin
#define STEPPER_DIRECTION_PIN 8

// Stepper: Step pin
#define STEPPER_STEP_PIN 9

// Stepper: Disable pin
#define STEPPER_DISABLE_PIN 38

// Stepper: Maximum steps per second
#define STEPPER_MAX_STEPS_PER_SECOND 600

// Stepper: Homing speed steps/sec
#define STEPPER_HOMING_SPEED_IN_STEPS_SEC 100

// Stepper: Angle per step in degrees
#define STEPPER_ANGLE_DEG_PER_STEP 1.8

/* Stepper step configuration
 * 1 = FULL_STEP, 200 PPR, pulses per revolution
 * 2 = HALF_STEP, 400 PPR, pulses per revolution
 * 5 = 5_MICRO_STEP, 1000 PPR, pulses per revolution
 * 10 = 10_MICRO_STEP, 2000 PPR, pulses per revolution
 */
#define STEPPER_MULTIPLIER_MODE 1

// Stepper: No. of steps in one revolution
#define STEPPER_STEPS_PER_REV 360 / (STEPPER_ANGLE_DEG_PER_STEP / STEPPER_MULTIPLIER_MODE)

// Stepper
// Gear Ratio
#define TEETH_ON_PINION 14.0
#define TEETH_ON_TIMING_PULLEY 60.0
#define GEAR_RATIO TEETH_ON_TIMING_PULLEY / TEETH_ON_PINION

// One revolution of Timing pulley in steps
#define TIMING_PULLEY_STEPS_PER_REV (GEAR_RATIO * STEPPER_STEPS_PER_REV)

// Convert from degrees to steps and vice-versa
#define TIMING_PULLEY_DEGREES_TO_STEPS(x) x*(TIMING_PULLEY_STEPS_PER_REV / 360.0)
#define TIMING_PULLEY_STEPS_TO_DEGREES(x) x*(360.0 / TIMING_PULLEY_STEPS_PER_REV)

// Enable or disable Feedback sensor.
#define USE_AMS_FEEDBACK 0

// DEBUG FLAGS
#define DEBUG_STEPPER 0
#define DEBUG_CORRECTION 0
#define DEBUG_WAVEFORM 0

// EEPROM address
#define EEPROM_ADDRESS 0x50

// Actuator Limits
#define MIN_ACT_POS_DEG 0.0
#define MAX_ACT_POS_DEG 180.0

#define HOME_MIN_DEG 0.2
#define HOME_MAX_DEG 1.0

#define HOME_CORR_MOVE_DEG 0.5
#define HOME_CORRECTION_MAX_DEG 5.0
#define HOME_CORRECTION_MIN_DEG 355.0
#define HOME_CORRECTION_SPEED_DEG_P_SEC 100

// Waveform
// Breaths per minute
#define BPM_MIN 8
#define BPM_MAX 30
#define DEF_BPM 8

// AMBU Bag limit, aka vt(liters)
#define MIN_BAG_VOL_L 0.1
#define MAX_BAG_VOL_L 1.06
#define DEF_BAG_VOL_L 1.06// Default
#define MIN_BAG_VOL_ML MIN_BAG_VOL_L * 1000
#define MAX_BAG_VOL_ML MAX_BAG_VOL_L * 1000
#define DEF_BAG_VOL_ML DEF_BAG_VOL_L * 1000// Default

// I:E ratio
#define IE_MIN 0.5
#define IE_MAX 4.0
#define DEF_IE 1.0// Default

// PEEP(cmh2o)
#define PEEP_MIN 5
#define PEEP_MAX 20
#define DEF_PEEP 5// Default

// PIP(cmh2o)
#define PIP_MIN 15
#define PIP_MAX 40
#define DEF_PIP 40// Default

// Plateau(time in ms)
#define PLATEAU_MIN 100
#define PLATEAU_MAX 300
#define DEF_PLATEAU 100// Default

// Flow(lpm)
#define FLOW_MIN -20
#define FLOW_MAX 80

// Pressure(cmh2o)
#define PRESSURE_MIN 0
#define PRESSURE_MAX 41

// The 'None' value for a readout.
// Setting any adjustable value to this constant (or below) will force the label to show "--" as the value
// Set below your lowest known value, within the range of `double`
#define READOUT_VALUE_NONE (-1e3)
#define READOUT_VALUE_DEFAULT (READOUT_VALUE_NONE - 1)

// How often to refresh the readouts on the left side of the screen
#define READOUT_REFRESH_INTERVAL 1250

// How often to poll sensors for readouts (ms)
// Chart & Readout values will be updated every time the sensor polls
// These changes will not be visible until the next chart refresh time or
// READOUT_REFRESH_INTERVAL
#define SENSOR_POLL_INTERVAL 200
// How long to delay before polling data (ms). Allows for startup time of hardware components
#define SENSOR_POLL_STARTUP_DELAY 5000

// Always display polled data, even if the state machine isn't running
#define VERBOSE_DATA_POLLING 1

// Chart Configuration
#define GAUGE_PRESSURE_CHART_MIN_VALUE (-5)
#define GAUGE_PRESSURE_CHART_MAX_VALUE PIP_MAX
#define GAUGE_PRESSURE_CHART_MAX_POINTS 100
#define GAUGE_PRESSURE_CHART_REFRESH_TIME 150
#define GAUGE_PRESSURE_CHART_LINE_MODE 1  /**< 0: No dots, only display line. 1: Use dots as data points */
#define GAUGE_PRESSURE_CHART_DOT_SIZE 5   /**< Dot LxW in px. Only applies to LINE_MODE 1 (Default: 5)*/
#define GAUGE_PRESSURE_CHART_LINE_WIDTH 2 /**< Changes the width of the line on the graph (Default: 2)*/

#define FLOW_CHART_MIN_VALUE FLOW_MIN
#define FLOW_CHART_MAX_VALUE FLOW_MAX
#define FLOW_CHART_MAX_POINTS 100
#define FLOW_CHART_REFRESH_TIME 150
#define FLOW_CHART_LINE_MODE 1  /**< 0: No dots, only display line. 1: Use dots as data points */
#define FLOW_CHART_DOT_SIZE 5   /**< Dot LxW in px. Only applies to LINE_MODE 1 (Default: 5)*/
#define FLOW_CHART_LINE_WIDTH 2 /**< Changes the width of the line on the graph (Default: 2)*/

#endif