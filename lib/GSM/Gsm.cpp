#include "Gsm.h"

const uint8_t SIM800_RST = 5;
const uint8_t SIM800_PWKEY = 4;
const uint8_t SIM800_POWER_ON = 23;
const uint8_t SIM800_TX = 27;
const uint8_t SIM800_RX = 26;
const uint8_t SIM800_I2C_SDA = 21;
const uint8_t SIM800_I2C_SCL = 22;

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
    ESP_LOGD(TAG, " AT+CCID RESPONSE: %s", receivedMessage.c_str());
    if (!checkOK(receivedMessage))
    {
        ESP_LOGE(TAG, "ERROR AT COMMAND CCID, %s", receivedMessage.c_str());
    }

    GsmSerial.println("AT+CREG?"); //Check whether it has registered in the network
    delay(1000);
    receivedMessage = updateSerial();

    ESP_LOGD(TAG, " AT+CREG? RESPONSE: %s", updateSerial().c_str());

    GsmSerial.println("AT+CMGF=1"); // Configuring TEXT mode
    delay(1000);
    ESP_LOGD(TAG, "AT+CMGF=1 RESPONSE: %s", updateSerial().c_str());

    GsmSerial.println("AT+CSCS=\"UCS2\"");
    delay(1000);
    ESP_LOGD(TAG, "AT+CSCS=\"UCS2\" RESPONSE: %s", updateSerial().c_str());

    GsmSerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
    delay(1000);
    ESP_LOGD(TAG, "AT+CNMI=1,2,0,0,0 RESPONSE: %s", updateSerial().c_str());

    GsmSerial.println("AT+CCLK? "); // Decides how newly arrived SMS messages should be handled
    delay(1000);
    ESP_LOGD(TAG, "AT+CCLK? RESPONSE: %s", updateSerial().c_str());

    GsmSerial.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
    delay(2000);
    checkQuality(updateSerial());
    //updateSerial();
}


String Gsm::updateSerial()
{
    String receivedResponse = "";
    if (GsmSerial.available())
    {
        receivedResponse = GsmSerial.readString();
    }
    return receivedResponse;
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