#pragma once

#include "config.h"
#include <CAN.h>

void SetupCAN()
{
    CAN.setPins (CAN_RX, CAN_TX);
    int maxAttempts = 3;
    bool canInitialized = false;

    for (int attempt = 1; attempt <= maxAttempts; attempt++) 
    {
        Serial.print("Attempting to start CAN (" + String(attempt) + "/" + String(maxAttempts) + ")...");
        if (CAN.begin(500E3)) 
        {
            canInitialized = true;
            Serial.println("Success!");
            break;
        }
        else 
        {
            Serial.println("Failed.");
            // Optionally delay before retrying
            delay(1000);
        }
    }

    if (!canInitialized) 
    {
        // If we still cannot initialize, we can loop forever or handle it differently.
        Serial.println("Starting CAN failed after all attempts. Check wiring/transceiver.");
        while (1) 
        {
            delay(500);
            digitalWrite(2, HIGH);
            delay(500);
            digitalWrite(2, LOW);
            Serial.println("CAN initialization error.");
        }
    }

    Serial.println("CAN initialized");
}

void SendInt32(long id, uint32_t value)
{
    CAN.beginExtendedPacket(id);
    CAN.write((value >> 24) & 0xFF);
    CAN.write((value >> 16) & 0xFF);
    CAN.write((value >>  8) & 0xFF);
    CAN.write((value      ) & 0xFF);        
    CAN.endPacket();
}

uint32_t ReadInt32(int packageSize)
{
    if(packageSize != 4)
    {
        Serial.print("Unexpected package size (ReadInt32 expected 4, received ");
        Serial.print(packageSize);
        Serial.println(")");
        return 0;
    }
    union 
    {
        int32_t i;
        uint8_t b[4];
    } receiveUnion;

    receiveUnion.b[3] = CAN.read();
    receiveUnion.b[2] = CAN.read();
    receiveUnion.b[1] = CAN.read();
    receiveUnion.b[0] = CAN.read();    
    return receiveUnion.i;
}