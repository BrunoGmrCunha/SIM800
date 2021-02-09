#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "AsyncJson.h"
#include <DNSServer.h>

AsyncWebServer server(80);
DNSServer _dnsServer;

const char *ssid = "PORTAO";
const char *password = "123456789";

const uint8_t MAX_USERS = 10;
const uint8_t MAX_MESSAGES = 10;
const uint8_t MAX_HISTORY = 50;

const uint8_t PIN_RELAY_1 = 32;
const uint8_t PIN_RELAY_2 = 33;
const uint8_t PIN_BUTTON = 15;
const uint8_t PIN_LED_GREEN = 13;
const uint8_t PIN_LED_RED = 25;
// TTGO T-Call pin definitions
#define MODEM_RST 5
#define MODEM_PWKEY 4
#define MODEM_POWER_ON 23
#define MODEM_TX 27
#define MODEM_RX 26
#define I2C_SDA 21
#define I2C_SCL 22
#define RELAY_1 18
#define RELAY_2 19

#define SerialAT Serial1
struct USERS
{
  String name;
  String number;
} users[MAX_USERS];

struct MESSAGES
{
  String message;
  bool relay1;
  bool relay2;
} messages[MAX_MESSAGES];

struct HISTORY
{
  String name;
  String message;
  String date;
  String hour;
  bool relay1;
  bool relay2;
} history;

DynamicJsonDocument jsonConfigDoc(12288);
String jsonConfig = "{\"users\":[],\"messages\":[]}";
uint8_t usersCount, messagesCount, historyCount = 0;
DynamicJsonDocument jsonHistoryDoc(12288);
String jsonHistory = "{\"history\":[]}";
bool receivedData = false;

StaticJsonDocument<1024> data;

void updateSerial();
String stringSpecialCharFormat(String inputStr);
String hexToAscii(String hex);
void checkCall(String str);
void checkSms(String str);
void checkQuality();
String normalize(String inputStr);

bool saveConfiguration();
bool loadConfiguration2Struct();
bool createJsonConfiguration();
void writeHistory();
void readHistory();

void configurationMode();

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

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_RELAY_1, OUTPUT);
  pinMode(PIN_RELAY_2, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  delay(200);
  digitalWrite(PIN_LED_RED, HIGH);

  if (!SPIFFS.begin())
  {
    ESP_LOGE(TAG, "An error has occurred while mounting LittleFS");
  }
  ESP_LOGD(TAG, "FS mounted successfully");

  if (SPIFFS.exists("/config"))
  {
    File file = SPIFFS.open("/config", "r");
    DeserializationError error = deserializeJson(jsonConfigDoc, file);
    if (error)
    {
      ESP_LOGE(TAG, "deserializeJson() failed: %s", error.f_str());
      SPIFFS.format();
    }
    loadConfiguration2Struct();
    file.close();
    String message;
    serializeJsonPretty(jsonConfigDoc, message);
    ESP_LOGD(TAG, "Json configuration doc: %s", message.c_str());
  }
  server.serveStatic("/", SPIFFS, "/");
  server.on("/info", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", jsonConfig);
    //json=String();
  });
  server.on("/signalStrength", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hello World!");
    //json=String();
  });
  server.on("/history", HTTP_GET, [](AsyncWebServerRequest *request) {
    readHistory();
    request->send(200, "application/json", jsonHistory);
    //json=String();
  });

  AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/update", [&](AsyncWebServerRequest *request, JsonVariant &json) {
    jsonConfigDoc.clear();
    jsonConfigDoc = json.as<JsonObject>();
    request->send(200, "application/json", json);
    receivedData = true;
  });
  server.addHandler(handler);

  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP

  // Set-up modem reset, enable, power pins

  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);

  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(6000);

  SerialAT.println("AT"); //Once the handshake test is successful, it will back to OK
  delay(1000);
  updateSerial();
  Serial.println("\n");

  SerialAT.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
  delay(1000);
  updateSerial();
  Serial.println("\n");

  SerialAT.println("AT+CCID"); //Read SIM information to confirm whether the SIM is plugged
  delay(1000);
  updateSerial();
  Serial.println("\n");

  SerialAT.println("AT+CREG?"); //Check whether it has registered in the network
  delay(1000);
  updateSerial();
  Serial.println("\n");

  SerialAT.println("AT+CMGF=1"); // Configuring TEXT mode
  delay(1000);
  updateSerial();
  Serial.println("\n");

  SerialAT.println("AT+CSCS=\"UCS2\"");
  delay(1000);
  updateSerial();
  Serial.println("\n");

  SerialAT.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  delay(1000);
  updateSerial();
  Serial.println("\n");

  SerialAT.println("AT+CCLK? "); // Decides how newly arrived SMS messages should be handled
  delay(1000);
  updateSerial();
  Serial.println("\n");
  delay(1000);
  SerialAT.write("AT+CSQ\n"); //Signal quality test, value range is 0-31 , 31 is the best
  delay(2000);
  //updateSerial();
  checkQuality();
  Serial.println("\n");
}

