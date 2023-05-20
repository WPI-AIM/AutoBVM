#include "command.h"
#include "controls/control.h"
#include "controls/machine.h"
#include "controls/waveform.h"
#include "utilities/logging.h"
#include <Arduino.h>
#include <limits.h>

enum class Error_Codes {
    ER_NONE,
    ER_NOT_ENOUGH_ARGS,
    ER_INVALID_ARG
};

static void command_help(int argc, char** argv);
static void command_actuator(int argc, char** argv);
static void command_state(int argc, char** argv);
static void command_eeprom(int argc, char** argv);
static void command_waveform(int argc, char** argv);
static void command_pressure(int argc, char** argv);
static void command_alarm(int argc, char** argv);
static void command_fault(int argc, char** argv);

/* Command response, with error code. */
static void print_response(Error_Codes error)
{
    serial_printf("e %d\n", error);
}

/* Convert the incoming string to a long(using strtol).
 * Return false if the string cannot be parsed to a long.
 */
inline bool sanitize_input(const char* str, int32_t* result)
{
    char* pEnd;// Pointer for strtol use.

    *result = strtol(str, &pEnd, 10);// Base 10

    /* If the end pointer is the same as the start of str pointer,
     * no parsing has taken place. Return false, else parsing was a success.
     */
    return !(pEnd == str);
}

/* Convert the incoming string to a float(using strtof).
 * Return false if the string cannot be parsed to a long.
 */
inline bool sanitize_input(const char* str, float* result)
{
    char* pEnd;// Pointer for strtol use.

    *result = strtof(str, &pEnd);

    /* If the end pointer is the same as the start of str pointer,
     * no parsing has taken place. Return false, else parsing was a success.
     */
    return !(pEnd == str);
}

/* Used by the callee to repeatedly print the requested value.
 */
bool repeat_break()
{
    delay(100);
    // read the incoming byte:
    return (Serial.read() == '\r');
}

/* Command list with their associated help text */
command_type commands[] =
        {
                {"help", command_help, "\t\tDisplay a list of commands.\r\n"},
                {"actuator", command_actuator, "\tActuator related commands.\r\n"},
                {"state", command_state, "\t\tState related commands.\r\n"},
                {"ee", command_eeprom, "\t\tEEPROM related commands.\r\n"},
                {"wave", command_waveform, "\t\tWaveform related commands.\r\n"},
                {"press", command_pressure, "\t\tPressure related commands.\r\n"},
                {"fault", command_fault, "\t\tForce a fault.\r\n"},
                {"alarm", command_alarm, "\t\\Alarm related commands.\r\n"}};

uint16_t const command_array_size = sizeof(commands) / sizeof(command_type);

/* Help function. */
static void
command_help(int argc, char** argv)
{
    unsigned int i;

    for (i = 0; i < command_array_size; i++) {
        Serial.print(commands[i].name);
        Serial.print(commands[i].help);
    }
}

/* Force a fault(testing). */
static void
command_fault(int argc, char** argv)
{
    if (argc < 2) {
        // Not enough arguments.
        print_response(Error_Codes::ER_NOT_ENOUGH_ARGS);
        return;
    }
    if (!(strcmp(argv[1], "help")) || (argc == 1)) {
        Serial.println("Format: fault id_no");
        Serial.println("1 - Actuator");
        return;
    }

    int32_t fault_id;

    // Check if the strings can be parsed. If False, abort.
    if (!(sanitize_input(argv[1], &fault_id))) {
        print_response(Error_Codes::ER_INVALID_ARG);
        return;
    }

    // Only do actuator fault. Others are not supported
    if (((Fault) fault_id != Fault::FT_ACTUATOR_FAULT)) {
        print_response(Error_Codes::ER_INVALID_ARG);
        return;
    }

    control_set_fault((Fault) fault_id);
}

