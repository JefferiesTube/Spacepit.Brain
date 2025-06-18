#pragma once

#include <Arduino.h>

#define SPPACKAGE_MAX_LENGTH 16
#define SPPACKAGE_FULL_LENGTH (SPPACKAGE_MAX_LENGTH + sizeof(uint64_t) + sizeof(uint16_t))

struct SPPackage
{
    union
    {
        uint8_t bytes[SPPACKAGE_FULL_LENGTH];
        struct
        {
            uint64_t id;
            uint16_t target;
            union
            {
                uint8_t bytes[SPPACKAGE_MAX_LENGTH];
                struct
                {
                    uint64_t lA;
                    uint64_t lB;
                } longData;
                struct
                {
                    uint32_t iA;
                    uint32_t iB;
                } intData;
            } data;
        } payload;
    };

    const char* data() const { return reinterpret_cast<const char*>(bytes); }
    size_t size() const { return sizeof(bytes); }
};