void loop()
{
  if (receivedData)
  {
    saveConfiguration();
    loadConfiguration2Struct();
    receivedData = false;
    String message;
    serializeJsonPretty(jsonConfigDoc, message);
    ESP_LOGD(TAG, "Received configuration: %s", message.c_str());
  }
  configurationMode();
  if (SerialAT.available())
  {
    String receivedStr = "";
    while (SerialAT.available())
    {
      receivedStr += (char)SerialAT.read();
      delay(10);
    }
    if (receivedStr != "")
    {
      ESP_LOGD(TAG, "Received String: %s", receivedStr.c_str());
      if (receivedStr.indexOf("+CLIP:") != -1)
      {
        checkCall(receivedStr);
      }
      else if (receivedStr.indexOf("+CMT:") != -1)
      {
        checkSms(receivedStr);
      }
    }
  }
}

void checkCall(String str)
{
  String number = str.substring(18, 27);
  ESP_LOGD(TAG, "Number : %s", number.c_str());
  for (size_t i = 0; i < usersCount; i++)
  {
    if (number == users[i].number)
    {
      delay(1000);
      SerialAT.println("ATA");
      delay(500);
      SerialAT.println("ATH");
      delay(500);
      ESP_LOGD(TAG, "Received call from: %s", users[i].name.c_str());
      return;
    }
  }
  ESP_LOGW(TAG, "Received call from number not recognized: %s", number.c_str());
}

void checkSms(String str)
{
  int index = str.indexOf("\"");
  int indexEnd = str.indexOf("\"", index + 1);
  String number = str.substring(index + 1, indexEnd);
  number = hexToAscii(number);
  ESP_LOGD(TAG,"Received Number %s", number.c_str());
  String date = str.substring(indexEnd + 6, indexEnd + 14);
  String hour = str.substring(indexEnd + 15, indexEnd + 23);
  history.date = date;
  history.hour = hour;
  index = str.indexOf('\n', 2);
  String message = str.substring(index + 1, str.length() - 2);
  String out = hexToAscii(message);
  message = stringSpecialCharFormat(out);
  ESP_LOGD(TAG, "Message received from: %s ", number.c_str());
  ESP_LOGD(TAG, "Message: %s ", message.c_str());
  ESP_LOGD(TAG, "Date: %s", date);
  ESP_LOGD(TAG, "Hour: %s", hour);

  for (size_t i = 0; i < usersCount; i++)
  {
    if (number == ("+351" + users[i].number))
    {
      history.name = users[i].name;
      ESP_LOGD(TAG, "Authorized number, name: %s", users[i].name.c_str());
      index = str.indexOf('\n', 2);
      for (size_t i = 0; i < messagesCount; i++)
      {
        String messageToCompare = normalize(messages[i].message.c_str());
        if (message == messageToCompare)
        {
          ESP_LOGD(TAG, "Message accepted");
          if (messages[i].relay1)
          {
            ESP_LOGD(TAG, "RELAY 1");
          }
          if (messages[i].relay2)
          {
            ESP_LOGD(TAG, "RELAY 2");
          }
          digitalWrite(PIN_RELAY_1, messages[i].relay1);
          digitalWrite(PIN_RELAY_2, messages[i].relay2);
          delay(500);
          digitalWrite(PIN_RELAY_1, LOW);
          digitalWrite(PIN_RELAY_2, LOW);
          history.message = messages[i].message;
          history.relay1 = messages[i].relay1;
          history.relay2 = messages[i].relay2;
          writeHistory();
          break;
        }
      }
    }
  }
}

