/* OneWireWindSpeed.cpp
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

#include "OneWireWindSpeed.h"

/* Taken from:
 * https://www.element14.com/community/groups/internet-of-things/blog/2015/01/07/old-meets-new-the-1-wire-weather-station-on-the-spark-core-part-5
 *
 * Two magnets mounted on a rotor attached to the wind cups axle that operate a
 * reed switch connected to a DS2423 counter chip. One magnet is mounted in
 * each of the two outermost holes of the rotor. This provides two counts per
 * revolution which improves response at low wind speeds. It also provides
 * rotational balance to the rotor, which becomes important with increasing
 * wind speed, as the rotor can reach 2400 rpm in 160 km/h winds.
 * The DS2423 keeps track of the total number of revolutions the wind cups make
 * and provides the data to the bus master on demand. The chip contains two
 * 232 bit counters and can be powered for ten years with a small Lithium
 * battery, however, here power for the counter chip comes from CR1 and C1
 * (Figure 1 again) which form a half wave rectifier that “steals” power from
 * the data line. The DS2423 can only be reset to zero when this
 * “parasite power” is lost. Wind speed is calculated by the bus master taking
 * the difference between two counts of the counter used. One count generated
 * before and the other after a clocked interval. The output is currently given
 * in rpm. This later needs to be converted to m/s or km/h.
 *
 * Calculations based on OneWireWeather Station: http://oww.sourceforge.net
 */

OneWireWindSpeed::OneWireWindSpeed(OneWire *ow, uint8_t *address)
    : DS2423(ow, address){};

void OneWireWindSpeed::begin() {
  	DS2423::begin(DS2423_COUNTER_A);
  	windSpeedRPS = 0.0;
}

void OneWireWindSpeed::update() {
	// wait at least 1 second between reads to get semi-accurate
	// measurements.
	if ((millis() - lastReadTimestamp) >= 1000) {
		DS2423::update();

		unsigned long currentTimestamp = getTimestamp();
		uint32_t currentCount = getCount(DS2423_COUNTER_A);

		float count = (currentCount - lastReadCount) * 1000.0;
		float milliSeconds = (currentTimestamp - lastReadTimestamp);
		windSpeedRPS = (count / milliSeconds) / 2; /* two magnets */

		lastReadCount = currentCount;
		lastReadTimestamp = currentTimestamp;
	}
}

float OneWireWindSpeed::getWindSpeedMPH() {
	if ((windSpeedRPS >= 0) && (windSpeedRPS * 2.453 < 200.0)) {
		return (float)windSpeedRPS * 2.453F;
	}

	return 0.0;
}

float OneWireWindSpeed::getWindSpeedKMPH() {
  	return getWindSpeedMPH() * 0.447040972F;
}

String OneWireWindSpeed::toString() {
  return String(getWindSpeedKMPH(), 2) + " km/h";
}