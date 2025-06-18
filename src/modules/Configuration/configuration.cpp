#include "modules/Configuration/configuration.h"

uint16_t Configuration::identification = 0;
bool Configuration::initialized = false;

uint16_t Configuration::LoadIdentification(bool force)
{
    if (force || identification == 0) 
    {
        identification = EEPROM.readUShort(OFFSET_IDENTIFICATION);
        if (identification == 0 || identification == 0xFFFF) 
        {
            identification = TARGETID;
            EEPROM.put(OFFSET_IDENTIFICATION, identification);
            EEPROM.commit();
        }
        identification = TARGETID;
        Serial.print(F("Loaded identification: "));
        Serial.println(identification, HEX);
    }
    
    return identification;
}

void Configuration::SaveIdentification(uint16_t id)
{
    identification = id;
    EEPROM.put(OFFSET_IDENTIFICATION, identification);
    EEPROM.commit();
}
