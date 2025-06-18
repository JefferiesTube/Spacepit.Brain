#pragma once

#include <Arduino.h>
#include "config.h"
#include <EEPROM.h>
#include "eepromLayout.h"

class Configuration
{
    private:
        static uint16_t identification;
        static bool initialized;
        static void Init();
    public:
        static uint16_t LoadIdentification(bool force = false);
        static void SaveIdentification(uint16_t id);
};
