#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <Gsm.h>
#include <FlashMemory.h>
#include <WebPage.h>
#include "constants.h"
#include <Preferences.h>

static Gsm _gsm;
static FlashMemory _flashMemory;
static WebPage _webPage(&_gsm, &_flashMemory);
Preferences preferences;

Users _users[MAX_USERS];

Messages _messages[MAX_MESSAGES];

Calls _calls[1];

History _history;

uint8_t _usersCount, _messagesCount, _historyCount, _callsCount = 0;

void configurationMode();
void checkGsm();
bool isAuthorizedNumber(String &number);
bool isAuthorizedSms(String &message, bool &relay1, bool &relay2);
bool isAuthorizedCall(String &number, bool &relay1, bool &relay2);

String normalize(String inputStr);

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_RELAY_1, OUTPUT);
  pinMode(PIN_RELAY_2, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_RED, HIGH);
  preferences.begin("my-app", false);

  unsigned int counter = preferences.getUInt("counter", 0);
  counter++;
  ESP_LOGD(TAG, "Current counter value: %u\n", counter);
  preferences.putUInt("counter", counter);

  // Close the Preferences
  preferences.end();
  _gsm.begin();
  _flashMemory.begin();

  _gsm.checkSignalStrength();
  int day, month, year, minute, second, hour;

  _gsm.getDateTime(&day, &month, &year, &hour, &minute, &second);
  String dateTime;
  dateTime += year;
  dateTime += "/";
  dateTime += month;
  dateTime += "/";
  dateTime += day;
  dateTime += " - ";
  dateTime += hour;
  dateTime += ":";
  dateTime += minute;
  dateTime += ":";
  dateTime += second;
  dateTime += "\n";
  _flashMemory.writeLog(dateTime);
  _webPage.begin();
  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(PIN_LED_GREEN, HIGH);

  if (!_flashMemory.loadConfiguration())
  {
    if (_webPage.configuration())
    {
      _flashMemory.loadConfiguration2Struct(_usersCount, _messagesCount, _callsCount, _users, _messages, _calls);
    }
  }
  else
  {
    _flashMemory.loadConfiguration2Struct(_usersCount, _messagesCount, _callsCount, _users, _messages, _calls);
  }
}
String _receivedStr;
void loop()
{
  configurationMode();
  checkGsm();
  /*   if (Serial.available())
  {
    String atCommand = Serial.readString();
    ESP_LOGD(TAG, "At command %s", atCommand.c_str());
    String respnse = _gsm.send(atCommand);
    ESP_LOGD(TAG, "Response %s", respnse.c_str());
  } */
}
long lastDebounceTime = 0; // the last time the output pin was toggled
long debounceDelay = 500;  // the debounce time; increase if the output flickers
void configurationMode()
{

  if (!digitalRead(PIN_BUTTON))
  {
    Serial.println(".");

    lastDebounceTime = millis(); //set the current time

    uint32_t times = 0;
    unsigned long startTime = millis();
    while (!digitalRead(PIN_BUTTON) && (times < 10000))
    {
      delay(1);
      times++;
      if (times >= 2000 && times < 6000)
      {
        if (millis() - startTime >= 1000)
        {
          digitalWrite(PIN_LED_RED, HIGH);
          delay(100);
          digitalWrite(PIN_LED_RED, LOW);
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
      digitalWrite(PIN_LED_RED, HIGH);
#ifdef DEBUG
      ESP_LOGD(TAG, "Configuration Mode");
#endif //DEBUG
      if (!_gsm.checkSignalStrength())
      {
#ifdef DEBUG
        ESP_LOGE(TAG, "Signal Stregn is Negative");
#endif //DEBUG
      }
      _flashMemory.loadConfiguration();
      if (_webPage.configuration())
      {
        _flashMemory.loadConfiguration2Struct(_usersCount, _messagesCount, _callsCount, _users, _messages, _calls);
      }
    }
    else if (times >= 6000)
    {
      digitalWrite(PIN_LED_GREEN, HIGH);
      digitalWrite(PIN_LED_RED, HIGH);
      SPIFFS.remove("/config");
      SPIFFS.remove("/history");
      ESP.restart();
#ifdef DEBUG
      ESP_LOGW(TAG, "FactoryReset");
#endif //DEBUG
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
    bool relay1 = false;
    bool relay2 = digitalRead(PIN_RELAY_2);
    _number.remove(0, 4);
    if (!isAuthorizedNumber(_number))
    {
#ifdef DEBUG
      ESP_LOGW(TAG, "Not authorized number");
#endif //DEBUG
      break;
    }
    if (!isAuthorizedSms(message, relay1, relay2))
    {
#ifdef DEBUG
      ESP_LOGW(TAG, "Not authorized sms");
#endif //DEBUG
      break;
    }
#ifdef DEBUG
    ESP_LOGD(TAG, "Authorized Number and SMS");
#endif //DEBUG

    digitalWrite(PIN_LED_RED, HIGH);
    if (digitalRead(PIN_RELAY_2) != relay2)
    {
      digitalWrite(PIN_RELAY_2, relay2);
      digitalWrite(PIN_RELAY_2, relay2);

      delay(DELAY_POWER);
    }

    digitalWrite(PIN_RELAY_1, relay1);
    //digitalWrite(PIN_RELAY_2, relay2);
    delay(500);

    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_RELAY_1, LOW);
    //digitalWrite(PIN_RELAY_2, LOW);
  }
  break;
  case 2:
  {
    String _number = number;
    bool relay1, relay2 = false;
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
    if (!isAuthorizedCall(number, relay1, relay2))
    {
#ifdef DEBUG
      ESP_LOGW(TAG, "Not authorized call");
#endif //DEBUG
      break;
    }
#ifdef DEBUG
    ESP_LOGD(TAG, "Authorized Call");
#endif //DEBUG
    digitalWrite(PIN_LED_RED, HIGH);
    if (digitalRead(PIN_RELAY_2) != relay2)
    {
      digitalWrite(PIN_RELAY_2, relay2);
      digitalWrite(PIN_RELAY_2, relay2);

      delay(DELAY_POWER);
    }

    digitalWrite(PIN_RELAY_1, relay1);
    //digitalWrite(PIN_RELAY_2, relay2);
    delay(500);

    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_RELAY_1, LOW);
    //digitalWrite(PIN_RELAY_2, LOW);
    // _gsm.hangUp();
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
#ifdef DEBUG
    ESP_LOGD(TAG, "Number: %s \tNumber to Compare: %s", number.c_str(), _users[i].number.c_str());
#endif //DEBUG
    if (number == _users[i].number)
    {
      return true;
    }
  }
  return false;
}

bool isAuthorizedSms(String &message, bool &relay1, bool &relay2)
{

  if (message == "ligar")
  {
    relay2 = LOW;
    return true;
  }
  else if (message == "desligar")
  {
    relay2 = HIGH;
    return true;
  }

  for (size_t i = 0; i < _messagesCount; i++)
  {
    String message2Compare = normalize(_messages[i].message);
#ifdef DEBUG
    ESP_LOGD(TAG, "Received Message: %s \tMessage to Compare: %s", message.c_str(), message2Compare.c_str());
#endif //DEBUG
    if (message == message2Compare)
    {
      relay1 = _messages[i].relay1;
      relay2 = LOW;
      return true;
    }
  }
  relay1 = false;
  relay2 = digitalRead(PIN_RELAY_2);

  return false;
}

bool isAuthorizedCall(String &number, bool &relay1, bool &relay2)
{
  relay1 = _calls[0].relay1;
  relay2 = LOW;
  return true;
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
