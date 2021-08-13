#ifndef __WEBPAGE_H__
#define __WEBPAGE_H__

#include "Arduino.h"
#include "ArduinoJson.h"
#include <ESPAsyncWebServer.h>
#include <FlashMemory.h>
#include <Gsm.h>

class WebPage
{
private:
    Gsm *_gsm;
    FlashMemory *_flashMemory;

public:
    WebPage(Gsm *gsm, FlashMemory *flashMemory);
    void begin();
    bool handleUpdate(AsyncWebServerRequest *request, uint8_t *data);
    bool handleConfiguration(AsyncWebServerRequest *request, uint8_t *data);

    bool configuration();
};

#endif // __WEBPAGE_H__