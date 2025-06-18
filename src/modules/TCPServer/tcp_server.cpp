#include "modules/TCPServer/tcp_server.h"

#include <unordered_map>
#include <functional>
#include <modules/TCPServer/TCPPackages.h>
#include <modules/Configuration/configuration.h>
#include "config.h"

AsyncServer *server;
std::vector<AsyncClient*> clients;

std::vector<SPPackage> messageQueue;
std::unordered_map<long, TCPServer::tcpHandler> TCPServer::handlerMap;

portMUX_TYPE queueMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE clientMux = portMUX_INITIALIZER_UNLOCKED;

const uint16_t localTcpPort = 12345;

TCPServer::TCPServer()
{
}

void TCPServer::begin() 
{
  server = new AsyncServer(localTcpPort);
  server->onClient([this](void *arg, AsyncClient *client) 
  {
    this->onClientConnect(arg, client);
  }, server);
  server->begin();
  Serial.printf("Async TCP Server started at port %u\n", localTcpPort);
}

void TCPServer::update() 
{
  std::vector<SPPackage> localQueue;

  portENTER_CRITICAL(&queueMux);
  localQueue.swap(messageQueue);
  portEXIT_CRITICAL(&queueMux);

  for (const auto& msg : localQueue) 
  {
    BroadcastToClients(msg);
  }
}

void TCPServer::BroadcastToClients(const SPPackage& message) 
{
  for (auto* client : clients) 
  {
    if (client && client->connected()) 
    { 
      client->write(message.data(), message.size());
    }
  }
}

void TCPServer::onClientConnect(void *arg, AsyncClient *client) 
{
  Serial.println("New TCP client connected");

  portENTER_CRITICAL(&clientMux);
  clients.push_back(client);
  portEXIT_CRITICAL(&clientMux);

  client->onDisconnect([](void *arg, AsyncClient *client) 
  {
    Serial.println("TCP client disconnected");
    portENTER_CRITICAL(&clientMux);
    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
    portEXIT_CRITICAL(&clientMux);
    delete client;
  }, NULL);

  client->onData([](void *arg, AsyncClient *client, void *data, size_t len) 
  {
    if (len >= SPPACKAGE_FULL_LENGTH) 
    {
      SPPackage pkg;
      memcpy(&pkg, data, SPPACKAGE_FULL_LENGTH);
      Serial.print("Received TCP SPPackage with id: ");
      Serial.println(pkg.payload.id);
      
      auto it = TCPServer::handlerMap.find(pkg.payload.id);
      if (it != TCPServer::handlerMap.end()) 
      {
        auto& handler = it->second;
        if (handler) 
        handler(pkg);
      }
  } 
  else 
  {
    Serial.println("Received TCP data too short for SPPackage");
  }
  }, NULL);
  
  // TCPServer::registerHandler(ID_MARCO, [this, client](const SPPackage& pkg) {
  //   SPPackage response = pkg;
  //   response.payload.id = ID_POLO;
  //   this->Send(response);
  //   Serial.println("Responded to ID_MARCO with ID_POLO");
  // });

  TCPServer::registerHandler(ID_MARCO, [this](const SPPackage& pkg){ this->HandleIdentifyRequest(pkg); });
  TCPServer::registerHandler(ID_GETFIRMWARE, [this](const SPPackage& pkg){ this->HandleFirmwareRequest(pkg); });
  
}

void TCPServer::Send(SPPackage& data)
{
  portENTER_CRITICAL(&queueMux);
  messageQueue.push_back(data);
  portEXIT_CRITICAL(&queueMux);  
}

void TCPServer::registerHandler(long id, std::function<void(const SPPackage&)> handler)
{
    handlerMap[id] = handler;
}

void TCPServer::registerHandlers(const std::unordered_map<long, std::function<void(const SPPackage&)>>& handlers)
{
    for (const auto& [id, handler] : handlers)
    {
        handlerMap[id] = handler;
    }
}

#pragma region "TCP Handlers"

void TCPServer::HandleIdentifyRequest(const SPPackage &pkg)
{
  SPPackage response = pkg;
  response.payload.id = ID_POLO;
  this->Send(response);
  Serial.println(F("Responded to ID_MARCO with ID_POLO"));
}

void TCPServer::HandleFirmwareRequest(const SPPackage &pkg)
{
  Serial.println(F("Firmware request received"));
  // TODO: Check if self. If so, respond with firmware version.
  // Otherweise, forward to CAN bus.
  
  uint16_t identification = Configuration::LoadIdentification();
  Serial.println(F("Expected Identification: "));
  Serial.println(pkg.payload.target);
  Serial.println(F("Identification: "));
  Serial.println(identification);
  if(pkg.payload.target == identification)
  {
    SPPackage response = pkg;
    response.payload.target = pkg.payload.target;
    response.payload.id = ID_GETFIRMWARE;
    response.payload.data.intData.iA = FIRMWARE_VERSION;  
    this->Send(response);
  }
  else
  {
    Serial.println(F("TODO: Forwarding firmware request to CAN bus"));
  }
}

#pragma endregion
