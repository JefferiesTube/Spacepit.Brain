#pragma once

#include <Arduino.h>

#define CAN_MAX_DATA_LENGTH 8

struct CANMessage
{
    long id;
    uint8_t length;

    union
    {
        uint8_t bytes[CAN_MAX_DATA_LENGTH];
        uint64_t ulongValue;
        uint32_t uintValue;
        float floatValue;
        struct
        {
            uint32_t a;
            uint32_t b;
        } packedIntValue;
    } data;
};