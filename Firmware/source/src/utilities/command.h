#ifndef UVENT_COMMAND_H
#define UVENT_COMMAND_H

#include <Arduino.h>

typedef struct
{
    const char* name;
    void (*function)(int argc, char** argv);
    const char* help;
} command_type;

extern command_type* command_get_array();
extern uint16_t command_get_array_size();
extern command_type commands[];

#endif//UVENT_COMMAND_H