void checkQuality()
{
  if (SerialAT.available())
  {
    String receivedStr = "";
    while (SerialAT.available())
    {
      receivedStr += (char)SerialAT.read();
      delay(10);
    }
    if (receivedStr != "")
    {
      ESP_LOGD(TAG, "Received String: %s", receivedStr.c_str());
      if (receivedStr.indexOf("+CSQ: ") != -1)
      {
        String quality = receivedStr.substring(14, receivedStr.indexOf(","));
        int qualityInt = quality.toInt();
        ESP_LOGD(TAG, "Quality:  %d", qualityInt);
      }
    }
  }
}

bool loadConfiguration2Struct()
{
  usersCount = jsonConfigDoc["users"].size();
  for (size_t i = 0; i < usersCount; i++)
  {
    JsonObject user = jsonConfigDoc["users"][i];
    users[i].name = user["name"].as<String>();
    users[i].number = user["number"].as<String>();
  }
  messagesCount = jsonConfigDoc["messages"].size();
  for (size_t i = 0; i < messagesCount; i++)
  {
    JsonObject message = jsonConfigDoc["messages"][i];
    messages[i].message = message["message"].as<String>();
    messages[i].relay1 = message["relay1"];
    messages[i].relay2 = message["relay2"];
  }
  jsonConfig = "";
  serializeJson(jsonConfigDoc, jsonConfig);
}

void writeHistory()
{
  JsonArray historyArray;
  if (SPIFFS.exists("/history"))
  {
    File fileRead = SPIFFS.open("/history", "r");

    DeserializationError error = deserializeJson(jsonHistoryDoc, fileRead);

    // Test if parsing succeeds.
    if (error)
    {
      ESP_LOGE(TAG, "deserializeJson() failed: %s", error.f_str());
      return;
    }
    fileRead.close();
    historyArray = jsonHistoryDoc["history"].as<JsonArray>();
  }
  else
  {
    jsonHistoryDoc.clear();
    historyArray = jsonHistoryDoc.createNestedArray("history");
  }

  JsonObject historyObj = historyArray.createNestedObject();
  historyObj["date"] = history.date;
  historyObj["hour"] = history.hour;
  historyObj["message"] = history.message;
  historyObj["name"] = history.name;
  historyObj["relay1"] = history.relay1;
  historyObj["relay2"] = history.relay2;
  if (historyArray.size() == MAX_HISTORY + 1)
  {
    historyArray.remove(0);
  }
  //historyArray.add(historyObj);
  File fileWrite = SPIFFS.open("/history", "w");
  serializeJson(jsonHistoryDoc, fileWrite);
  fileWrite.close();
}

void readHistory()
{
  if (SPIFFS.exists("/history"))
  {
    File fileRead = SPIFFS.open("/history", "r");

    DeserializationError error = deserializeJson(jsonHistoryDoc, fileRead);

    // Test if parsing succeeds.
    if (error)
    {
      ESP_LOGE(TAG, "deserializeJson() failed: %s", error.f_str());
      return;
    }
    fileRead.close();
    JsonArray historyArray = jsonHistoryDoc["history"].as<JsonArray>();
    jsonHistoryDoc.clear();
    JsonArray orderArray = jsonHistoryDoc.createNestedArray("history");
    int ArrayLength = historyArray.size();

    for (int i = 0; i < ArrayLength; i++)
    {
      orderArray[i] = historyArray[ArrayLength - 1 - i].as<JsonObject>();
      orderArray[ArrayLength - 1 - i] = historyArray[i].as<JsonObject>();
    }
    serializeJson(jsonHistoryDoc, jsonHistory);
    serializeJsonPretty(jsonHistoryDoc, Serial);
  }
  else
  {
    jsonHistory = "";
  }
}

bool saveConfiguration()
{
  File file = SPIFFS.open("/config", "w");
  serializeJson(jsonConfigDoc, file);
  file.close();
}

String hexToAscii(String hex)
{
  uint16_t len = hex.length();
  String ascii = "";
  for (uint16_t i = 2; i < len; i += 4)
  {
    ascii += (char)strtol(hex.substring(i, i + 2).c_str(), NULL, 16);
  }
  return ascii;
}

