/* OneWireDirection.cpp
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

#include "OneWireDirection.h"

// define North offset here
const int NORTH_OFFSET = 0;

const float directionTable[16][4] = {
    {4.5, 4.5, 2.5, 4.5}, // N
    {4.5, 2.5, 2.5, 4.5}, // NNE
    {4.5, 2.5, 4.5, 4.5}, // NE
    {2.5, 2.5, 4.5, 4.5}, // ENE
    {2.5, 4.5, 4.5, 4.5}, // E
    {2.5, 4.5, 4.5, 0.0}, // ESE
    {4.5, 4.5, 4.5, 0.0}, // SE
    {4.5, 4.5, 0.0, 0.0}, // SSE
    {4.5, 4.5, 0.0, 4.5}, // S
    {4.5, 0.0, 0.0, 4.5}, // SSW
    {4.5, 0.0, 4.5, 4.5}, // SW
    {0.0, 0.0, 4.5, 4.5}, // WSW
    {0.0, 4.5, 4.5, 4.5}, // W
    {0.0, 4.5, 4.5, 2.5}, // WNW
    {4.5, 4.5, 4.5, 2.5}, // NW
    {4.5, 4.5, 2.5, 2.5}  // NNW
};

const char directions[17][4] = {"N",  "NNE", "NE", "ENE", "E",  "ESE",
                                "SE", "SSE", "S",  "SSW", "SW", "WSW",
                                "W",  "WNW", "NW", "NNW", "Err"};

OneWireDirection::OneWireDirection(OneWire *ow, uint8_t *address) : DS2450(ow, address){};

void OneWireDirection::begin() {
  DS2450::begin();
  direction = 0;
}

void OneWireDirection::update() {
    DS2450::update();

    if (!isError()) {
        direction = 16;

        float ch_a = getVoltage(0);
        float ch_b = getVoltage(1);
        float ch_c = getVoltage(2);
        float ch_d = getVoltage(3);

        for (int i = 0; i < 16; i++) {
            if (((ch_a < directionTable[i][0] + 1.0) &&
              (ch_a >= directionTable[i][0] - 1.0)) &&
              ((ch_b < directionTable[i][1] + 1.0) &&
              (ch_b >= directionTable[i][1] - 1.0)) &&
              ((ch_c < directionTable[i][2] + 1.0) &&
              (ch_c >= directionTable[i][2] - 1.0)) &&
              ((ch_d < directionTable[i][3] + 1.0) &&
              (ch_d >= directionTable[i][3] - 1.0))) {

              direction = (i + NORTH_OFFSET) % 16;
              break;
            }
        }
    } else {
      Serial.println("Error reading from DS2450 device");
	}
}

float OneWireDirection::getDirection() { 
	return direction; 
}

String OneWireDirection::getBearing() { 
	return directions[direction]; 
}

String OneWireDirection::toString() {
  return getBearing();
}