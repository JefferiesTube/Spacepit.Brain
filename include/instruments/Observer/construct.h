#include "config.h"
#include "modules/WiFi/wifi_manager.h"
#include "modules/TCPServer/tcp_server.h"
#include "modules/CAN/CANHandler.h"
#include "modules/CAN/PackageIDs.h"
#include "modules/Illumination/illumination.h"

CANHandler canHandler;
TCPServer tcpServer;

volatile bool rx = false;
volatile bool tx = false;

void setupInstrument() 
{
  Serial.begin(115200);
  delay(1000);
  SetupWiFi();
  tcpServer.begin();
  canHandler.begin(
    [](){ tx = true; },
    [](){ tx = false; },
    [](){ rx = true; },
    [](){ rx = false; });

/*  canHandler.registerHandlers({
    { ID_TIME_START, RelayInt },
    { ID_TIME_STOP, RelayInt },
    { ID_TIME_SWITCHINPUT, RelayInt },
    { ID_TIME_RESET, RelayInt },
    { ID_TIME_TIME, RelayPackedInt },
    { ID_TIME_PLAYTIME, RelayPackedInt }
  });  */
}

void loopInstrument()
{
  tcpServer.update();
  delay(1);
}