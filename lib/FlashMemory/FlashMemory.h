#ifndef __FLASHMEMORY_H__
#define __FLASHMEMORY_H__
#include "Arduino.h"
#include "ArduinoJson.h"

typedef struct
{
    String name;
    String number;
} Users;

typedef struct
{
    String message;
    bool relay1;
    bool relay2;
} Messages;

typedef struct
{
    String name;
    String message;
    String date;
    String hour;
    bool relay1;
    bool relay2;
} History;

class FlashMemory
{
private:
    /* data */
public:
    void begin();
    bool loadConfiguration();
    String getConfiguration();
    bool setConfiguration(DynamicJsonDocument &receivedConfiguration);
    bool loadConfiguration2Struct(uint8_t &usersCount, uint8_t &messagesCount, Users *users, Messages *messages);
};

#endif // __FLASHMEMORY_H__