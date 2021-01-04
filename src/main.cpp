#include <Arduino.h>
// TTGO T-Call pin definitions
#define MODEM_RST 5
#define MODEM_PWKEY 4
#define MODEM_POWER_ON 23
#define MODEM_TX 27
#define MODEM_RX 26
#define I2C_SDA 21
#define I2C_SCL 22

#define SerialAT Serial1

String hexToAscii(String hex)
{
  uint16_t len = hex.length();
  String ascii = "";
  long a;
  for (uint16_t i = 2; i < len; i += 4)
  {
    ascii += (char)strtol(hex.substring(i, i + 2).c_str(), NULL, 16);
  }
 return ascii;
}

String stringSpecialCharFormat(String inputStr){
 String out;
 int c;
  for (size_t i = 0; i < inputStr.length(); i++)
  {
    c = (int)inputStr.charAt(i);
    Serial.println(c);
    if (c > 128)
    {
      switch (c)
      {
      case 193:
        out += "Á";
        break;
      case 194:
        out += "Â";
        break;
      case 195:
        out += "Ã";
        break;
      case 199:
        out += "Ç";
        break;
      case 200:
        out += "È";
        break;
      case 201:
        out += "É";
        break;
      case 204:
        out += "Ì";
        break;
      case 205:
        out += "Í";
        break;
      case 2010:
        out += "Ò";
        break;
      case 211:
        out += "Ó";
        break;
      case 212:
        out += "Ô";
        break;
      case 217:
        out += "Ù";
        break;
      case 218:
        out += "Ú";
        break;
      case 224:
        out += "à";
        break;
      case 225:
        out += "á";
        break;
      case 226:
        out += "â";
        break;
      case 227:
        out += "ã";
        break;
      case 231:
        out += "ç";
        break;
      case 232:
        out += "è";
        break;
      case 233:
        out += "é";
        break;
      case 234:
        out += "ê";
        break;
      case 236:
        out += "ì";
        break;
      case 237:
        out += "í";
        break;
      case 242:
        out += "ò";
        break;
      case 243:
        out += "ó";
        break;
      case 244:
        out += "ô";
        break;
      case 245:
        out += "õ";
        break;
      case 249:
        out += "ù";
        break;
      case 250:
        out += "ú";
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
    out.remove(out.length()-1);
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
void setup()
{
  // Set-up modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);

  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);
  Serial.begin(115200);
  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(1000);
  Serial.println("depois do begin");
  Serial.println("Handshake: ");
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

  /* SerialAT.println("AT+CSCS=\"UCS2\"");
  updateSerial();
  delay(1000);
  SerialAT.println("AT+CSMP=17,168,0,8");
  updateSerial();
  delay(1000); */

  SerialAT.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
  delay(1000);
  SerialAT.println("AT+CCLK? "); // Decides how newly arrived SMS messages should be handled
  updateSerial();

  delay(1000);
}

void loop()
{

  String str = "";
  while (SerialAT.available())
  {
    str = (char)SerialAT.read();
  }

  if (str != "")
  {
    Serial.print("STRING: ");
    Serial.println(str);

    if (str.indexOf("+CLIP:") != -1)
    {
      Serial.print("INDEX: ");
      Serial.println(str.indexOf("916235197"));
      String number = str.substring(18, 27);
      Serial.print("NUMBER: ");
      Serial.println(number);
      if (number == "916235197")
      {
        SerialAT.println("ATH");
        delay(5000);
        SerialAT.println("ATD+351916235197");
      }
    }

    if (str.indexOf("+CMT:") != -1)
    {

      int index = str.indexOf("\"");
      int indexEnd = str.indexOf("\"", index + 1);
      String number = str.substring(index + 1, indexEnd);
      number = hexToAscii(number);
      Serial.print("out number: ");
      Serial.println(number);
      index = str.indexOf('\n', 2);
      String message = str.substring(index + 1, str.length() - 2);
      String out = hexToAscii(message);
      message=stringSpecialCharFormat(out);
      Serial.print("out: ");
      Serial.println(message);
      String toCompare = "Portão de fora";
      toCompare.toUpperCase();
      Serial.print(toCompare);
      if (message == toCompare)
      {
        Serial.println("É igual");
      }
    }
    str = "";
  }
}
