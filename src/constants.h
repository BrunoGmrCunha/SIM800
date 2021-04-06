#ifndef constants_h
#define constants_h
#include "Arduino.h"

const uint8_t MAX_USERS = 10;
const uint8_t MAX_MESSAGES = 10;
const uint8_t MAX_HISTORY = 50;

const uint8_t PIN_RELAY_1 = 18;
const uint8_t PIN_RELAY_2 = 19;
const uint8_t PIN_BUTTON = 15;
const uint8_t PIN_LED_GREEN = 13;
const uint8_t PIN_LED_RED = 25;

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

#endif //constants_h