#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <Gsm.h>
#include <FlashMemory.h>
#include <WebPage.h>
#include "constants.h"

static Gsm _gsm;
static FlashMemory _flashMemory;
static WebPage _webPage(&_gsm, &_flashMemory);

Users _users[MAX_USERS];

Messages _messages[MAX_MESSAGES];

History _history;

uint8_t _usersCount, _messagesCount, _historyCount = 0;
/*
StaticJsonDocument<1024> _data;
String normalize(String inputStr);
void checkCall(String str);
void checkSms(String str);
String stringSpecialCharFormat(String inputStr);
String hexToAscii(String hex); */

void configurationMode();
void checkGsm();
bool isAuthorizedNumber(String &number);
bool isAuthorizedSms(String &message);
String normalize(String inputStr);

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_RELAY_1, OUTPUT);
  pinMode(PIN_RELAY_2, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);

  _gsm.begin();
  _flashMemory.begin();

  _gsm.checkSignalStrength();

  _webPage.begin();
  digitalWrite(PIN_LED_RED, HIGH);

  if (!_flashMemory.loadConfiguration())
  {
    if (_webPage.configuration())
    {
      _flashMemory.loadConfiguration2Struct(_usersCount, _messagesCount, _users, _messages);
    }
  }
  else
  {
    _flashMemory.loadConfiguration2Struct(_usersCount, _messagesCount, _users, _messages);
  }
}
String _receivedStr;
void loop()
{
  configurationMode();
  checkGsm();

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

void configurationMode()
{
  if (digitalRead(PIN_BUTTON))
  {
#ifdef DEBUG
    ESP_LOGD(TAG, "Button pressed");
#endif //DEBUG
    uint32_t times = 0;
    unsigned long startTime = millis();
    while (digitalRead(PIN_BUTTON) && (times < 10000))
    {
      delay(1);
      times++;
      if (times >= 2000 && times < 6000)
      {
        if (millis() - startTime >= 1000)
        {
          digitalWrite(PIN_LED_GREEN, HIGH);
          delay(100);
          digitalWrite(PIN_LED_GREEN, LOW);
          startTime = millis();
        }
      }
      else if (times >= 6000)
      {
        if (millis() - startTime >= 1000)
        {
          digitalWrite(PIN_LED_GREEN, HIGH);
          digitalWrite(PIN_LED_RED, HIGH);
          delay(100);
          digitalWrite(PIN_LED_GREEN, LOW);
          digitalWrite(PIN_LED_RED, LOW);
          startTime = millis();
        }
      }
    }
    if (times >= 2000 && times < 6000)
    {
      digitalWrite(PIN_LED_GREEN, HIGH);
#ifdef DEBUG
      ESP_LOGD(TAG, "Configuration Mode");
#endif //DEBUG
      if (!_gsm.checkSignalStrength())
      {
        ESP_LOGE(TAG, "Signal Stregn is Negative");
      }
      _flashMemory.loadConfiguration();
      if (_webPage.configuration())
      {
        _flashMemory.loadConfiguration2Struct(_usersCount, _messagesCount, _users, _messages);
      }
    }
    else if (times >= 6000)
    {
      digitalWrite(PIN_LED_GREEN, HIGH);
      digitalWrite(PIN_LED_RED, HIGH);
      SPIFFS.remove("/config");
      SPIFFS.remove("/history");
      ESP.restart();
      ESP_LOGW(TAG, "FactoryReset");
    }
  }
}

void checkGsm()
{
  String number, message, date, hour;
  uint8_t communicationType = _gsm.received(number, message, date, hour);
  switch (communicationType)
  {
  case 1:
  {
#ifdef DEBUG
    ESP_LOGD(TAG, "Received Message \nMessage: %s \nNumber: %s \nDate: %s \nHour: %s", message.c_str(), number.c_str(), date.c_str(), hour.c_str());
#endif //DEBUG
    String _number = number;
    _number.remove(0, 4);
    if (!isAuthorizedNumber(_number))
    {
#ifdef DEBUG
      ESP_LOGW(TAG, "Not authorized number");
#endif //DEBUG
      break;
    }
    if (!isAuthorizedSms(message))
    {
#ifdef DEBUG
      ESP_LOGW(TAG, "Not authorized sms");
#endif //DEBUG
      break;
    }
#ifdef DEBUG
    ESP_LOGD(TAG, "Authorized Number and SMS");
#endif //DEBUG
  }
  break;
  case 2:
  {
#ifdef DEBUG
    ESP_LOGD(TAG, "Received Call \nNumber: %s \nDate: %s \nHour: %s", number.c_str(), date.c_str(), hour.c_str());
#endif //DEBUG
    if (!isAuthorizedNumber(number))
    {
#ifdef DEBUG
      ESP_LOGW(TAG, "Not authorized number");
#endif //DEBUG
      break;
    }
#ifdef DEBUG
    ESP_LOGD(TAG, "Authorized Call");
#endif //DEBUG
  }
  break;

  default:
    break;
  }
}

bool isAuthorizedNumber(String &number)
{
  for (uint8_t i = 0; i < _usersCount; i++)
  {
    ESP_LOGD(TAG, "Number: %s \tNumber to Compare: %s", number.c_str(), _users[i].number.c_str());
    if (number == _users[i].number)
    {
      return true;
    }
  }
  return false;
}

bool isAuthorizedSms(String &message)
{
  for (size_t i = 0; i < _messagesCount; i++)
  {
    String message2Compare = normalize(_messages[i].message);
    ESP_LOGD(TAG, "Received Message: %s \tMessage to Compare: %s", message.c_str(), message2Compare.c_str());

    if (message == message2Compare)
    {
      return true;
    }
  }
  return false;
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
