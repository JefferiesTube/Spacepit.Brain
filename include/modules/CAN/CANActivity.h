#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class CANActivity
{
public:
    CANActivity(uint16_t activeDurationMs, 
        std::function<void()> onActivate = [](){},
        std::function<void()> onDeactivate = [](){});
    ~CANActivity();

    void trigger();

private:
    static void taskRunner(void* param);
    std::function<void()> onActivateCallback = [](){};
    std::function<void()> onDeactivateCallback = [](){};

    void run();

    uint16_t activeDurationMs;
    volatile bool activity = false;
    TaskHandle_t taskHandle = nullptr;
};
