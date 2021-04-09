#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <FlashMemory.h>

String _configurationStr;

void FlashMemory::begin()
{
    if (!SPIFFS.begin())
    {
#ifdef DEBUG
        ESP_LOGE(TAG, "An error has occurred while mounting LittleFS");
#endif //DEBUG
        uint8_t spiffsRetries = 0;
        while (spiffsRetries < 3)
        {
            if (!SPIFFS.begin())
            {
                spiffsRetries++;
            }
            delay(500);
        }
        if (spiffsRetries == 4)
        {
            SPIFFS.format();
        }
    }
}

bool FlashMemory::loadConfiguration()
{
    if (SPIFFS.exists("/config"))
    {
        DynamicJsonDocument configurationJson(12288);
        File file = SPIFFS.open("/config", "r");
        DeserializationError error = deserializeJson(configurationJson, file);
        if (error)
        {
#ifdef DEBUG
            ESP_LOGE(TAG, "DeserializeJson() failed: %s", error.f_str());
#endif //DEBUG
            SPIFFS.format();
        }
        file.close();

        serializeJson(configurationJson, _configurationStr);
#ifdef DEBUG
        ESP_LOGD(TAG, "Json configuration doc: %s", _configurationStr.c_str());
#endif //DEBUG
        return true;
    }
    else
    {
        _configurationStr = "{}";
#ifdef DEBUG
        ESP_LOGD(TAG, "Configuration File not exist");
#endif //DEBUG
    }
    return false;
}

String FlashMemory::getConfiguration()
{

    return _configurationStr;
}

bool FlashMemory::setConfiguration(DynamicJsonDocument &receivedConfiguration)
{
    File file = SPIFFS.open("/config", "w");

    if (serializeJson(receivedConfiguration, file) > 0)
    {
        file.close();
        ESP_LOGD(TAG, "File save with success");
        return true;
    }
    file.close();
    ESP_LOGE(TAG, "Error to serialize");
    return false;
}

bool FlashMemory::loadConfiguration2Struct(uint8_t &usersCount, uint8_t &messagesCount, Users *users, Messages *messages)
{
    if (SPIFFS.exists("/config"))
    {
        DynamicJsonDocument configurationJson(12288);
        File file = SPIFFS.open("/config", "r");
        DeserializationError error = deserializeJson(configurationJson, file);
        if (error)
        {
#ifdef DEBUG
            ESP_LOGE(TAG, "DeserializeJson() failed: %s", error.f_str());
#endif //DEBUG
            SPIFFS.format();
        }
        file.close();
        usersCount = configurationJson["users"].size();
        for (size_t i = 0; i < usersCount; i++)
        {
            JsonObject user = configurationJson["users"][i];
            users[i].name = user["name"].as<String>();
            users[i].number = user["number"].as<String>();
        }

        messagesCount = configurationJson["messages"].size();
        for (size_t i = 0; i < messagesCount; i++)
        {
            JsonObject message = configurationJson["messages"][i];
            messages[i].message = message["message"].as<String>();
            messages[i].relay1 = message["relay1"];
            messages[i].relay2 = message["relay2"];
        }
    }
    return true;
}