#include "modules/CAN/CANActivity.h"

CANActivity::CANActivity(uint16_t activeDurationMs,     
    std::function<void()> onActivate,
    std::function<void()> onDeactivate) 
    : activeDurationMs(activeDurationMs),
    onActivateCallback(std::move(onActivate)),
    onDeactivateCallback(std::move(onDeactivate))
{
    if(xTaskCreate(taskRunner, "CANActivityTask", 8192, this, 1, &taskHandle) != pdPASS)
    {
        Serial.println("Failed to create CANActivity task");
        return;
    }
    else
    {
        Serial.println("CANActivity task created successfully.");
    }
}

CANActivity::~CANActivity() {
    if (taskHandle) {
        vTaskDelete(taskHandle);
        taskHandle = nullptr;
    }
}

void CANActivity::trigger()
{
    activity = true;
}

void CANActivity::taskRunner(void* param)
{
    static_cast<CANActivity*>(param)->run();
}

void CANActivity::run()
{
    const TickType_t blinkTime = pdMS_TO_TICKS(activeDurationMs);

    for (;;)
    {
        if (activity)
        {
            activity = false;
            if(onActivateCallback)
                onActivateCallback();
            vTaskDelay(blinkTime);
            if(onDeactivateCallback)
                onDeactivateCallback();
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
