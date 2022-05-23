#include "test_eeprom.h"

void test_eeprom::select_test(void)
{
    Serial.begin(115200);
    Wire.begin();

    Serial.println("WARNING: THIS TEST WILL REWRITE MEMORY ON THE EEPROM.");
    Serial.println("WARNING: BE SURE THAT YOU HAVE NOTHING SAVED IN THE TESTING MEMORY ADDRESS LOCATIONS.");
    Serial.println("WARNING: THE TESTING MEMORY ADDRESS LOCATIONS ARE STORED IN uvent_conf.h.");
    Serial.println("WARNING: REFERENCE uvent_conf.h FOR SPECIFIC MEMORY ADDRESS LOCATIONS.");
    Serial.println();
    Serial.println("Select Test:");
    Serial.println("Enter 1, 2, or 3");
    Serial.println("(1) test_write_read");
    Serial.println("(2) test_settings");
    Serial.println("(3) test_struct");

    int choice = getNumber();
    Return_code code;

    switch (choice) {
    case 1:
        Serial.println("\n\n\n");
        code = test_write_read();
        break;
    case 2:
        Serial.println("\n\n\n");
        code = test_settings();
        break;
    case 3:
        Serial.println("\n\n\n");
        code = test_struct();
        break;
    default:
        Serial.println("\n");
        Serial.print("Selection unavailable");
        Serial.println("\n\n\n");
        code = Return_code::fail;
        break;
    }

    if (code != Return_code::success) {
        switch (code) {
        case Return_code::write_read_mem_fail:
            Serial.println("FAIL: write_read_test memory failure");
            break;
        case Return_code::settings_mem_fail:
            Serial.println("FAIL: settings test memory failure");
            break;
        case Return_code::struct_mem_fail:
            Serial.println("FAIL: struct_test memory failure");
            break;
        case Return_code::erased_memory:
            Serial.println("Successfully erased all memory on the eeprom");
            Serial.println("\n\n\n");
            break;
        default:
            Serial.println("FAIL");
            break;
        }
    }
}

Return_code test_eeprom::test_write_read(void)
{
    Serial.println("write read EEPROM example");

    if (external_eeprom.begin() == false) {
        Serial.println("No memory detected. Freezing.");
        return Return_code::write_read_mem_fail;
    }
    Serial.println("Memory detected!");

    Serial.print("Mem size in bytes: ");
    Serial.println(external_eeprom.length());

    //Yes you can read and write bytes, but you shouldn't!
    byte myValue1 = 200;
    external_eeprom.write(EEPROM_TEST_1_MEM_1, myValue1);//(location, data)

    byte myRead1 = external_eeprom.read(EEPROM_TEST_1_MEM_1);
    Serial.println("Expect byte \"200\".");
    Serial.print("I read: ");
    Serial.println(myRead1);

    //You should use gets and puts. This will automatically and correctly arrange
    //the bytes for larger variable types.
    int myValue2 = -366;
    external_eeprom.put(EEPROM_TEST_1_MEM_2, myValue2);//(location, data)
    int myRead2;
    external_eeprom.get(EEPROM_TEST_1_MEM_2, myRead2);//location to read, thing to put data into
    Serial.println("Expect int \"-366\".");
    Serial.print("I read: ");
    Serial.println(myRead2);

    float myValue3 = 43.22;
    external_eeprom.put(EEPROM_TEST_1_MEM_3, myValue3);//(location, data)
    float myRead3;
    external_eeprom.get(EEPROM_TEST_1_MEM_3, myRead3);//location to read, thing to put data into
    Serial.println("Expect float \"43.22\".");
    Serial.print("I read: ");
    Serial.println(myRead3);

    double myValue4 = -7.355454;
    external_eeprom.put(EEPROM_TEST_1_MEM_4, myValue4);//(location, data)
    double myRead4;
    external_eeprom.get(EEPROM_TEST_1_MEM_4, myRead4);//location to read, thing to put data into
    Serial.println("Expect double \"-7.355454\".");
    Serial.print("I read: ");
    Serial.println(myRead4, 6);

    const char* myValue5 = "hello worlds 3.14";
    external_eeprom.put(EEPROM_TEST_1_MEM_5, myValue5);//(location, data)
    const char* myRead5;
    external_eeprom.get(EEPROM_TEST_1_MEM_5, myRead5);//location to read, thing to put data into
    Serial.println("Expect string \"hello worlds 3.14\".");
    Serial.print("I read: ");
    Serial.println(myRead5);
    Serial.println("\n\n\n");

    return Return_code::success;
}

