/* DS2438.h
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

#ifndef DS2438_h
#define DS2438_h

#include <Arduino.h>
#include <OneWireTemperatureDevice.h>

#define DS2438_CHA 0
#define DS2438_CHB 1

#define DS2438_MODE_CHA 0x01
#define DS2438_MODE_CHB 0x02
#define DS2438_MODE_TEMPERATURE 0x04

class DS2438 : public OneWireTemperatureDevice {
    public:
        DS2438(OneWire *ow, uint8_t *address);

        void begin(uint8_t mode=(DS2438_MODE_CHA | DS2438_MODE_CHB | DS2438_MODE_TEMPERATURE));
        void update();
        boolean isError();

        double getTemperature();
		
        float getVoltage(int channel=DS2438_CHA);

    private:
        boolean startConversion(int channel, boolean doTemperature);
        boolean selectChannel(int channel);
        void writePageZero(uint8_t *data);
        boolean readPageZero(uint8_t *data);

        uint8_t mode;
        double temperature;
        float voltageA;
        float voltageB;
};

#endif
