/* DS2423.cpp
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
#include "DS2423.h"

#define DS2423_READ_MEMORY_COMMAND 0xa5
#define DS2432_SKIP_ROM_COMMAND 0xcc
#define DS2423_PAGE_ONE 0xc0
#define DS2423_PAGE_TWO 0xe0

DS2423::DS2423(OneWire *ow, uint8_t *address) : OneWireDevice(ow, address) {
};

void DS2423::begin() {
	OneWireDevice::begin();
	errorA = true;
	errorB = true;
	countA = 0;
	countB = 0;
}

void DS2423::update() {
	OneWireDevice::update();
	readCounter(DS2423_COUNTER_A);
	readCounter(DS2423_COUNTER_B);
}

uint32_t DS2423::getCount(uint8_t counter) {
  	switch (counter) {
		case DS2423_COUNTER_A:
			return countA;
		case DS2423_COUNTER_B:
			return countB;
		default:
			return 0;
	}
}

void DS2423::readCounter(uint8_t counter) {
	uint8_t data[45];

	data[0] = DS2423_READ_MEMORY_COMMAND;
	data[1] = (counter == DS2423_COUNTER_A ? DS2423_PAGE_ONE : DS2423_PAGE_TWO);
	data[2] = 0x01;

	ow->reset();
	ow->select(address);
	ow->write(data[0], 0);
	ow->write(data[1], 0);
	ow->write(data[2], 0);
	for (int j = 3; j < 45; j++) {
		data[j] = ow->read();
	}
	ow->reset();

	uint32_t count = (uint32_t)data[38];
	for (int j = 37; j >= 35; j--) {
		count = (count << 8) + (uint32_t)data[j];
	}
	uint16_t crc = ow->crc16(data, 43);
	uint8_t *crcBytes = (uint8_t *)&crc;
	uint8_t crcLo = ~data[43];
	uint8_t crcHi = ~data[44];
	error = (crcLo != crcBytes[0]) || (crcHi != crcBytes[1]);
	if (counter == DS2423_COUNTER_A) {
		countA = count;
		errorA = error;
	} else if (counter == DS2423_COUNTER_B) {
		countB = count;
		errorB = error;
	}
}

String DS2423::toString() {
  return "(" + String(countA) + ", " + String(countB) + ")";
}