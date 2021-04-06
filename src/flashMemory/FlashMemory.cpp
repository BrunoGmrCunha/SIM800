#include "FlashMemory.h"
#include "SPIFFS.h"
#include "ArduinoJson.h"

#include "Arduino.h"
#include "constants.h"
DynamicJsonDocument _jsonConfigDoc(12288);
String _jsonConfig = "{\"users\":[],\"messages\":[]}";
DynamicJsonDocument _jsonHistoryDoc(12288);
String _jsonHistory = "{\"history\":[]}";

void flashMemorySetup(uint8_t &usersCount, uint8_t &messagesCount, Users *users, Messages *messages)
{
    if (!SPIFFS.begin())
    {
        ESP_LOGE(TAG, "An error has occurred while mounting LittleFS");
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

#ifdef DEBUG
    ESP_LOGD(TAG, "FS mounted successfully");
#endif //DEBUG

    if (SPIFFS.exists("/config"))
    {
        File file = SPIFFS.open("/config", "r");
        _jsonConfigDoc.clear();
        DeserializationError error = deserializeJson(_jsonConfigDoc, file);
        if (error)
        {
#ifdef DEBUG
            ESP_LOGE(TAG, "deserializeJson() failed: %s", error.f_str());
#endif //DEBUG
            SPIFFS.format();
        }
        loadConfiguration2Struct(usersCount, messagesCount, users, messages);
        file.close();

#ifdef DEBUG
        String message;
        serializeJsonPretty(_jsonConfigDoc, message);
        ESP_LOGD(TAG, "Json configuration doc: %s", message.c_str());
#endif //DEBUG
    }
}

bool loadConfiguration2Struct(uint8_t &usersCount, uint8_t &messagesCount,Users *users, Messages *messages)
{
    usersCount = _jsonConfigDoc["users"].size();
    for (size_t i = 0; i < usersCount; i++)
    {
        JsonObject user = _jsonConfigDoc["users"][i];
        users[i].name = user["name"].as<String>();
        users[i].number = user["number"].as<String>();
    }
    messagesCount = _jsonConfigDoc["messages"].size();
    for (size_t i = 0; i < messagesCount; i++)
    {
        JsonObject message = _jsonConfigDoc["messages"][i];
        messages[i].message = message["message"].as<String>();
        messages[i].relay1 = message["relay1"];
        messages[i].relay2 = message["relay2"];
    }
    _jsonConfig = "";
    serializeJson(_jsonConfigDoc, _jsonConfig);

#ifdef DEBUG
    String message;
    serializeJsonPretty(_jsonConfigDoc, message);
    ESP_LOGD(TAG, "Received configuration: %s", message.c_str());
#endif //DEBUG
}

String getJsonConfig()
{
    return _jsonConfig;
}

void writeHistory(History &history)
{
    JsonArray historyArray;
    if (SPIFFS.exists("/history"))
    {
        File fileRead = SPIFFS.open("/history", "r");

        DeserializationError error = deserializeJson(_jsonHistoryDoc, fileRead);

        // Test if parsing succeeds.
        if (error)
        {
            ESP_LOGE(TAG, "deserializeJson() failed: %s", error.f_str());
            return;
        }
        fileRead.close();
        historyArray = _jsonHistoryDoc["history"].as<JsonArray>();
    }
    else
    {
        _jsonHistoryDoc.clear();
        historyArray = _jsonHistoryDoc.createNestedArray("history");
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
    serializeJson(_jsonHistoryDoc, fileWrite);
    fileWrite.close();
}

String readHistory()
{
    if (SPIFFS.exists("/history"))
    {
        File fileRead = SPIFFS.open("/history", "r");

        DeserializationError error = deserializeJson(_jsonHistoryDoc, fileRead);
        if (error)
        {
            ESP_LOGE(TAG, "deserializeJson() failed: %s", error.f_str());
            SPIFFS.remove(("/history"));
            delay(2000);
            ESP.restart();
        }
        fileRead.close();
        JsonArray historyArray = _jsonHistoryDoc["history"].as<JsonArray>();
        _jsonHistoryDoc.clear();
        JsonArray orderArray = _jsonHistoryDoc.createNestedArray("history");
        int ArrayLength = historyArray.size();

        for (int i = 0; i < ArrayLength; i++)
        {
            orderArray[i] = historyArray[ArrayLength - 1 - i].as<JsonObject>();
            orderArray[ArrayLength - 1 - i] = historyArray[i].as<JsonObject>();
        }
        serializeJson(_jsonHistoryDoc, _jsonHistory);
    }
    else
    {
        _jsonHistory = "";
    }
    return _jsonHistory;
}

void setJsonConfig(DynamicJsonDocument &receivedJsonDocumment)
{
    _jsonConfigDoc = receivedJsonDocumment;
}

bool saveConfiguration()
{
    File file = SPIFFS.open("/config", "w");
    serializeJson(_jsonConfigDoc, file);
    file.close();

        //loadConfiguration2Struct(usersCount, messagesCount, users, messages);
}
