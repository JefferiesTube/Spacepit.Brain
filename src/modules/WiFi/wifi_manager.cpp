#include "modules/WiFi/wifi_manager.h"
#include <WiFi.h>
#include <WiFiManager.h>
#include <EEPROM.h>

void SetupWiFi() 
{
  EEPROM.begin(512);

  WiFiManager wm;
  
  if (!wm.autoConnect("SpacePit", "spacespace")) 
  {
    Serial.println(F("Failed to connect and hit timeout"));
    ESP.restart();
    delay(1000);
  }
  
  Serial.println(F("Connected to WiFi!"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
}
