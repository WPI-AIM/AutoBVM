/*
 * parser.cpp
 *      Author: BEC
 */

#include <Arduino.h>
#include "parser.h"
#include "../config/uvent_conf.h"

/* Initialize the parser.
 * Show the MOTD and reset the command buffer.
 */
void Parser::init(command_type* pComArr, uint16_t comArrSize)
{
    // MOTD
    Serial.println("Universal Ventilator Controls");
    Serial.print(__TIME__);
    Serial.print(" ");
    Serial.println(__DATE__);
    char buf[20];
    snprintf(buf, sizeof(buf), "Version: %d.%d.%d", UVENT_VERSION_MAJOR, UVENT_VERSION_MINOR, UVENT_VERSION_PATCH);
    Serial.println(buf);
    Serial.println("----------------------------------");

    // Reset the command payload
    memset(&payload, 0, sizeof(payload));

    // Serial.println(commandArr);
    p_command_array = pComArr;
    command_array_size = comArrSize;

    // Display the prompt
    prompt();
}

/* Look for data in the serial port.
 * If data is detected, handle it, else pop out.
 * This function is non-blocking.
 */
void Parser::service()
{
    // Read a single byte from the serial port
    if (Serial.available() > 0) {
        handle_input(Serial.read());
    }
}

/* Accept characters from the serial port into a buffer
 * If \r or \n is detected, process the input buffer.
 * If \b, backspace is detected, handle it.
 * Handle buffer overflow.
 */
void Parser::handle_input(char c)
{
    switch (c) {
        /* CR or LF detected. Parse the input buffer. */
        case '\r':
        case '\n':
            Serial.print(PARSER_NEXT_LINE);
            if (payload.index > 0) {
                argument_parse();
                payload.index = 0;

                // Command has been serviced, clear the payload.
                memset(payload.buf, 0, sizeof(payload.buf));
            }
            prompt();
            break;

        /* Backspace detected, terminate '\0' the captured buffer,
        * move input buffer index back by one,
        * issue backspace \b so that the cursor moves back,
        * display a "space" to clear the character,
        * issue backspace again.
        */
        case '\b':
            if (payload.index) {
                payload.buf[payload.index - 1] = '\0';
                payload.index--;
                Serial.print(c);
                Serial.print(' ');
                Serial.print(c);
            }
            break;
        /* This default case streams data back to the serial port,
         * as an echo. It also accumulates each character into the
         * payload buffer.
         */
        default:
            // Make sure, buffer does not overflow
            if (payload.index < PARSER_CMD_BUFFER_LEN - 1) {
                Serial.print(c);
                payload.buf[payload.index] = c;
                payload.index++;
            }
            else {
                Serial.println("Command buffer full");
                Serial.println("Unable to accecpt more characters.");
                Serial.println("Press ENTER to reset.");
            }
    }
}

/* This function parses the command on the serial port into a function call
    * and it's respective parameters. It looks for whitespaces between words and
    * arguments.
    *
    * argv : ARGument Vector
    * argc : ARGument Count, No. of strings pointed by argv
    *
    * The function can take a maximum of 16 parameters.
    *
    */
void Parser::argument_parse(void)
{
    uint16_t i;
    char* argv[PARSER_MAX_ARGUMENTS];
    int16_t argc = 0;
    char* in_arg = NULL;

    for (i = 0; i < payload.index; i++) {
        if (isspace(payload.buf[i]) && argc == 0)
            continue;

        if (isspace(payload.buf[i])) {
            if (in_arg) {
                payload.buf[i] = '\0';
                in_arg = NULL;
            }
        }
        else if (!in_arg) {
            in_arg = &payload.buf[i];
            argv[argc] = in_arg;
            argc++;
        }
    }
    payload.buf[i] = '\0';

    if (argc > 0)
        // Parsed into arguments. Find command in array and process it.
        process_command(argc, argv);
}

/* This function calls the appropriate function from the function
 * table.
 */
void Parser::process_command(uint16_t argc, char** argv)
{
    uint16_t i;
    // Length of first argument, which is the command
    uint16_t arg_com_len = strlen(argv[0]);

    // Length of the command in the command array
    uint16_t com_len;

    for (i = 0; i < command_array_size; i++) {
        com_len = strlen(p_command_array[i].name);

        /* Evaluate the command if:
         * - The incoming command length == Length of command in command array
         * - The function pointed to by the command is not NULL. Valid function is required.
         * - Incoming command name == Name in command array
         */
        if ((com_len == arg_com_len)
            && (p_command_array[i].function != NULL)
            && (!strncmp(argv[0], p_command_array[i].name, arg_com_len))) {
            p_command_array[i].function(argc, argv);
        }
    }
}