/* Actuator function. */
static void
command_actuator(int argc, char** argv)
{
    // Check is help is requested for this command or no arguments were included.
    if (!(strcmp(argv[1], "help")) || (argc == 1)) {
        Serial.println("Format: actuator command");
        Serial.println("home     - Homes the paddle.");
        Serial.println("zero     - Sets the current actuator position as home.");
        Serial.println("pos      - Gets the current actuator position.");
        Serial.println("pos_raw  - Gets the current raw data from the angle sensor .");
        Serial.println("mv_deg   - Moves the actuator to a position(degrees).");
        Serial.println("mv_steps - Moves the actuator by no. of steps(steps).");
        Serial.println("volume   - Get the tidal volume from the Ambu Bag (liters).");
        Serial.println("enable   - Enable/Disable the drive.");
        return;
    }
    else if (!(strcmp(argv[1], "home"))) {
        control_change_state(States::ST_ACTUATOR_HOME);
        print_response(Error_Codes::ER_NONE);
        return;
    }
    else if (!(strcmp(argv[1], "zero"))) {
        control_zero_actuator_position();
        print_response(Error_Codes::ER_NONE);
        return;
    }
    else if (!(strcmp(argv[1], "pos"))) {
        if (!(strcmp(argv[2], "r"))) {
            // Repeat requested. Print till Enter is pressed.
            while (!repeat_break()) {
                Serial.println(control_get_actuator_position(), DEC);
            }
        }
        else {
            Serial.println(control_get_actuator_position(), DEC);
        }
        return;
    }
    else if (!(strcmp(argv[1], "pos_raw"))) {
        double angle;
        int8_t ret = control_get_actuator_position_raw(angle);
        if (ret != -1) {
            Serial.println(angle, DEC);
        }

        return;
    }
    else if (!(strcmp(argv[1], "mv_deg"))) {
        if (argc == 2) {// Not enough arguments.
            print_response(Error_Codes::ER_NOT_ENOUGH_ARGS);
            return;
        }
        if (!(strcmp(argv[2], "help"))) {
            Serial.println("Format: mv_deg angle speed");
            Serial.println("angle - The angle(int) of the shaft in degrees");
            Serial.println("speed - The speed(int) in degrees per second.");
            return;
        }

        int32_t req_angle;
        int32_t req_speed;

        // Check if the strings can be parsed. If False, abort.
        if (!(sanitize_input(argv[2], &req_angle)) || !(sanitize_input(argv[3], &req_speed))) {
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }

        // Values should be between 0 and 360 degrees.
        if ((req_angle > 360) || (req_angle < -360)) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        else if ((req_speed > 180) || (req_speed < 0)) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        else {
            //change state to jog
            control_change_state(States::ST_ACTUATOR_JOG);
            control_actuator_manual_move(Tick_Type::TT_DEGREES, req_angle, req_speed);
        }
    }
    else if (!(strcmp(argv[1], "mv_steps"))) {
        if (argc == 2) {
            // Not enough arguments.
            print_response(Error_Codes::ER_NOT_ENOUGH_ARGS);
            return;
        }
        if (!(strcmp(argv[2], "help"))) {
            Serial.println("Format: mv_steps angle speed");
            Serial.println("angle - The angle(int) of the shaft in degrees");
            Serial.println("speed - The speed(int) in degrees per second.");
            return;
        }

        int32_t req_steps;
        int32_t req_speed;

        // Check if the strings can be parsed. If False, abort.
        if (!(sanitize_input(argv[2], &req_steps)) || !(sanitize_input(argv[3], &req_speed))) {
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }

        if ((req_steps > LONG_MAX) || (req_steps < LONG_MIN)) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        else if ((req_speed > STEPPER_MAX_STEPS_PER_SECOND) || (req_speed < -STEPPER_MAX_STEPS_PER_SECOND)) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        else {
            // Change state to jog
            control_change_state(States::ST_ACTUATOR_JOG);
            control_actuator_manual_move(Tick_Type::TT_STEPS, req_steps, req_speed);
        }
    }
    else if (!(strcmp(argv[1], "volume"))) {
        if (argc == 2) {
            // Not enough arguments.
            print_response(Error_Codes::ER_NOT_ENOUGH_ARGS);
            return;
        }
        if (!(strcmp(argv[2], "help"))) {
            Serial.println("Format: volume compliance");
            Serial.println("compliance - The compliance of the artificial lung.");
            Serial.println("Options are: none, 20, or 50");
            return;
        }

        if ((strcmp(argv[2], "none")) && (strcmp(argv[2], "None")) && (strcmp(argv[2], "NONE")) && (strcmp(argv[2], "20")) && (strcmp(argv[2], "50"))) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }

        int dec_place = 3;
        if (!(strcmp(argv[2], "none")) || !(strcmp(argv[2], "None")) || !(strcmp(argv[2], "NONE"))) {
            Serial.println(control_get_degrees_to_volume(C_Stat::NONE), dec_place);
            return;
        }
        else if (!(strcmp(argv[2], "20"))) {
            if (!(strcmp(argv[2], "r"))) {
                // Repeat requested. Print till Enter is pressed.
                while (!repeat_break()) {
                    Serial.println(control_get_degrees_to_volume(C_Stat::TWENTY), dec_place);
                }
            }
            else {
                Serial.println(control_get_degrees_to_volume(C_Stat::TWENTY), dec_place);
            }
            return;
        }
        else if (!(strcmp(argv[2], "50"))) {
            if (!(strcmp(argv[2], "r"))) {
                // Repeat requested. Print till Enter is pressed.
                while (!repeat_break()) {
                    Serial.println(control_get_degrees_to_volume(C_Stat::FIFTY), dec_place);
                }
            }
            else {
                Serial.println(control_get_degrees_to_volume(C_Stat::FIFTY), dec_place);
            }
            return;
        }
    }
    else if (!(strcmp(argv[1], "enable"))) {
        if (!(strcmp(argv[2], "help"))) {
            Serial.println("Format: enable 0/1");
            return;
        }
        else if (!(strcmp(argv[2], "1"))) {
            control_actuator_set_enable(true);
            print_response(Error_Codes::ER_NONE);
            return;
        }
        else if (!(strcmp(argv[2], "0"))) {
            control_actuator_set_enable(false);
            print_response(Error_Codes::ER_NONE);
            return;
        }
        else {
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
    }
}

