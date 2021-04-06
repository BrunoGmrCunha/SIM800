#include "Site.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "AsyncJson.h"
#include <DNSServer.h>
#include <SPIFFS.h>
#include <flashMemory/FlashMemory.h>

AsyncWebServer _server(80);
DNSServer _dnsServer;
DynamicJsonDocument receivedConfigDoc(12288);
const char *_ssid = "PORTAO";
const char *_password = "123456789";
bool _receivedData = false;

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

void siteSetup()
{

  _server.serveStatic("/", SPIFFS, "/");

  _server.on("/info", HTTP_GET, [](AsyncWebServerRequest *request) {
    String jsonConfig = getJsonConfig();
    request->send(200, "application/json", jsonConfig);
    //json=String();
  });

  _server.on("/signalStrength", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hello World!");
    //json=String();
  });

  _server.on("/history", HTTP_GET, [](AsyncWebServerRequest *request) {
    String jsonHistory = readHistory();
    request->send(200, "application/json", jsonHistory);
    //json=String();
  });

  AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/update", [&](AsyncWebServerRequest *request, JsonVariant &json) {
    receivedConfigDoc.clear();
    receivedConfigDoc = json.as<JsonObject>();
    request->send(200, "application/json", json);
    _receivedData = true;
    setJsonConfig((receivedConfigDoc));
  });

  _server.addHandler(handler);

  _server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP
}

void siteConfiguration()
{
#ifdef DEBUG
  ESP_LOGD(TAG, "Configuration Mode");
#endif //DEBUG
  digitalWrite(PIN_LED_GREEN, HIGH);
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
      digitalWrite(PIN_LED_GREEN, HIGH);
      delay(100);
      digitalWrite(PIN_LED_GREEN, LOW);
      currentTime = millis();
    }
    if (digitalRead(PIN_BUTTON))
    {
      //ESP_LOGD(TAG, "button pressed");
      uint32_t times = 0;
      while (digitalRead(PIN_BUTTON))
      {
        delay(1);
        times++;
        if (times >= 1000)
        {
          WiFi.mode(WIFI_MODE_NULL);
          digitalWrite(PIN_LED_GREEN, LOW);
          return;
        }
      }
    }

    delay(100);
    yield();
    _dnsServer.processNextRequest();
    if (_receivedData)
    {
      break;
    }
  }
  WiFi.mode(WIFI_MODE_NULL);
  digitalWrite(PIN_LED_GREEN, LOW);
}

void getReceivedJson(DynamicJsonDocument &receivedDoc)
{
  receivedDoc.clear();
  receivedDoc = receivedConfigDoc;
}

bool receivedData()
{
  return _receivedData;
}

void setReceivedData(bool state)
{
  _receivedData = state;
}

