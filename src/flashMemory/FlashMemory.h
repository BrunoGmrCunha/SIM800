#ifndef FlashMemory_h
#define FlashMemory_h

#include "Arduino.h"
#include "constants.h"
#include "ArduinoJson.h"

void flashMemorySetup(uint8_t &usersCount, uint8_t &messagesCount, Users *users, Messages *messages);
bool saveConfiguration();
bool loadConfiguration2Struct(uint8_t &usersCount, uint8_t &messagesCount,  Users *users, Messages *messages);
bool createJsonConfiguration();
void writeHistory(History &history);
String readHistory();

String getJsonConfig();
void setJsonConfig(DynamicJsonDocument &receivedJsonDocumment);
#endif //FlashMemory_h