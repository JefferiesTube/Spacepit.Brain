#pragma once

#include <Arduino.h>
#include <AsyncTCP.h>
#include <fmt.h>
#include <vector>
#include <string>
#include <WiFi.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "modules/TCPServer/SPPackage.h"

class TCPServer
{
    public:
        TCPServer();
        void begin();
        void update();

        void BroadcastToClients(const SPPackage& message);

        void registerHandler(long id, std::function<void(const SPPackage&)> handler);
        void registerHandlers(const std::unordered_map<long, std::function<void(const SPPackage&)>>& handlers);

        void Send(SPPackage& data);
    private:
        void onClientConnect(void *arg, AsyncClient *client);


        using tcpHandler = std::function<void(const SPPackage&)>;
        static QueueHandle_t canQueue;
        static std::unordered_map<long, tcpHandler> handlerMap;

        void HandleIdentifyRequest(const SPPackage &pkg);
        void HandleFirmwareRequest(const SPPackage &pkg);
};