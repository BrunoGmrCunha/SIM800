#include <WebPage.h>
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <AsyncJson.h>
#include <SPIFFS.h>

AsyncWebServer _server(80);
DNSServer _dnsServer;

bool _configurationCompleted;

const char *_ssid = "PORTAO";
const char *_password = "123456789";
const uint8_t PIN_BUTTON = 2;
const uint8_t PIN_LED_RED = 13;
class CaptiveRequestHandler : public AsyncWebHandler
{
public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request)
    {
        return true;
    }

    void handleRequest(AsyncWebServerRequest *request)
    {
        // AsyncWebServerResponse *response = request->beginResponse_P(200, F("text/html"), index_html_gz, index_html_gz_len);
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, F("/index.html"), F("text/html"));
        /*         response->addHeader(F("Content-Encoding"), F("gzip"));
 */
        response->addHeader(F("Cache-Control"), F("max-age=600"));
        request->send(response);
    }
};

WebPage::WebPage(Gsm *gsm, FlashMemory *flashMemory)
{
    _gsm = gsm;
    _flashMemory = flashMemory;
}

void WebPage::begin()
{
    _server.serveStatic("/", SPIFFS, "/");

    /* _server.on("/info", HTTP_GET, [&](AsyncWebServerRequest *request) {
       // request->send(200, "application/json", _flashMemory->getConfiguration());
        //json=String();
    }); */

    _server.on("/signalStrength", HTTP_GET, [&](AsyncWebServerRequest *request) {
        request->send(200, "application/json", _gsm->getSignalStrength());
        //json=String();
    });
    _server.on("/logs.txt", HTTP_GET, [&](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/logs.txt", "text/plain");
        //json=String();
    });

    /*    _server.on("/history", HTTP_GET, [](AsyncWebServerRequest *request) {
        //String jsonHistory = readHistory();
        //request->send(200, "application/json", jsonHistory);
        //json=String();
    }); */

    _server.on(
        "/update", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        if(!handleUpdate(request, data))
        {
            request->send(400, "text/plain", "Error update info ");
        } 
        _configurationCompleted = true ; 
        request->send(200); });

    // _server.addHandler(handler);

    _server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP
}

bool WebPage::handleUpdate(AsyncWebServerRequest *request, uint8_t *data)
{
    DynamicJsonDocument receivedConfigDoc(12288);

    // StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(receivedConfigDoc, (const char *)data);
    if (error)
    {
#ifdef DEBUG
        ESP_LOGE(TAG, "deserializeJson() failed: %s", error.c_str());
#endif //DEBUG
        return false;
    }
    else
    {
        _flashMemory->setConfiguration(receivedConfigDoc);
    }
    return true;

    /*   if (connectionPage(doc["ssid"].as<String>(), doc["password"].as<String>()))
    {
        doc.clear();
        doc["ssid"] = WiFi.SSID();
        doc["quality"] = String(getRSSIasQuality(WiFi.RSSI()));
        doc["ipAddress"] = WiFi.localIP().toString();
        doc["subnetMask"] = WiFi.subnetMask().toString();
        doc["routerAddress"] = WiFi.gatewayIP().toString();
        String json;
        serializeJson(doc, json);

        request->send(200, F("application/json"), json);
        return true;
    } */
}

bool WebPage::configuration()
{
#ifdef DEBUG
    ESP_LOGD(TAG, "Configuration Mode");
#endif //DEBUG
    //digitalWrite(PIN_LED_GREEN, HIGH);
    WiFi.softAP(_ssid, _password);
    _server.begin();
    delay(1000);
    _dnsServer.setTTL(300);
    _dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
    if (!_dnsServer.start(53, "*", WiFi.softAPIP()))
    {
#ifdef DEBUG
        ESP_LOGE(TAG, "Error init dns Server");
#endif //DEBUG
    }
    delay(1000);
    unsigned long currentTime = millis();
    while (true)
    {
        if (millis() - currentTime >= 2000)
        {
            digitalWrite(PIN_LED_RED, HIGH);
            delay(100);
            digitalWrite(PIN_LED_RED, LOW);
            currentTime = millis();
        }
        if (digitalRead(PIN_BUTTON))
        {
            uint32_t times = 0;
            while (digitalRead(PIN_BUTTON))
            {
                delay(1);
                times++;
                if (times >= 1000)
                {
                    WiFi.mode(WIFI_MODE_NULL);
                    digitalWrite(PIN_LED_RED, LOW);
                    return true;
                }
            }
        }

        delay(100);
        yield();
        _dnsServer.processNextRequest();
        if (_configurationCompleted)
        {
            _configurationCompleted = false;
            break;
        }
    }
    WiFi.mode(WIFI_MODE_NULL);
    digitalWrite(PIN_LED_RED, LOW);
    return true;
}