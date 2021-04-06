#ifndef Gsm_h
#define Gsm_h

#include "Arduino.h"

class Gsm
{

public:
    void begin();


    private:
    String updateSerial();
    int checkQuality(String receivedResponse);
    bool checkOK(String receivedResponde);

};

#endif //Gsm_h