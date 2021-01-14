#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "AsyncJson.h"

AsyncWebServer server(80);

const char *ssid = "PORTAO";
const char *password = "123456789";

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
} users[50];

struct MESSAGES
{
  String message;
  bool relay1;
  bool relay2;
} messages[50];

struct HISTORY
{
  String name;
  String message;
  String date;
  String hour;
  bool relay1;
  bool relay2;
} history[100];

DynamicJsonDocument jsonConfigDoc(1024);
String jsonConfig = "{\"users\":[],\"messages\":[]}";
uint8_t usersCount, messagesCount = 0;
DynamicJsonDocument jsonHistoryDoc(1024);
String jsonHistory = "{\"history\":[]}";
bool receivedData = false;

StaticJsonDocument<1024> data;

void updateSerial();
String stringSpecialCharFormat(String inputStr);
String hexToAscii(String hex);
void checkCall(String str);
void checkSms(String str);
String normalize(String inputStr);

bool saveConfiguration();
bool loadConfiguration2Struct();
bool createJsonConfiguration();

void setup()
{
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  if (!SPIFFS.begin())
  {
    ESP_LOGE(TAG, "An error has occurred while mounting LittleFS");
  }
  ESP_LOGD(TAG, "FS mounted successfullt");

  if (SPIFFS.exists("/config"))
  {
    File file = SPIFFS.open("/config", "r");
    if (!file)
    {
      Serial.println("No Config Exist");
    }
    else
    {
      DeserializationError error = deserializeJson(jsonConfigDoc, file);

      // Test if parsing succeeds.
      if (error)
      {
        ESP_LOGE(TAG, "deserializeJson() failed: %s", error.f_str());
      }
      loadConfiguration2Struct();
      file.close();
      serializeJson(jsonConfigDoc, Serial);
    }
  }
  else
  {
    createJsonConfiguration();
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.serveStatic("/", SPIFFS, "/");

  server.on("/info", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", jsonConfig);
    //json=String();
  });

  AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/update", [&](AsyncWebServerRequest *request, JsonVariant &json) {
    jsonConfigDoc.clear();
    jsonConfigDoc = json.as<JsonObject>();
    /* String response;
    serializeJson(jsonConfigDoc, response); */

    request->send(200, "application/json", json);
    //Serial.println(response);
    receivedData = true;
  });
  server.addHandler(handler);
  server.begin();

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

  Serial.println("WIFI begin");
  SerialAT.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  Serial.println("Qualidade do sinal: ");

  SerialAT.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
  updateSerial();
  delay(1000);

  Serial.println("Sim info: ");

  SerialAT.println("AT+CCID"); //Read SIM information to confirm whether the SIM is plugged
  updateSerial();
  delay(1000);

  Serial.println("Network: ");

  SerialAT.println("AT+CREG?"); //Check whether it has registered in the network
  updateSerial();
  delay(1000);

  SerialAT.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  delay(1000);

  SerialAT.println("AT+CSCS=\"UCS2\"");
  updateSerial();
  delay(1000);
  /*   SerialAT.println("AT+CSMP=17,168,0,8");
  updateSerial(); */
  delay(1000);

  SerialAT.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
  delay(1000);
  SerialAT.println("AT+CCLK? "); // Decides how newly arrived SMS messages should be handled
  updateSerial();

  delay(1000);
}

void loop()
{
  //updateSerial();
  if (receivedData)
  {
    saveConfiguration();
    loadConfiguration2Struct();
    receivedData = false;
    serializeJsonPretty(jsonConfigDoc, Serial);
  }
  if (SerialAT.available())
  {
    String str = "";
    while (SerialAT.available())
    {
      str += (char)SerialAT.read();
      delay(10);
    }
    if (str != "")
    {
      ESP_LOGD(TAG, "Received String: %s", str.c_str());
      checkCall(str);
      checkSms(str);
    }
  }
}

void checkCall(String str)
{
  if (str.indexOf("+CLIP:") != -1)
  {
    Serial.print("INDEX: ");
    Serial.println(str.indexOf("916235197"));
    String number = str.substring(18, 27);
    Serial.print("NUMBER: ");
    Serial.println(number);
    if (number == "916235197")
    {
      delay(2000);

      SerialAT.println("ATH");
    }
  }
}

void checkSms(String str)
{
  if (str.indexOf("+CMT:") != -1)
  {

    int index = str.indexOf("\"");
    int indexEnd = str.indexOf("\"", index + 1);
    String number = str.substring(index + 1, indexEnd);
    number = hexToAscii(number);
    Serial.print("Received Number: ");
    Serial.println(number);
    index = str.indexOf('\n', 2);
    String message = str.substring(index + 1, str.length() - 2);
    String out = hexToAscii(message);
    message = stringSpecialCharFormat(out);
    //message = normalize(out);
    Serial.print("Received Message: ");
    Serial.println(message);

    for (size_t i = 0; i < 3; i++)
    {
      if (number == ("+351" + users[i].number))
      {
        Serial.print("Authorized");
        index = str.indexOf('\n', 2);
        String message = str.substring(index + 1, str.length() - 2);
        String out = hexToAscii(message);
        message = stringSpecialCharFormat(out);
        Serial.print("Received Message: ");
        Serial.println(message);

        for (size_t i = 0; i < 3; i++)
        {
          //String messageToCompare = stringSpecialCharFormat(messages[i].message.c_str());
          String messageToCompare = normalize(messages[i].message.c_str());
          //messageToCompare.toUpperCase();
          Serial.print("Message to compare: ");
          Serial.println(messageToCompare);
          if (message == messageToCompare)
          {
            Serial.println("Same Message, Authorized access");
            if (messages[i].relay1)
              Serial.println("relay 1");
            if (messages[i].relay2)
              Serial.println("relay 2");
            digitalWrite(RELAY_1, messages[i].relay1);
            digitalWrite(RELAY_2, messages[i].relay2);
            delay(500);
            digitalWrite(RELAY_1, LOW);
            digitalWrite(RELAY_2, LOW);
            break;
          }
        }
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

bool saveConfiguration()
{
  File file = SPIFFS.open("/config", "w");
  serializeJson(jsonConfigDoc, file);
  file.close();
}

bool createJsonConfiguration()
{
  jsonConfigDoc.clear();
  JsonArray usersJson = jsonConfigDoc.createNestedArray("users");
  for (size_t i = 0; i < usersCount; i++)
  {
    JsonObject user = usersJson.createNestedObject();
    user["name"] = users[i].name;
    user["number"] = users[i].number;
  }

  JsonArray messagesJson = jsonConfigDoc.createNestedArray("messages");
  for (size_t i = 0; i < messagesCount; i++)
  {
    JsonObject message = messagesJson.createNestedObject();
    message["message"] = messages[i].message;
    message["relay1"] = messages[i].relay1;
    message["relay2"] = messages[i].relay2;
  }
}

String hexToAscii(String hex)
{
  uint16_t len = hex.length();
  String ascii = "";
  for (uint16_t i = 2; i < len; i += 4)
  {
    ascii += (char)strtol(hex.substring(i, i + 2).c_str(), NULL, 16);
  }
  Serial.print("ASCII: ");
  Serial.println(ascii);

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