/* OneWireDevice.cpp
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

#include "OneWireDevice.h"
#include "OneWireTemperatureDevice.h"
#include "DS1822.h"
#include "DS18S20.h"
#include "OneWireDirection.h"
#include "OneWireSpeed.h"

OneWireDevice *OneWireDevice::objectForDevice(OneWire *ow, uint8_t *address) {
	OneWireDevice *device = NULL;
  	switch (address[0]) {
		case 0x10:
			Serial.println("DS18S20 Temp");
			device = new DS18S20(ow, address);
			break;
		case 0x1D:
			Serial.println("DS2423 RAM/Counter");
			//device = new DS2423(ow, address);
			device = new OneWireSpeed(ow, address);
			break;
		case 0x20:
			Serial.println("DS2450 Quad A/D");
			device = new OneWireDirection(ow, address);
			break;
		case 0x22:
			Serial.println("DS1822 Temp");
			device = new DS1822(ow, address);
			break;
		case 0x28:
			Serial.println("DS18B20 Temp");
			device = new DS1822(ow, address);
			break;
		case 0x29:
			Serial.println("DS2408 8 Switch");
			break;
		default:
            device = NULL;
			break;
  	}

    return device;
}

OneWireDevice::OneWireDevice(OneWire *ow, uint8_t *address) {
  	_ow = ow;

	// copy address
	for (byte i = 0; i < 8; i++) {
		this->_address[i] = address[i];
	}
}

void OneWireDevice::begin() {
  	_error = true;
  	_timestamp = 0;
}

void OneWireDevice::update() {
  _timestamp = millis();
}

boolean OneWireDevice::isError() {
	return _error; 
}

unsigned long OneWireDevice::getTimestamp() {
	return _timestamp; 
}

String OneWireDevice::toString() { 
	return "NoDevice"; 
}

