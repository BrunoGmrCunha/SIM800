#include "Gsm.h"

const uint8_t SIM800_RST = 5;
const uint8_t SIM800_PWKEY = 4;
const uint8_t SIM800_POWER_ON = 23;
const uint8_t SIM800_TX = 27;
const uint8_t SIM800_RX = 26;
const uint8_t SIM800_I2C_SDA = 21;
const uint8_t SIM800_I2C_SCL = 22;

String _signalStrength = "";

#define GsmSerial Serial1

void Gsm::begin()
{
  pinMode(SIM800_PWKEY, OUTPUT);
  pinMode(SIM800_RST, OUTPUT);
  pinMode(SIM800_POWER_ON, OUTPUT);
  delay(200);
  digitalWrite(SIM800_PWKEY, LOW);
  digitalWrite(SIM800_RST, HIGH);
  digitalWrite(SIM800_POWER_ON, HIGH);

  GsmSerial.begin(115200, SERIAL_8N1, SIM800_RX, SIM800_TX);
  delay(2000);

  String receivedResponse = "";
  while (receivedResponse.indexOf("OK") == -1)
  {
    GsmSerial.println("AT"); //Once the handshake test is successful, it will back to OK
    delay(1000);
    receivedResponse = updateSerial();
#ifdef DEBUG
    ESP_LOGD(TAG, " AT RESPONSE: %s", receivedResponse.c_str());
#endif //DEBUG
    unsigned long startTime = millis();
    String receivedMessage;
    while (millis() - startTime < 5000)
    {
      receivedMessage = updateSerial().c_str();
      if (receivedMessage != "")
      {
#ifdef DEBUG
        ESP_LOGD(TAG, "Received Message %s", receivedMessage.c_str());
#endif //DEBUG
        break;
      }
      delay(100);
    }
  }
  String receivedMessage;
  GsmSerial.println("AT+CCID"); //Read SIM information to confirm whether the SIM is plugged
  delay(1000);
  receivedMessage = updateSerial();
#ifdef DEBUG
  ESP_LOGD(TAG, " AT+CCID RESPONSE: %s", receivedMessage.c_str());
#endif //DEBUG
  if (!checkOK(receivedMessage))
  {
#ifdef DEBUG
    ESP_LOGE(TAG, "ERROR AT COMMAND CCID, %s", receivedMessage.c_str());
#endif //DEBUG
  }

  GsmSerial.println("AT+CREG?"); //Check whether it has registered in the network
  receivedMessage = updateSerial();

#ifdef DEBUG
  ESP_LOGD(TAG, " AT+CREG? RESPONSE: %s", updateSerial().c_str());
#endif //DEBUG

  GsmSerial.println("AT+CMGF=1"); // Configuring TEXT mode

#ifdef DEBUG
  ESP_LOGD(TAG, "AT+CMGF=1 RESPONSE: %s", updateSerial().c_str());
#endif //DEBUG

  GsmSerial.println("AT+CSCS=\"UCS2\"");

#ifdef DEBUG
  ESP_LOGD(TAG, "AT+CSCS=\"UCS2\" RESPONSE: %s", updateSerial().c_str());
#endif                                    //DEBUG
  GsmSerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
#ifdef DEBUG
  ESP_LOGD(TAG, "AT+CNMI=1,2,0,0,0 RESPONSE: %s", updateSerial().c_str());
#endif                            //DEBUG
  GsmSerial.println("AT+CCLK? "); // Decides how newly arrived SMS messages should be handled
#ifdef DEBUG
  ESP_LOGD(TAG, "AT+CCLK? RESPONSE: %s", updateSerial().c_str());
#endif                              //DEBUG
  GsmSerial.println("AT+CMGD=1,4"); // Delete all Messages
#ifdef DEBUG
  ESP_LOGD(TAG, "AT+CMGDA=6 RESPONSE: %s", updateSerial().c_str());
#endif //DEBUG

  delAllSms();

  GsmSerial.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best

  checkQuality(updateSerial());
  
  //updateSerial();
}

String Gsm::updateSerial()
{
  /* uint64_t timeOld = millis();

  while (!GsmSerial.available() && !(millis() > timeOld + 5000))
  {
    delay(13);
  }

  String str;

  while (GsmSerial.available())
  {
    if (GsmSerial.available() > 0)
    {
      str += (char)GsmSerial.read();
    }
  }

  return str; */
  String receivedResponse = "";
  unsigned long startTime = millis();
  while (millis() - startTime < 5000)
  {
    if (GsmSerial.available())
    {
      receivedResponse = GsmSerial.readString();
      return receivedResponse;
    }
  }
  return receivedResponse;
}

String Gsm::updateSerial(uint64_t timeout)
{
  /*  String receivedResponse = "";
  unsigned long startTime = millis();
  while (millis() - startTime < 5000)
  {
    if (GsmSerial.available())
    {
      receivedResponse  += (char)GsmSerial.read();
      //return receivedResponse;
    }
  }
  return receivedResponse; */

  uint64_t timeOld = millis();

  while (!GsmSerial.available() && !(millis() > timeOld + timeout))
  {
    delay(13);
  }

  String str;

  while (GsmSerial.available())
  {
    if (GsmSerial.available() > 0)
    {
      str += (char)GsmSerial.read();
    }
  }

  return str;
}