String stringSpecialCharFormat(String inputStr)
{
  String out;
  int c;
  for (size_t i = 0; i < inputStr.length(); i++)
  {
    c = (int)inputStr.charAt(i);
    if (c > 128)
    {
      switch (c)
      {
      case 32:
        out += "";
        break;
      case 193:
        out += "A";
        break;
      case 194:
        out += "A";
        break;
      case 195:
        out += "A";
        break;
      case 199:
        out += "C";
        break;
      case 200:
        out += "E";
        break;
      case 201:
        out += "E";
        break;
      case 204:
        out += "I";
        break;
      case 205:
        out += "I";
        break;
      case 210:
        out += "O";
        break;
      case 211:
        out += "O";
        break;
      case 212:
        out += "O";
        break;
      case 213:
        out += "O";
        break;
      case 217:
        out += "U";
        break;
      case 218:
        out += "U";
        break;
      case 224:
        out += "a";
        break;
      case 225:
        out += "a";
        break;
      case 226:
        out += "a";
        break;
      case 227:
        out += "a";
        break;
      case 231:
        out += "c";
        break;
      case 232:
        out += "e";
        break;
      case 233:
        out += "e";
        break;
      case 234:
        out += "e";
        break;
      case 236:
        out += "i";
        break;
      case 237:
        out += "i";
        break;
      case 242:
        out += "o";
        break;
      case 243:
        out += "o";
        break;
      case 244:
        out += "o";
        break;
      case 245:
        out += "o";
        break;
      case 249:
        out += "u";
        break;
      case 250:
        out += "u";
        break;
      }
    }
    else
    {
      out += inputStr.charAt(i);
    }
  }
  out.trim();
  out.replace(" ", "");
  out.toLowerCase();
  return out;
}

String normalize(String inputStr)
{
  inputStr.toLowerCase();
  inputStr.trim();
  inputStr.replace("_", "");
  inputStr.replace(".", "");
  inputStr.replace("/", "");
  inputStr.replace("\\", "");
  inputStr.replace("º", "");
  inputStr.replace("ª", "");
  inputStr.replace("ç", "c");
  inputStr.replace("á", "a");
  inputStr.replace("à", "a");
  inputStr.replace("&", "");
  inputStr.replace("%", "");
  inputStr.replace("$", "");
  inputStr.replace("#", "");
  inputStr.replace("!", "");
  inputStr.replace("+", "");
  inputStr.replace(",", "");
  inputStr.replace("\"", "");
  inputStr.replace(" ", "");
  inputStr.replace("â", "a");
  inputStr.replace("ã", "a");
  inputStr.replace("ú", "u");
  inputStr.replace("ù", "u");
  inputStr.replace("é", "e");
  inputStr.replace("è", "e");
  inputStr.replace("ê", "e");
  inputStr.replace("í", "i");
  inputStr.replace("ì", "i");
  inputStr.replace("õ", "o");
  inputStr.replace("ó", "o");
  inputStr.replace("ò", "o");
  inputStr.replace("@", "o");
  inputStr.replace("|", "");
  return inputStr;
}

void updateSerial()
{
  // delay(500);
  while (Serial.available())
  {
    SerialAT.write(Serial.read()); //Forward what Serial received to Software Serial Port
  }
  while (SerialAT.available())
  {
    Serial.write(SerialAT.read()); //Forward what Software Serial received to Serial Port
  }
}

void configurationMode()
{
  if (digitalRead(PIN_BUTTON))
  {
    //ESP_LOGD(TAG, "button pressed");
    uint32_t times = 0;
    while (digitalRead(PIN_BUTTON) && (times < 2000))
    {
      delay(1);
      times++;
    }
    if (times >= 2000)
    {
      ESP_LOGD(TAG, "Configuration Mode");
      digitalWrite(PIN_LED_GREEN, HIGH);
      WiFi.softAP(ssid, password);
      server.begin();
      delay(1000);
      _dnsServer.setTTL(300);
      _dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
      if (!_dnsServer.start(53, "*", WiFi.softAPIP()))
      {
        ESP_LOGE(TAG, "Error init dns Server");
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
        if (receivedData)
        {
          break;
        }
      }
      WiFi.mode(WIFI_MODE_NULL);
      digitalWrite(PIN_LED_GREEN, LOW);
    }
  }
}