#ifndef UVENT_STORAGE_H
#define UVENT_STORAGE_H

#include "../config/uvent_conf.h"
#include "SparkFun_External_EEPROM.h"

// UVent settings structure
struct __attribute__((packed)) uvent_settings {
    /* Julian Date: YYMMDD
     * Lot:         L: A,B,C...
     * Serial:      XXXX
     */
    char serial[13];            // YYMMDDLXXXX
    uint16_t diff_pressure_type;// 0: ±0.07, 1: ±0.09
    uint16_t actuator_home_offset_adc_counts;
    /***************************************************/
    /*             Adjustable Value Storage            */
    /***************************************************/
    uint16_t tidal_volume;
    uint8_t respiration_rate;
    uint8_t peep_limit;
    uint8_t pip_limit;
    uint16_t plateau_time;
    double ie_ratio_left;
    double ie_ratio_right;
};

class Storage {
public:
    bool init();
    bool is_crc_ok();
    void load_defaults();
    void get_settings(uvent_settings&);
    void set_settings(uvent_settings&);
    void display_storage();

private:
    ExternalEEPROM external_eeprom;

    // Defaults cal. settings
    uvent_settings def_settings = {
            .serial = UVENT_SERIAL_IDENTIFIER,
            .diff_pressure_type = PRESSURE_SENSOR_TYPE_0,
            .actuator_home_offset_adc_counts = 0,
            .tidal_volume = (uint16_t) (DEF_BAG_VOL_ML),
            .respiration_rate = DEF_BPM,
            .peep_limit = DEF_PEEP,
            .pip_limit = DEF_PIP,
            .plateau_time = DEF_PLATEAU,
            .ie_ratio_left = DEF_IE,
            .ie_ratio_right = DEF_IE,
    };

    uint32_t crc_calculate();
};

#endif