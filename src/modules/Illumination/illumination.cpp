#include "modules/Illumination/illumination.h"

CRGB leds[ILLUMINATION_COUNT];

void setupIllumination() 
{
    FastLED.addLeds<NEOPIXEL, STATUS_LED_PIN>(leds, ILLUMINATION_COUNT);
    FastLED.clear();
    FastLED.setBrightness(50);
    for(int i = 0; i < ILLUMINATION_COUNT; i++)
    {
        leds[i] = CRGB::Red; FastLED.show(); delay(50);
        leds[i] = CRGB::Green; FastLED.show(); delay(50);
        leds[i] = CRGB::Blue; FastLED.show(); delay(50);        
        leds[i] = CRGB::Black; FastLED.show(); 
    }    
}