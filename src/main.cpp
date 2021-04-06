#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <Gsm.h>
//#include <gsmModem/GsmModem.h>
//#include <site/Site.h>
//#include "flashMemory/FlashMemory.h"
#include "constants.h"

static Gsm _gsm;

/* Users _users[MAX_USERS];

Messages _messages[MAX_MESSAGES];

History _history;

uint8_t _usersCount, _messagesCount, _historyCount = 0;

StaticJsonDocument<1024> _data;

void configurationMode();
String normalize(String inputStr);

void checkCall(String str);
void checkSms(String str);
String stringSpecialCharFormat(String inputStr);
String hexToAscii(String hex); */
void setup()
{

  Serial.begin(115200);
  pinMode(PIN_RELAY_1, OUTPUT);
  pinMode(PIN_RELAY_2, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);

  _gsm.begin();

/*   sim800Setup();
  flashMemorySetup(_usersCount, _messagesCount, &_users, &_messages);
  siteSetup();
  gsmInit();
  digitalWrite(PIN_LED_RED, HIGH); */
}
String _receivedStr;
void loop()
{

/*   if (receivedData())
  {

    saveConfiguration();
    loadConfiguration2Struct(_usersCount, _messagesCount, &_users, &_messages);
    setReceivedData(false);
  }
  configurationMode();
  if (received(_receivedStr))
  {
    if (_receivedStr != "")
    {
      ESP_LOGD(TAG, "Received String: %s", _receivedStr.c_str());
      if (_receivedStr.indexOf("+CLIP:") != -1)
      {
        checkCall(_receivedStr);
      }
      else if (_receivedStr.indexOf("+CMT:") != -1)
      {
        checkSms(_receivedStr);
      }
    }
  } */
}
/* 
void checkCall(String str)
{
  String number = str.substring(18, 27);
  ESP_LOGD(TAG, "Number : %s", number.c_str());
  for (size_t i = 0; i < _usersCount; i++)
  {
    if (number == _users[i].number)
    {
      callReject();
#ifdef DEBUG
      ESP_LOGD(TAG, "Received call from: %s", _users[i].name.c_str());
#endif //DEBUG
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
  ESP_LOGD(TAG, "Received Number %s", number.c_str());
  String date = str.substring(indexEnd + 6, indexEnd + 14);
  String hour = str.substring(indexEnd + 15, indexEnd + 23);
  _history.date = date;
  _history.hour = hour;
  index = str.indexOf('\n', 2);
  String message = str.substring(index + 1, str.length() - 2);
  String out = hexToAscii(message);
  message = stringSpecialCharFormat(out);
  ESP_LOGD(TAG, "Message received from: %s ", number.c_str());
  ESP_LOGD(TAG, "Message: %s ", message.c_str());
  ESP_LOGD(TAG, "Date: %s", date);
  ESP_LOGD(TAG, "Hour: %s", hour);

  for (size_t i = 0; i < _usersCount; i++)
  {
    if (number == ("+351" + _users[i].number))
    {
      _history.name = _users[i].name;
      ESP_LOGD(TAG, "Authorized number, name: %s", _users[i].name.c_str());
      index = str.indexOf('\n', 2);
      for (size_t i = 0; i < _messagesCount; i++)
      {
        String messageToCompare = normalize(_messages[i].message.c_str());
        if (message == messageToCompare)
        {
          ESP_LOGD(TAG, "Message accepted");
          if (_messages[i].relay1)
          {
            ESP_LOGD(TAG, "RELAY 1");
          }
          if (_messages[i].relay2)
          {
            ESP_LOGD(TAG, "RELAY 2");
          }
          digitalWrite(PIN_RELAY_1, _messages[i].relay1);
          digitalWrite(PIN_RELAY_2, _messages[i].relay2);
          delay(500);
          digitalWrite(PIN_RELAY_1, LOW);
          digitalWrite(PIN_RELAY_2, LOW);
          _history.message = _messages[i].message;
          _history.relay1 = _messages[i].relay1;
          _history.relay2 = _messages[i].relay2;
          writeHistory(_history);
          break;
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

      siteConfiguration();
    }
  }
} */