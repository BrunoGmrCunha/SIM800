#ifndef Site_h
#define Site_h
#include "Arduino.h"
#include "constants.h"
#include "ArduinoJson.h"

void siteSetup();
void siteConfiguration();
void getReceivedJson(DynamicJsonDocument &receivedDoc);
bool receivedData();
void setReceivedData(bool state);

#endif //Site_h