/* State function. */
static void
command_state(int argc, char** argv)
{
    // Check is help is requested for this command or no arguments were included.
    if (!(strcmp(argv[1], "help")) || (argc == 1)) {
        Serial.println("Format: state command");
        Serial.println("list      - Lists all the states.");
        Serial.println("which     - Returns the current state ID.");
        Serial.println("which_str - Returns the current state string.");
        Serial.println("switch    - Force switch to a state.");
    }
    else if (!(strcmp(argv[1], "which"))) {
        Serial.println((uint16_t) control_get_state());
        return;
    }
    else if (!(strcmp(argv[1], "which_str"))) {
        Serial.println(control_get_state_string());
        return;
    }
    else if (!(strcmp(argv[1], "list"))) {

        uint8_t state_list_size = 0;
        const char** p = control_get_state_list(&state_list_size);

        // Spool through the list.
        for (uint8_t i = 0; i < state_list_size; i++) {
            // Print the index and state string
            serial_printf("%d: %s\r\n", i, *p);
            p++;
        }

        return;
    }
    else if (!(strcmp(argv[1], "switch"))) {
        if (argc == 2) {
            // Not enough arguments.
            print_response(Error_Codes::ER_NOT_ENOUGH_ARGS);
            return;
        }
        if (!(strcmp(argv[2], "help"))) {
            Serial.println("Format: switch state_id");
            Serial.println("state_id - The index of the state");
            return;
        }

        int32_t req_state;

        // Check if the strings can be parsed. If False, abort.
        if (!(sanitize_input(argv[2], &req_state))) {
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }

        // Values should be between 0 max state count.
        if ((req_state > (int16_t) States::ST_COUNT) || (req_state < 0)) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        else {
            //change state to jog
            control_change_state((States) req_state);
        }
    }
}

