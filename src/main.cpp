#include <Arduino.h>

#include "Communication.h"

void setup() 
{
  Serial.begin(115200);
  SetupCAN();
}

void loop() 
{
  int packetSize = CAN.parsePacket();

  if (packetSize) 
  {
    long id = CAN.packetId();
    switch(id)
    {
      case 0xABCDEF:
        {
          int val = ReadInt32(packetSize);
          Serial.print("Encoder Value: ");
          Serial.println(val);
        }
        break;
      case 0xC0FFEE:
        {
          int val2 = ReadInt32(packetSize);
          Serial.println(val2 == 0 ? "Button released" : "Button pressed");
        }
        break;
    }
  }
}

