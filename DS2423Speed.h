/* DS2423Speed.h
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

#ifndef DS2423Speed_h
#define DS2423Speed_h

#include <Arduino.h>
#include "DS2423.h"

class DS2423Speed : public DS2423 {
    public:
        DS2423Speed(OneWire *ow, uint8_t *address);

        void begin();
        void update();
        void reset();
        String toString();
        String toJSON();

		float getSpeed();           // Instantaneous speed
        float getAverageSpeed();    // average speed since last reset();
        float getMaximumSpeed();    // maximum speed since last reset();

    private:
        float revolutionsPerSecond;
        unsigned long lastTimestamp = 0;
        uint32_t lastCount = 0;

        float maximumRPS = 0;
        float averageRPSSum = 0;
        uint32_t averageRPSCount = 0;
};

#endif
