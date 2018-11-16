/* DS18S20.cpp
 *
 * (C) 2018 Stephen Houser https://stephenhouser.com
 * (based on code by Joe Bechter -- (C) 2012, bechter.com)
 *
 * All files, software, schematics and designs are provided as-is with no 
 * warranty. All files, software, schematics and designs are for experimental
 * or hobby use. Under no circumstances should any part be used for critical 
 * systems where safety, life or property depends upon it. You are responsible 
 * for all use. You are free to use, modify, derive or otherwise extend for 
 * your own non-commercial purposes provided
 *  1. No part of this software or design may be used to cause injury or 
 *     death to humans or animals.
 *  2. Use is non-commercial.
 *  3. Credit is given to the author (i.e. portions © bechter.com), and provide
 *  a link to the original source.
 */

#include "DS18S20.h"

DS18S20::DS18S20(OneWire *ow, uint8_t *address) : OneWireTemperatureDevice(ow, address) {
}

void DS18S20::update() {
    uint8_t data[9];

    _timestamp = millis();
    _error = this->readTemperature(data);
    if (!_error) {
        uint8_t lsb = data[0];
        uint8_t msb = data[1];
        int16_t temp = (msb << 8) + lsb;
        uint8_t sign = temp & 0x8000;
        if (sign) {
            temp = (temp ^ 0xffff) + 1;
        }

        _temperature = (double)temp / 2.0 + 0.05;
        if (sign) {
           _temperature = 0.0 - _temperature;
        }
    }
}