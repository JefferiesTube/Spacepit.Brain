#include <Arduino.h>
#include <FastLED.h>
#include "config.h"
#include <EEPROM.h>

#ifdef INSTRUMENT_OBSERVER
#include "instruments/Observer/construct.h"
#endif

#include "modules/Illumination/illumination.h"
#include "modules/Configuration/configuration.h"

void setup()
{
    EEPROM.begin(512);
    Configuration::LoadIdentification();
    setupIllumination();
    setupInstrument();
}

void loop()
{
    loopInstrument();
}

