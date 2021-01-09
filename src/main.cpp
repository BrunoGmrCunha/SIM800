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

#define SerialAT Serial1

String messages[3] = {
    "Abrir portão de dentro",
    "ABRIR PORTÃO DE FORA",
    "Abrir portões"};

String numbers[3] = {"916235197", "913068935", "912696938"};

String JSON = "{\"users\":[{\"name\":\"Bruno\", \"number\":\"916235197\"}, {\"name\":\"João\", \"number\":\"913068936\"}],\"messages\":[{\"message\":\"Dentro\",\"relay1\":\"1\",\"relay2\":\"0\"},{\"message\":\"Fora\",\"relay1\":\"0\",\"relay2\":\"1\"}]}";

void updateSerial();
String stringSpecialCharFormat(String inputStr);
String hexToAscii(String hex);
void checkCall(String str);
void checkSms(String str);

void setup()
{
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  if (!SPIFFS.begin())
  {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.serveStatic("/", SPIFFS, "/");

  server.on("/info", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = JSON;
    request->send(200, "application/json", json);
    json = String();
  });

  /* 
  AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/rest/endpoint", [](AsyncWebServerRequest *request, JsonVariant &json) {
  StaticJsonDocument<1000>& jsonDoc= json.as<StaticJsonDocument>();
  // ...
}); */
  StaticJsonDocument<200> data;
  bool receivedData = false;
  AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/update", [&](AsyncWebServerRequest *request, JsonVariant &json) {
    if (json.is<JsonArray>())
    {
      data = json.as<JsonArray>();
    }
    else if (json.is<JsonObject>())
    {
      data = json.as<JsonObject>();
    }
    String response;
    serializeJson(data, response);
    request->send(200, "application/json", response);
    Serial.println(response);
    receivedData = true;
  });
  server.addHandler(handler);
  /*   server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = JSON;
    request->send(200, "application/json", json);
    json = String();
  });  */

  /*   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", "{\"message\":\"Welcome\"}");
  }); */
  /*    server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<100> data;
    if (request->hasParam("users"))
    {
      data["users"] = request->getParam("users")->value();
    }
    else
    {
      data["users"] = "No message parameter";
    }
    String response;
    serializeJson(data, response);
    serializeJson(data, Serial);
    request->send(200, "application/json", response);
  }); 
 */
  server.begin();
  while (1 && !receivedData)
  {
    delay(100);
  }

  Serial.print("Data: ");
  serializeJsonPretty(data,Serial);
  // Set-up modem reset, enable, power pins

  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);

  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);
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
  SerialAT.println("AT+CSMP=17,168,0,8");
  updateSerial();
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
      Serial.print("STRING: ");
      Serial.println(str);

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
    for (size_t i = 0; i < 3; i++)
    {
      if (number == ("+351" + numbers[i]))
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
          String messageToCompare = stringSpecialCharFormat(messages[i]);
          messageToCompare.toUpperCase();
          Serial.print("Message to compare: ");
          Serial.println(messageToCompare);
          if (message == messageToCompare)
          {
            Serial.println("Same Message, Authorized access");
            break;
          }
        }
      }
    }
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
  if (out.endsWith(" "))
  {
    out.remove(out.length() - 1);
  }
  out.toUpperCase();
  return out;
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