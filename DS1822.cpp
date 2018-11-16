/* DS18S22.cpp
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

#include "DS1822.h"

DS1822::DS1822(OneWire *ow, uint8_t *address) : OneWireTemperatureDevice(ow, address) {
}

void DS1822::update() {
	uint8_t data[9];

	_timestamp = millis();
	_error = this->readTemperature(data);
	if (!_error) {
		int16_t raw = (data[1] << 8) | data[0];
		byte cfg = (data[4] & 0x60);

		// at lower res, the low bits are undefined, so let's zero them
		if (cfg == 0x00) {
            raw = raw & ~7; // 9 bit resolution, 93.75 ms
		} else if (cfg == 0x20) {
			raw = raw & ~3; // 10 bit res, 187.5 ms
		} else if (cfg == 0x40) {
			raw = raw &	~1; // 11 bit res, 375 ms
							// default is 12 bit resolution, 750 ms conversion time
		}

		_temperature = (float)raw / 16.0;
	}
}
