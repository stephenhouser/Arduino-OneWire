/* DS2438.cpp
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

#include "DS2438.h"

#define DS2438_TEMPERATURE_CONVERSION_COMMAND 0x44
#define DS2438_VOLTAGE_CONVERSION_COMMAND 0xb4
#define DS2438_WRITE_SCRATCHPAD_COMMAND 0x4e
#define DS2438_COPY_SCRATCHPAD_COMMAND 0x48
#define DS2438_READ_SCRATCHPAD_COMMAND 0xbe
#define DS2438_RECALL_MEMORY_COMMAND 0xb8
#define DS2438_PAGE_0 0x00

#define DS2438_TEMPERATURE_DELAY 10
#define DS2438_VOLTAGE_CONVERSION_DELAY 8

DS2438::DS2438(OneWire *ow, uint8_t *address) : OneWireTemperatureDevice(ow, address){};

void DS2438::begin(uint8_t mode) {
	OneWireDevice::begin();

  	mode = mode & (DS2438_MODE_CHA | DS2438_MODE_CHB | DS2438_MODE_TEMPERATURE);
  	voltageA = 0.0;
  	voltageB = 0.0;
}

void DS2438::update() {
	OneWireDevice::update();
	
	uint8_t data[9];
	error = true;

	if (mode & DS2438_MODE_CHA || mode == DS2438_MODE_TEMPERATURE) {
		boolean doTemperature = mode & DS2438_MODE_TEMPERATURE;
		if (!startConversion(DS2438_CHA, doTemperature)) {
			return;
		}

		if (!readPageZero(data)) {
			return;
		}

		if (doTemperature) {
			temperature = (double)(((((int16_t)data[2]) << 8) | (data[1] & 0x0ff)) >> 3) * 0.03125;
		}
		if (mode & DS2438_MODE_CHA) {
			voltageA = (((data[4] << 8) & 0x00300) | (data[3] & 0x0ff)) / 100.0;
		}
	}

	if (mode & DS2438_MODE_CHB) {
		boolean doTemperature = mode & DS2438_MODE_TEMPERATURE && !(mode & DS2438_MODE_CHA);
		if (!startConversion(DS2438_CHB, doTemperature)) {
			return;
		}

		if (!readPageZero(data)) {
			return;
		}

		if (doTemperature) {
			temperature = (double)(((((int16_t)data[2]) << 8) | (data[1] & 0x0ff)) >> 3) * 0.03125;
		}

		voltageB = (((data[4] << 8) & 0x00300) | (data[3] & 0x0ff)) / 100.0;
	}

	error = false;
}

float DS2438::getVoltage(int channel) {
	if (channel == DS2438_CHA) {
		return voltageA;
	} else if (channel == DS2438_CHB) {
		return voltageB;
	} else {
		return 0.0;
	}
}

boolean DS2438::startConversion(int channel, boolean doTemperature) {
	if (!selectChannel(channel)) {
		return false;
	}

	ow->reset();
	ow->select(address);
	if (doTemperature) {
		ow->write(DS2438_TEMPERATURE_CONVERSION_COMMAND, 0);
        delay(DS2438_TEMPERATURE_DELAY);
		ow->reset();
		ow->select(address);
	}

	ow->write(DS2438_VOLTAGE_CONVERSION_COMMAND, 0);
    delay(DS2438_VOLTAGE_CONVERSION_DELAY);
    return true;
}

boolean DS2438::selectChannel(int channel) {
	uint8_t data[9];
	if (readPageZero(data)) {
		if (channel == DS2438_CHB)
		data[0] = data[0] | 0x08;
		else
		data[0] = data[0] & 0xf7;
		writePageZero(data);
		return true;
	}
	
	return false;
}

void DS2438::writePageZero(uint8_t *data) {
	ow->reset();
	ow->select(address);
	ow->write(DS2438_WRITE_SCRATCHPAD_COMMAND, 0);
	ow->write(DS2438_PAGE_0, 0);
	for (int i = 0; i < 8; i++) {
		ow->write(data[i], 0);
	}
		
	ow->reset();
	ow->select(address);
	ow->write(DS2438_COPY_SCRATCHPAD_COMMAND, 0);
	ow->write(DS2438_PAGE_0, 0);
}

boolean DS2438::readPageZero(uint8_t *data) {
	ow->reset();
	ow->select(address);
	ow->write(DS2438_RECALL_MEMORY_COMMAND, 0);
	ow->write(DS2438_PAGE_0, 0);
	ow->reset();
	ow->select(address);
	ow->write(DS2438_READ_SCRATCHPAD_COMMAND, 0);
	ow->write(DS2438_PAGE_0, 0);
	for (int i = 0; i < 9; i++) {
		data[i] = ow->read();
	}
	return ow->crc8(data, 8) == data[8];
}
