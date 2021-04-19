#ifndef Gsm_h
#define Gsm_h

#include "Arduino.h"

typedef struct
{
    String number;
    String message;

} ReceivedAt;

class Gsm
{

public:
    void begin();
    bool checkSignalStrength();
    String getSignalStrength();
    bool isSms(String receivedAt);
    bool isCall(String receivedAt);
    uint8_t received(String &number, String &message, String &date, String &hour);
    String hexToAscii(String hex);
    String stringSpecialCharFormat(String inputStr);
    void hangUp();
    String send(String &atCommand);
    void getDateTime(int *day, int *month, int *year, int *hour, int *minute, int *second);

private:
    String updateSerial();
    String updateSerial(uint64_t timeout);
    int checkQuality(String receivedResponse);
    bool checkOK(String receivedResponde);
    bool dealSms(String receivedAT, String &number, String &message, String &date, String &hour);
    bool dealCall(String receivedAT, String &number, String &date, String &hour);
    bool delAllSms();
};

#endif //Gsm_h