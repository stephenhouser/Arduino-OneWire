/* DS2450.cpp
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
#include "DS2450.h"

#define DS2450_WRITE_COMMAND 0x55
#define DS2450_BEGIN_VOLTAGE_CONVERSION 0x3c
#define DS2450_REGISTER_4CHANNEL 0x0f
#define DS2450_READOUT_CONTROL 0xaa
#define DS2450_MEMORY_READ_COMMAND 0xaa
#define DS2450_8_BIT_RESOLUTION 0x08
#define DS2450_POR_OFF_NO_ALARMS_5V_RANGE 0x01
#define DS2450_VCC_CONTROL_BYTE_ADDR_LO 0x1c
#define DS2450_VCC_CONTROL_BYTE_ADDR_HI 0x00
#define DS2450_VCC_POWERED 0x40
#define DS2450_CONTROL_STATUS_DATA_ADDR_LO 0x08
#define DS2450_CONTROL_STATUS_DATA_ADDR_HI 0x00
#define DS2450_CONVERSION_COMPLETE 0xff
#define DS2450_AD_CHANNELS_ADDR_LO 0x00
#define DS2450_AD_CHANNELS_ADDR_HI 0x00
#define DS2450_TEMP_CONVERSION_TIMEOUT 50

DS2450::DS2450(OneWire *ow, uint8_t *address) : OneWireDevice(ow, address){
};

void DS2450::begin() {
	OneWireDevice::begin();

	ow->reset();
	ow->select(address);
	ow->write(DS2450_WRITE_COMMAND, 0);
	ow->write(DS2450_VCC_CONTROL_BYTE_ADDR_LO, 0);
	ow->write(DS2450_VCC_CONTROL_BYTE_ADDR_HI, 0);
	ow->write(DS2450_VCC_POWERED, 0);
	ow->read(); // crc lsb
	ow->read(); // crc msb
	ow->read(); // verify

	ow->reset();
	ow->select(address);
	ow->write(DS2450_WRITE_COMMAND, 0);
	ow->write(DS2450_CONTROL_STATUS_DATA_ADDR_LO, 0);
	ow->write(DS2450_CONTROL_STATUS_DATA_ADDR_HI, 0);

	for (int i = 0; i < 4; i++) {
		ow->write(DS2450_8_BIT_RESOLUTION, 0);
		ow->read(); // crc
		ow->read();
		ow->read(); // verify data

		ow->write(DS2450_POR_OFF_NO_ALARMS_5V_RANGE, 0);
		ow->read(); // crc
		ow->read();
		ow->read(); // verify data
		voltage[i] = 0.0;
	}
}

void DS2450::update() {
	OneWireDevice::update();

	uint8_t data[13];

	error = true;

	ow->reset();
	ow->select(address);
	data[0] = DS2450_BEGIN_VOLTAGE_CONVERSION;
	data[1] = DS2450_REGISTER_4CHANNEL;
	data[2] = DS2450_READOUT_CONTROL;
	ow->write(data[0], 0);
	ow->write(data[1], 0);
	ow->write(data[2], 0);
	data[3] = ow->read(); // crc
	data[4] = ow->read();

	while (1) {
		if ((long)millis() - (long)timestamp <= DS2450_TEMP_CONVERSION_TIMEOUT) {
			if (ow->read() == DS2450_CONVERSION_COMPLETE) {
				break;
			}
		} else {
			// conversion taking too long
			Serial.println("ERROR: DS2450 Conversion took too long.");
			return;
		}
	}

	vTaskDelay(1 / portTICK_PERIOD_MS);

	ow->reset();
	ow->select(address);
	data[0] = DS2450_MEMORY_READ_COMMAND;
	data[1] = DS2450_AD_CHANNELS_ADDR_LO;
	data[2] = DS2450_AD_CHANNELS_ADDR_HI;
	ow->write(data[0]);
	ow->write(data[1]);
	ow->write(data[2]);
	for (int i = 3; i < 13; i++) {
		data[i] = ow->read();
	}

	voltage[0] = (float)data[4] / 50.0;
	voltage[1] = (float)data[6] / 50.0;
	voltage[2] = (float)data[8] / 50.0;
	voltage[3] = (float)data[10] / 50.0;

	uint16_t crc = ow->crc16(data, 11);
	uint8_t *crcBytes = (uint8_t *)&crc;
	uint8_t crcLo = ~data[11];
	uint8_t crcHi = ~data[12];
	error = !(crcBytes[0] == crcLo && crcBytes[1] == crcHi);
}

float DS2450::getVoltage(int channel) {
	if (channel >= 0 && channel < 4) {
		return voltage[channel];
	} else {
		return 0.0;
	}
}

String DS2450::toString() {
  return "(" + String(voltage[0], 1) 
  		+ ", " + String(voltage[1], 1) 
		+ ", " + String(voltage[2], 1) 
		+ ", " + String(voltage[3], 1) + ")";
}
