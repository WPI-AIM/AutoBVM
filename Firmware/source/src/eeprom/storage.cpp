#include "../config/uvent_conf.h"
#include <CRC32.h>
#include <utilities/logging.h>
#include "storage.h"

bool Storage::init()
{
    // Init the EEPROM
    if (external_eeprom.begin() == false) {
        Serial.println(F("No memory detected..."));
        return false;
    }

    return true;
}

void Storage::get_settings(uvent_settings& outset)
{
#if ENABLE_CONTROL
    external_eeprom.get(EXT_EEPROM_SETTINGS_LOC, outset);
#else
    Serial.println("In debug mode, defaults will be copied into the requested obj");
    memcpy(&outset, &def_settings, sizeof(uvent_settings));
#endif
}

void Storage::set_settings(uvent_settings& inset)
{
    external_eeprom.put(EXT_EEPROM_SETTINGS_LOC, inset);

    // Update the EEPROM CRC
    external_eeprom.put(EXT_EEPROM_CRC_LOC, crc_calculate());
}

void Storage::display_storage()
{
    uvent_settings temp_set;

    external_eeprom.get(EXT_EEPROM_SETTINGS_LOC, temp_set);

    Serial.println("---- UVENT SETTINGS ---");
    Serial.print("Serial no.: ");
    Serial.println(temp_set.serial);
    Serial.print("Diff. pressure Type: ");
    Serial.println(temp_set.diff_pressure_type);
    Serial.print("Actuator home offset: ");
    Serial.println(temp_set.actuator_home_offset_adc_counts);
    serial_printf("Tidal Volume: %d\n", temp_set.tidal_volume);
    serial_printf("Resp. Rate: %d\n", temp_set.respiration_rate);
    serial_printf("PEEP: %d\n", temp_set.peep_limit);
    serial_printf("PIP: %d\n", temp_set.pip_limit);
    serial_printf("Plateau: %d\n", temp_set.plateau_time);
    serial_printf("IE: %.1f : %.1f\n", temp_set.ie_ratio_left, temp_set.ie_ratio_right);
}

bool Storage::is_crc_ok()
{
    // First get the CRC from the EEPROM
    uint32_t stored_crc;
    external_eeprom.get(EXT_EEPROM_CRC_LOC, stored_crc);

    uint32_t calc_crc = crc_calculate();

    Serial.print("Stored CRC is:");
    Serial.print(stored_crc);

    Serial.print(", Calculated CRC is:");
    Serial.println(calc_crc);

    return (calc_crc == stored_crc);
}

/* Load the latest settings from the EEPROM and return the 
 * CRC.
 */
uint32_t Storage::crc_calculate()
{
    // Calculate a checksum one byte at a time.
    CRC32 crc;
    uint8_t byte_buffer[sizeof(uvent_settings)];

    // Get the latest data settings from the EEPROM.
    external_eeprom.get(EXT_EEPROM_SETTINGS_LOC, byte_buffer);

    // Generate the final CRC32 checksum.
    return (crc.calculate(byte_buffer, sizeof(uvent_settings)));
}

void Storage::load_defaults()
{
    // Write the defaults.
    external_eeprom.put(EXT_EEPROM_SETTINGS_LOC, def_settings);

    // Update the EEPROM CRC
    external_eeprom.put(EXT_EEPROM_CRC_LOC, crc_calculate());
}