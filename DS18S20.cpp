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

#define DS18S20_TEMPERATURE_CONVERSION_COMMAND 0x44
#define DS18S20_READ_SCRATCHPAD_COMMAND 0xbe
#define DS18S20_TEMPERATURE_CONVERSION_DELAY 750

DS18S20::DS18S20(OneWire *ow, uint8_t *address) : OneWireTemperatureDevice(ow, address) {
}

void DS18S20::update() {
    OneWireTemperatureDevice::update();

    uint8_t data[9];
    ow->reset();
    ow->select(address);
    ow->write(DS18S20_TEMPERATURE_CONVERSION_COMMAND, 0);
    vTaskDelay(DS18S20_TEMPERATURE_CONVERSION_DELAY / portTICK_PERIOD_MS);
    ow->reset();
    ow->select(address);
    ow->write(DS18S20_READ_SCRATCHPAD_COMMAND, 0);

    for (int i = 0; i < 9; i++) {
      data[i] = ow->read();
    }

    error = data[8] != ow->crc8(data, 8);

    if (!error) {
        uint8_t lsb = data[0];
        uint8_t msb = data[1];
        int16_t temp = (msb << 8) + lsb;
        uint8_t sign = temp & 0x8000;
        if (sign) {
            temp = (temp ^ 0xffff) + 1;
        }

        temperature = (double)temp / 2.0 + 0.05;
        if (sign) {
           temperature = 0.0 - temperature;
        }
    }
}