/* EEPROM function. */
static void
command_eeprom(int argc, char** argv)
{
    // Check is help is requested for this command or no arguments were included.
    if (!(strcmp(argv[1], "help")) || (argc == 1)) {
        Serial.println("Format: state command");
        Serial.println("dump      - Dumps eeprom contents.");
    }
    else if (!(strcmp(argv[1], "dump"))) {
        control_display_storage();
        return;
    }
}

/* Waveform function. */
static void
command_waveform(int argc, char** argv)
{
    waveform_params* p_wave_params = control_get_waveform_params();

    // Check is help is requested for this command or no arguments were included.
    if (!(strcmp(argv[1], "help")) || (argc == 1)) {
        Serial.println("Format: state command");
        Serial.println("dump      - Dumps waveform details.");
        Serial.println("bpm       - Breaths per minute");
        Serial.println("vt        - Tidal volume.");
        Serial.println("ie        - IE ratio.");
        Serial.println("pip       - Peak inspiratory pressure.");
        Serial.println("peep      - Peak end expiratory pressure.");
    }
    else if (!(strcmp(argv[1], "dump"))) {

        control_waveform_display_details();
        return;
    }
    else if (!(strcmp(argv[1], "bpm"))) {
        if (argc < 3) {// Not enough arguments.
            print_response(Error_Codes::ER_NOT_ENOUGH_ARGS);
            return;
        }

        if (!(strcmp(argv[2], "help"))) {
            Serial.println("Format: bpm value");
            return;
        }

        int32_t bpm;

        // Check if the strings can be parsed. If False, abort.
        if (!(sanitize_input(argv[2], &bpm))) {
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }

        // Sanitize
        if ((bpm > BPM_MAX) || (bpm < BPM_MIN)) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        else {
            p_wave_params->bpm = bpm;
            control_calculate_waveform();
            print_response(Error_Codes::ER_NONE);
        }
    }
    else if (!(strcmp(argv[1], "vt"))) {
        if (argc < 3) {// Not enough arguments.
            print_response(Error_Codes::ER_NOT_ENOUGH_ARGS);
            return;
        }

        if (!(strcmp(argv[2], "help"))) {
            Serial.println("Format: Vt value");
            return;
        }

        float vt;

        // Check if the strings can be parsed. If False, abort.
        if (!(sanitize_input(argv[2], &vt))) {
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }

        // Sanitize
        if ((vt > (MAX_BAG_VOL_L * 1000)) || (vt < (MIN_BAG_VOL_L * 1000))) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        else {
            p_wave_params->volume_ml = vt;
            control_calculate_waveform();
            print_response(Error_Codes::ER_NONE);
        }
    }
    else if (!(strcmp(argv[1], "ie"))) {
        if (argc < 4) {// Not enough arguments.
            print_response(Error_Codes::ER_NOT_ENOUGH_ARGS);
            return;
        }

        if (!(strcmp(argv[2], "help"))) {
            Serial.println("Format: ie i e");
            Serial.println("eg: ie 1.0 1.5");
            return;
        }

        float ie_i;
        float ie_e;

        // Check if the strings can be parsed. If False, abort.
        if (!(sanitize_input(argv[2], &ie_i))) {
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        if (!(sanitize_input(argv[3], &ie_e))) {
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        // Sanitize
        if ((ie_i > IE_MAX) || (ie_i < IE_MIN) || (ie_e > IE_MAX) || (ie_e < IE_MIN)) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        else {
            p_wave_params->ie_i = ie_i;
            p_wave_params->ie_e = ie_e;
            control_calculate_waveform();
            print_response(Error_Codes::ER_NONE);
        }
    }
    else if (!(strcmp(argv[1], "pip"))) {
        if (argc < 3) {// Not enough arguments.
            print_response(Error_Codes::ER_NOT_ENOUGH_ARGS);
            return;
        }

        if (!(strcmp(argv[2], "help"))) {
            Serial.println("Format: pip value");
            return;
        }

        int32_t pip;

        // Check if the strings can be parsed. If False, abort.
        if (!(sanitize_input(argv[2], &pip))) {
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }

        // Sanitize
        if ((pip > PIP_MAX) || (pip < PIP_MIN)) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        else {
            p_wave_params->pip = pip;
            control_calculate_waveform();
            print_response(Error_Codes::ER_NONE);
        }
    }
    else if (!(strcmp(argv[1], "peep"))) {
        if (argc < 3) {// Not enough arguments.
            print_response(Error_Codes::ER_NOT_ENOUGH_ARGS);
            return;
        }

        if (!(strcmp(argv[2], "help"))) {
            Serial.println("Format: peep value");
            return;
        }

        int32_t peep;

        // Check if the strings can be parsed. If False, abort.
        if (!(sanitize_input(argv[2], &peep))) {
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }

        // Sanitize
        if ((peep > PEEP_MAX) || (peep < PEEP_MIN)) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        else {
            p_wave_params->peep = peep;
            control_calculate_waveform();
            print_response(Error_Codes::ER_NONE);
        }
    }
}

/* Pressure function. */
static void
command_pressure(int argc, char** argv)
{
    // Check is help is requested for this command or no arguments were included.
    if (!(strcmp(argv[1], "help")) || (argc == 1)) {
        Serial.println("Format: press command");
        Serial.println("gauge      - Dumps gauge pressure.");
        Serial.println("diff       - Dumps diff pressure");
    }
    else if (!(strcmp(argv[1], "gauge"))) {
        if (!(strcmp(argv[2], "r"))) {
            // Repeat requested. Print till Enter is pressed.
            while (!repeat_break()) {
                Serial.println(control_get_gauge_pressure(), DEC);
            }
        }
        else {
            Serial.println(control_get_gauge_pressure(), DEC);
        }
        return;
    }
    else if (!(strcmp(argv[1], "diff"))) {
        if (!(strcmp(argv[2], "r"))) {
            // Repeat requested. Print till Enter is pressed.
            while (!repeat_break()) {
                Serial.println(control_get_diff_pressure(), DEC);
            }
        }
        else {
            Serial.println(control_get_diff_pressure(), DEC);
        }
        return;
    }
}

command_type* command_get_array(void)
{
    return commands;
}

uint16_t command_get_array_size()
{
    return command_array_size;
}

/* Alarm function. */
static void
command_alarm(int argc, char** argv)
{
    if (!(strcmp(argv[1], "help")) || (argc == 1)) {
        Serial.println("Format: state command");
        Serial.println("snooze   - Snoozes alarm. Can be used to toggle.");
        Serial.println("count    - Displays no. of current alarms.");
        Serial.println("text     - Displays text of current alarm.");
        Serial.println("alloff   - Turns off all alarms.");
        Serial.println("list     - Get a list of alarms.");
        Serial.println("test     - Trigger an emergency Over Current alarm.");
    }
    else if (!(strcmp(argv[1], "snooze"))) {
        control_alarm_snooze();
        print_response(Error_Codes::ER_NONE);
        return;
    }
    else if (!(strcmp(argv[1], "count"))) {
        Serial.println(control_get_alarm_count());
        return;
    }
    else if (!(strcmp(argv[1], "text"))) {
        String a_text = control_get_alarm_text();
        if (a_text) {
            Serial.println(a_text);
        }
        else {
            Serial.println("No alarms");
        }
        return;
    }
    else if (!(strcmp(argv[1], "alloff"))) {
        control_set_alarm_all_off();
        print_response(Error_Codes::ER_NONE);
        return;
    }
    else if (!(strcmp(argv[1], "list"))) {
        Alarm* p_alarm_list = control_get_alarm_list();

        for (int i = 0; i < NUM_ALARMS; i++) {
            Serial.print(p_alarm_list->isON());
            Serial.println(p_alarm_list->text());
            p_alarm_list++;
        }

        return;
    }
    else if (!(strcmp(argv[1], "test"))) {
        control_alarm_test();
        print_response(Error_Codes::ER_NONE);
        return;
    }
}