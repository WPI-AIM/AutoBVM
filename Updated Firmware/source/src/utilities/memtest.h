#ifndef UVENT_MEMTEST_H
#define UVENT_MEMTEST_H

#include <malloc.h>
#include "logging.h"

extern char _end;
extern "C" char* sbrk(int i);
const char* ramstart = (char*) 0x20070000;
const char* ramend = (char*) 0x20088000;

inline void printMem()
{
    char* heapend = sbrk(0);
    register char* stack_ptr asm ("sp");
    struct mallinfo mi = mallinfo();
    serial_printf("------------------------\n");
    serial_printf("Dynamic ram used: %d bytes\n", mi.uordblks);
    serial_printf("Program static ram used %d bytes\n", &_end-ramstart);
    serial_printf("Stack ram used %d bytes\n", ramend-stack_ptr);
    serial_printf("My guess at free mem: %d bytes\n", stack_ptr-heapend+mi.fordblks);
    serial_printf("------------------------\n\n");

}

#endif //UVENT_MEMTEST_H
