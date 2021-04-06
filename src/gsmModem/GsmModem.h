#ifndef GsmModem_h
#define GsmModem_h
#include <Arduino.h>
#include "constants.h"

void sim800Setup();

void gsmInit();

String updateSerial();

void checkQuality();

bool received(String &receivedStr);

void callReject();


#endif //GsmModem