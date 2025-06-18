#include "modules/CAN/CANHandler.h"
#include "modules/CAN/PackageIDs.h"
#include "modules/CAN/CANActivity.h"
#include "config.h"

CANActivity* CANSendActivity = nullptr;
CANActivity* CANReceiveActivity = nullptr;

QueueHandle_t CANHandler::canQueue = nullptr;
std::unordered_map<long, std::function<void(const CANMessage&)>> CANHandler::handlerMap;

CANHandler::CANHandler()
{
}

void CANHandler::SetupCAN(std::function<void()> onTXActivate, std::function<void()> onTXDeactivate,
            std::function<void()> onRXActivate, std::function<void()> onRXDeactivate)
{
    if (CANSendActivity) delete CANSendActivity;
    if (CANReceiveActivity) delete CANReceiveActivity;

    CANSendActivity = new CANActivity(CAN_ACTIVITY_DURATION, onTXActivate, onTXDeactivate);
    CANReceiveActivity = new CANActivity(CAN_ACTIVITY_DURATION, onRXActivate, onRXDeactivate);

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
            // TODO: Show status LED
            delay(500);
            digitalWrite(2, HIGH);
            delay(500);
            digitalWrite(2, LOW);
            Serial.println("CAN initialization error.");
        }
    }

    Serial.println("CAN initialized");
}

void CANHandler::begin(std::function<void()> onTXActivate, std::function<void()> onTXDeactivate,
std::function<void()> onRXActivate, std::function<void()> onRXDeactivate)
{
    SetupCAN(onTXActivate, onTXDeactivate, onRXActivate, onRXDeactivate);

    canQueue = xQueueCreate(10, sizeof(CANMessage));
    if (!canQueue)
    {
        Serial.println("CAN queue creation failed!");
        return;
    }

    Serial.println("CAN queue created successfully.");
    xTaskCreate(processingTask, "CANProcessingTask", 8192, nullptr, 1, nullptr);
    CAN.onReceive(onReceiveISR);
}

void CANHandler::send(const CANMessage& msg)
{
    CANSendActivity->trigger();
    if (msg.length > CAN_MAX_DATA_LENGTH)
    {
        Serial.println("CAN send error: message length exceeds maximum allowed length.");
        return;
    }

    CAN.beginExtendedPacket(msg.id);
    for (uint8_t i = 0; i < msg.length; i++)
    {
        CAN.write(msg.data.bytes[i]);
    }
    CAN.endPacket();
}

void CANHandler::registerHandler(long id, std::function<void(const CANMessage&)> handler)
{
    handlerMap[id] = handler;
}

void CANHandler::registerHandlers(const std::unordered_map<long, std::function<void(const CANMessage&)>>& handlers)
{
    for (const auto& [id, handler] : handlers)
    {
        handlerMap[id] = handler;
    }
}

void IRAM_ATTR CANHandler::onReceiveISR(int packetSize)
{
    CANMessage msg;
    msg.id = CAN.packetId();
    msg.length = 0;
    CANReceiveActivity->trigger();

    while (CAN.available() && msg.length < CAN_MAX_DATA_LENGTH)
    {
        msg.data.bytes[msg.length++] = CAN.read();
    }

    BaseType_t highTaskWoken = pdFALSE;
    xQueueSendFromISR(canQueue, &msg, &highTaskWoken);
    if (highTaskWoken)
    {
        portYIELD_FROM_ISR();
    }
}

void CANHandler::processingTask(void* pvParameters)
{
    CANMessage msg;

    for (;;)
    {
        if (xQueueReceive(canQueue, &msg, portTICK_PERIOD_MS * 5))
        {
            auto it = handlerMap.find(msg.id);
            if (it != handlerMap.end())
            {
                it->second(msg);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}