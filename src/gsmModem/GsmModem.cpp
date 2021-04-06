#include "GsmModem.h"
#include <Arduino.h>




void gsmInit()
{
  
}




/* 
bool received(String &receivedStr)
{
  if (SerialAT.available())
  {
    receivedStr = "";
    while (SerialAT.available())
    {
      receivedStr += (char)SerialAT.read();
      delay(10);
    }
    return true;
  }
  return false;
}

void callReject()
{
  delay(1000);
  SerialAT.println("ATA");
  delay(500);
  SerialAT.println("ATH");
  delay(500);
} */