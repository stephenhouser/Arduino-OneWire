/* OneWireTemperatireDevice.cpp
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

#include "OneWireTemperatureDevice.h"

OneWireTemperatureDevice::OneWireTemperatureDevice(OneWire *ow, uint8_t *address)
    : OneWireDevice(ow, address) {
}

void OneWireTemperatureDevice::begin() {
  OneWireDevice::begin();
  temperature = 0.0;
}

double OneWireTemperatureDevice::getTemperature() { 
	return temperature; 
}

String OneWireTemperatureDevice::toString() {
  float celsius = this->getTemperature();
  float fahrenheit = celsius * 1.8 + 32.0;
  return String(fahrenheit, 2) + "°F " + String(celsius, 2) + "°C";
}

String OneWireTemperatureDevice::toJSON() {
    float celsius = this->getTemperature();
    return "{\"address\":\"" + getAddressString() + "\"," +
            "\"timestamp\":" + timestamp + "," +
            "\"temperature\":" + celsius + "," +
            "\"units\":\"°C\"" + "}";
}
