#ifndef UVENT_PARSER_H
#define UVENT_PARSER_H

#include "command.h"

const String PARSER_PROMPT_STRING = ">";
const String PARSER_NEXT_LINE = "\r\n";

// The macimum number of characters in a command.
const uint16_t PARSER_CMD_BUFFER_LEN = 50;

// Commands are split into argv, argc.
// The below metric is the max argc, or max parameters in a command.
const uint16_t PARSER_MAX_ARGUMENTS = 16;

class Parser {
public:
    void init(command_type* pCommandArray, uint16_t commandArraySize);
    void service();

private:
    // Display the prompt
    void prompt() { Serial.print(PARSER_PROMPT_STRING); }

    // Accept characters from the serial port and form a payload.
    void handle_input(char c);

    // Split the incoming command into argc, argv format
    void argument_parse();

    // Look up the command from the command array
    void process_command(uint16_t argc, char** argv);

private:
    // As characters get typed in, it forms the payload.
    struct
    {
        char buf[PARSER_CMD_BUFFER_LEN];
        uint8_t index;
    } payload;

    command_type* p_command_array;

    uint16_t command_array_size;
};

#endif /* UVENT_PARSER_H */