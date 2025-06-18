#pragma once

#pragma region "Globals and Defaults"

#define FIRMWARE_VERSION 0x0000C001
#define TARGETID 0xFFFF

/* The Type identifies the type of device */
#define DEFAULTTYPE_ID 0x0000FFFF
#define TYPEID DEFAULTTYPE_ID

#define CAN_TX 4
#define CAN_RX 5
#define CAN_ACTIVITY_DURATION 250
#define STATUS_LED_PIN 16
#define ILLUMINATION_COUNT 1

#pragma endregion

#pragma region "Observer"
#ifdef INSTRUMENT_OBSERVER
    #include "instruments/Observer/config.h"
#endif

#pragma endregion
