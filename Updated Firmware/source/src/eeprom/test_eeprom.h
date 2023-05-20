#ifndef TEST_EEPROM
#define TEST_EEPROM

/*
  Read and write settings and calibration data to an external I2C EEPROM
  By: Nathan Seidle
  SparkFun Electronics
  Date: December 11th, 2019
  License: This code is public domain but you buy me a beer if you use this
  and we meet someday (Beerware license).
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14764
  This example demonstrates how to record various user settings easily to EEPROM.
  The I2C EEPROM should have all its ADR pins set to GND (0). This is default
  on the Qwiic board.
  Hardware Connections:
  Plug the SparkFun Qwiic EEPROM to an Uno, Artemis, or other Qwiic equipped board
  Load this sketch
  Open output window at 115200bps
*/

#include "../config/uvent_conf.h"
#include "SparkFun_External_EEPROM.h"// Click here to get the library: http://librarymanager/All#SparkFun_External_EEPROM
#include <Wire.h>
#include <exception>

//This is the struct that contains all the user settings. Add as many vars as you want.
struct struct_userSettings {
    double degrees;
    const char* serial_number;
};

typedef enum class return_code {
    success,
    write_read_mem_fail,
    settings_mem_fail,
    struct_mem_fail,
    erased_memory,
    fail
} Return_code;

class test_eeprom {
public:
    // Pick the test to run
    // Three options

    // 1: write_read
    // This example demonstrates how to read and write various variables to memory.

    // 2: settings
    //This example demonstrates how to set the various settings for a given EEPROM.
    //Read the datasheet! Each EEPROM will have specific values for:
    //Overall EEPROM size in bytes (512kbit = 64000, 256kbit = 32000)
    //Bytes per page write (64 and 128 are common)
    //Whether write polling is supported

    // 3 : struct
    //This example demonstrates how to record various user settings easily to EEPROM.

    // 4: erase memory
    // This test erases all the memory on the eeprom
    void select_test(void);

private:
    ExternalEEPROM external_eeprom;

    //These are the default settings for each variable. They will be written if the EEPROM is blank.
    struct_userSettings settings = {
            .degrees = 54.23343,
            .serial_number = "PG002"};

    Return_code test_write_read(void);

    Return_code test_settings(void);

    Return_code test_struct(void);

    Return_code setup(void);

    void waitForInput(void);

    double getNumber(void);

    void loadUserSettings(void);

    void recordUserSettings(void);
};

#endif//TEST_EEPROM