Return_code test_eeprom::test_settings(void)
{
    delay(10);
    Serial.println("I2C/Settings EEPROM example");

    Wire.setClock(400000);//Most EEPROMs can run 400kHz and higher

    if (external_eeprom.begin() == false) {
        Serial.println("No memory detected. Freezing.");
        return Return_code::settings_mem_fail;
    }
    Serial.println("Memory detected!");

    if (external_eeprom.isConnected(EEPROM_ADDRESS)) {
        Serial.print("EEPROM on address: ");
        Serial.print(EEPROM_ADDRESS);
        Serial.println(" Is connected!!");
    }
    else {
        Serial.print("EEPROM on address: ");
        Serial.print(EEPROM_ADDRESS);
        Serial.println(" Is NOT connected!!");
        Serial.println("FAILURE to detect EEPROM on specified configuration address");
    }

    int memSize = external_eeprom.getMemorySize();
    Serial.print("Mem size returned from function getMemorySize: ");
    Serial.println(memSize);

    //Set settings for this EEPROM
    external_eeprom.setMemorySize(512000 / 8);   //In bytes. 512kbit = 64kbyte
    external_eeprom.setPageSize(128);            //In bytes. Has 128 byte page size.
    external_eeprom.enablePollForWriteComplete();//Supports I2C polling of write completion
    external_eeprom.setPageWriteTime(3);         //3 ms max write time

    Serial.print("Mem size in bytes (set): ");
    Serial.println(external_eeprom.length());
    Serial.println("\n\n\n");

    return Return_code::success;
}

Return_code test_eeprom::test_struct(void)
{
    Return_code code;
    code = setup();
    if (code != Return_code::success) {
        return code;
    }
    return code;
}

Return_code test_eeprom::setup(void)
{
    delay(10);
    Serial.println(F("Struct EEPROM example"));

    if (external_eeprom.begin() == false) {
        Serial.println(F("No memory detected. Freezing."));
        return Return_code::struct_mem_fail;
    }

    Serial.println(F("Memory detected!"));
    Serial.print("Size of user settings (bytes): ");
    uint32_t size = sizeof(settings);
    Serial.println(size);

    Serial.println("Record user settings to the eeprom");
    recordUserSettings();
    Serial.println("...");

    settings.degrees = 0.0;
    settings.serial_number = "If you get this for serial_number or '0.0' for degrees then this test failed.";

    Serial.println("Load user settings from the eeprom");
    loadUserSettings();

    Serial.println("...");
    Serial.println("Expect 54.23343");
    Serial.print("I read: ");
    Serial.println(settings.degrees, 5);

    Serial.println("Expect \"PG002\"");
    Serial.print("I read: ");
    Serial.println(settings.serial_number);

    //Now we can change something
    settings.degrees = 23.600;
    settings.serial_number = "SQ1232PG002";
    Serial.println("Record new settings to eeprom");
    //Now we can save it
    recordUserSettings();

    settings.degrees = 0.0;
    settings.serial_number = "If you get this for serial_number or '0.0' for degrees then this test failed.";

    Serial.println("...");
    Serial.println("Load new settings from eeprom....");
    loadUserSettings();
    Serial.println("...");
    Serial.println("Expect 23.600");
    Serial.print("I read: ");
    Serial.println(settings.degrees, 3);

    Serial.println("Expect \"SQ1232PG002\"");
    Serial.print("I read: ");
    Serial.println(settings.serial_number);

    //Reset settings to default for next test
    settings.degrees = 54.23343;
    settings.serial_number = "PG002";
    Serial.println("\n\n\n");

    return Return_code::success;
}

//Blocking wait for user input
void test_eeprom::waitForInput()
{
    delay(10);                                   //Wait for any incoming chars to hit buffer
    while (Serial.available() > 0) Serial.read();//Clear buffer
    while (Serial.available() == 0)
        ;
}

//Get a string/value from user, remove all non-numeric values
double test_eeprom::getNumber()
{
    waitForInput();//Wait for user to send a value

    //Get input from user
    char cleansed[20];

    int spot = 0;
    while (spot < 20 - 1)//Leave room for terminating \0
    {
        while (Serial.available() == 0)
            ;//Wait for user input

        byte incoming = Serial.read();
        if (incoming == '\n' || incoming == '\r') {
            Serial.println();
            break;
        }

        if (isDigit(incoming) == true) {
            Serial.write(incoming);//Echo user's typing
            cleansed[spot++] = (char) incoming;
        }
    }

    cleansed[spot] = '\0';

    String tempValue = cleansed;
    return (tempValue.toDouble());
}

//Load the current settings from EEPROM into the settings struct
void test_eeprom::loadUserSettings()
{
    //Read current settings
    Serial.println("loading...");
    external_eeprom.get(EEPROM_TEST_2_MEM_1, settings);
}

//Record the current settings into EEPROM
void test_eeprom::recordUserSettings()
{
    Serial.println("recording...");
    external_eeprom.put(EEPROM_TEST_2_MEM_1, settings);//That'
}