int Gsm::checkQuality(String receivedResponse)
{
#ifdef DEBUG
  ESP_LOGD(TAG, "Received String Quality: %s", receivedResponse.c_str());
#endif //DEBUG
  if (receivedResponse.indexOf("+CSQ: ") != -1)
  {
    String quality = receivedResponse.substring(14, receivedResponse.indexOf(","));
    int qualityInt = quality.toInt();
#ifdef DEBUG
    ESP_LOGD(TAG, "Quality:  %d", qualityInt);
#endif //DEBUG
    return qualityInt;
  }
  return -1;
}

bool Gsm::checkOK(String receivedResponde)
{
  if (receivedResponde.indexOf("OK") != -1)
  {
    return true;
  }
  return false;
}

bool Gsm::dealSms(String receivedAT, String &number, String &message, String &date, String &hour)
{
  int index = receivedAT.indexOf("\"");
  int indexEnd = receivedAT.indexOf("\"", index + 1);
  String _number = receivedAT.substring(index + 1, indexEnd);
  number = hexToAscii(_number);
  date = receivedAT.substring(indexEnd + 6, indexEnd + 14);
  hour = receivedAT.substring(indexEnd + 15, indexEnd + 23);
  index = receivedAT.indexOf('\n', 2);
  String _message = receivedAT.substring(index + 1, receivedAT.length() - 2);
  String out = hexToAscii(_message);
  message = stringSpecialCharFormat(out);
}

bool Gsm::dealCall(String receivedAT, String &number, String &date, String &hour)
{
  number = receivedAT.substring(18, 27);
  date = "";
  hour = "";
}

String Gsm::hexToAscii(String hex)
{
  uint16_t len = hex.length();
  String ascii = "";
  for (uint16_t i = 2; i < len; i += 4)
  {
    ascii += (char)strtol(hex.substring(i, i + 2).c_str(), NULL, 16);
  }
  return ascii;
}

String Gsm::stringSpecialCharFormat(String inputStr)
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

void Gsm::hangUp()
{
  GsmSerial.println("ATH");
  delay(500);
}

String Gsm::send(String &atCommand)
{
  GsmSerial.println(atCommand); //Signal quality test, value range is 0-31 , 31 is the best
  return updateSerial();
}

void Gsm::getDateTime(int *day, int *month, int *year, int *hour, int *minute, int *second)
{
  GsmSerial.print(F("at+cclk?\r\n"));
  // if respond with ERROR try one more time.
  String buffer = updateSerial();
  if ((buffer.indexOf("ERR")) != -1)
  {
    delay(50);
    GsmSerial.print(F("at+cclk?\r\n"));
  }
  if ((buffer.indexOf("ERR")) == -1)
  {
    buffer = buffer.substring(buffer.indexOf("\"") + 1, buffer.lastIndexOf("\"") - 1);
    *year = buffer.substring(0, 2).toInt();
    *month = buffer.substring(3, 5).toInt();
    *day = buffer.substring(6, 8).toInt();
    *hour = buffer.substring(9, 11).toInt();
    *minute = buffer.substring(12, 14).toInt();
    *second = buffer.substring(15, 17).toInt();
  }
}

bool Gsm::checkSignalStrength()
{
  GsmSerial.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
  delay(2000);

  int signalStrengthInt = checkQuality(updateSerial());

  if (signalStrengthInt > 0)
  {
    _signalStrength = "{\"SignalStrength\":" + String(signalStrengthInt) + "}";

    return true;
  }
  return false;
}

String Gsm::getSignalStrength()
{
  return _signalStrength;
}

bool Gsm::isSms(String receivedMessage)
{
  if (receivedMessage.indexOf("+CMT:") != -1)
  {
    return true;
  }
  return false;
}

bool Gsm::isCall(String receivedCall)
{
  if (receivedCall.indexOf("+CLIP:") != -1)
  {
    hangUp();
    return true;
  }
}

uint8_t Gsm::received(String &number, String &message, String &date, String &hour)
{
  String receivedAT = updateSerial();
  if (receivedAT != "")
  {
#ifdef DEBUG
    ESP_LOGD(TAG, "Received AT: %s", receivedAT.c_str());
#endif //DEBUG
    if (isSms(receivedAT))
    {
      dealSms(receivedAT, number, message, date, hour);
      return 1;
    }
    else if (isCall(receivedAT))
    {
      dealCall(receivedAT, number, date, hour);

      return 2;
    }
  }
  return 0;
}

bool Gsm::delAllSms()
{
  // Can take up to 25 seconds

  GsmSerial.print(F("at+cmgda=\"del all\"\n\r"));
  String buffer = updateSerial(25000);
  if ((buffer.indexOf("ER")) != -1)
  {
#ifdef DEBUG
    ESP_LOGE(TAG, "Error Delete Messages: %s", buffer.c_str());
#endif //DEBUG
    return false;
  }
#ifdef DEBUG
  ESP_LOGD(TAG, "Received Response: %s", buffer.c_str());
#endif //DEBUG
  return true;
  // Error found, return 1
  // Error NOT found, return 0
}
