#pragma once

#include <Arduino.h>
#include <functional>
#include <unordered_map>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "CANMessage.h"
#include <CAN.h>

class CANHandler
{
public:
    CANHandler();

    void begin(std::function<void()> onTXActivate, std::function<void()> onTXDeactivate,
               std::function<void()> onRXActivate, std::function<void()> onRXDeactivate);
    void registerHandler(long id, std::function<void(const CANMessage&)> handler);
    void registerHandlers(const std::unordered_map<long, std::function<void(const CANMessage&)>>& handlers);
    void send(const CANMessage& msg);
private:
    static void IRAM_ATTR onReceiveISR(int packetSize);
    static void processingTask(void* pvParameters);

    void SetupCAN(std::function<void()> onTXActivate, std::function<void()> onTXDeactivate,
        std::function<void()> onRXActivate, std::function<void()> onRXDeactivate);

    using InternalHandler = std::function<void(const CANMessage&)>;
    static QueueHandle_t canQueue;
    static std::unordered_map<long, InternalHandler> handlerMap;
};
