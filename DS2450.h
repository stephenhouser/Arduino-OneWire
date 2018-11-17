/* DS2450.h
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

#ifndef DS2450_h
#define DS2450_h

#include <Arduino.h>
#include <OneWireDevice.h>

class DS2450 : public OneWireDevice {
    public:
        DS2450(OneWire *ow, uint8_t *address);
        void begin();
        void update();
		String toString();

        float getVoltage(int channel);

    private:
        float _voltage[4];
};

#endif
