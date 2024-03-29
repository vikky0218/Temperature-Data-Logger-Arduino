#include <DHT.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <SdFat.h>

#define DHTPIN A9
#define DHTPIN1 A11
#define DHTPIN2 A13
#define DHTPIN3 A15
#define DHTTYPE DHT11
#define DHTTYPE1 DHT11
#define DHTTYPE2 DHT11
#define DHTTYPE3 DHT11

DHT dht(DHTPIN, DHTTYPE);
DHT dht1(DHTPIN1, DHTTYPE1);
DHT dht2(DHTPIN2, DHTTYPE2);
DHT dht3(DHTPIN3, DHTTYPE3);
RTC_DS3231 rtc;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const int r1 = 22;
const int r2 = 23;
const int r3 = 24;
const int r4 = 25;
const int r5 = 26;
const int r6 = 27;
const int r7 = 28;
const int r8 = 29;

SdFat SD;
File dataFile;
const int maxStringLength = 256;

String head[20];
String body[50];
String deviceName = "Logger";
const String password = "1234";
String logType = "Temperature";
String NoOfSensor = "1";
String rType = "C";
bool unit = false;
String T1SV = "0";
String H1SV = "0";
String T1UL = "0";
String T1LL = "0";
String H1UL = "0";
String H1LL = "0";
String T1C = "0";
String H1C = "0";
String T2SV = "0";
String H2SV = "0";
String T2UL = "0";
String T2LL = "0";
String H2UL = "0";
String H2LL = "0";
String T2C = "0";
String H2C = "0";
String T3SV = "0";
String H3SV = "0";
String T3UL = "0";
String T3LL = "0";
String H3UL = "0";
String H3LL = "0";
String T3C = "0";
String H3C = "0";
String T4SV = "0";
String H4SV = "0";
String T4UL = "0";
String T4LL = "0";
String H4UL = "0";
String H4LL = "0";
String T4C = "0";
String H4C = "0";
int interval = 10000;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  if (NoOfSensor.equals("1")) {
    dht.begin();
    pinMode(r1, OUTPUT);
    pinMode(r2, OUTPUT);
    digitalWrite(r1, HIGH);
    digitalWrite(r2, HIGH);
  }
  if (NoOfSensor.equals("2")) {
    dht1.begin();
    pinMode(r1, OUTPUT);
    pinMode(r2, OUTPUT);
    pinMode(r3, OUTPUT);
    pinMode(r4, OUTPUT);

    digitalWrite(r1, HIGH);
    digitalWrite(r2, HIGH);
    digitalWrite(r3, HIGH);
    digitalWrite(r4, HIGH);
  }
  if (NoOfSensor.equals("3")) {
    dht2.begin();
    pinMode(r1, OUTPUT);
    pinMode(r2, OUTPUT);
    pinMode(r3, OUTPUT);
    pinMode(r4, OUTPUT);
    pinMode(r5, OUTPUT);
    pinMode(r6, OUTPUT);

    digitalWrite(r1, HIGH);
    digitalWrite(r2, HIGH);
    digitalWrite(r3, HIGH);
    digitalWrite(r4, HIGH);
    digitalWrite(r5, HIGH);
    digitalWrite(r6, HIGH);
  }
  if (NoOfSensor.equals("4")) {
    dht3.begin();
    pinMode(r1, OUTPUT);
    pinMode(r2, OUTPUT);
    pinMode(r3, OUTPUT);
    pinMode(r4, OUTPUT);
    pinMode(r5, OUTPUT);
    pinMode(r6, OUTPUT);
    pinMode(r7, OUTPUT);
    pinMode(r8, OUTPUT);

    digitalWrite(r1, HIGH);
    digitalWrite(r2, HIGH);
    digitalWrite(r3, HIGH);
    digitalWrite(r4, HIGH);
    digitalWrite(r5, HIGH);
    digitalWrite(r6, HIGH);
    digitalWrite(r7, HIGH);
    digitalWrite(r8, HIGH);
  }
  while (!SD.begin(53)) {
    Serial.println(deviceName + "_Error_SDFail");
    lcd.setCursor(0, 0);
    lcd.print("SD Fail");
    delay(3000);
    return;
  }
  while (!rtc.begin()) {
    Serial.println(deviceName + "_Error_RTC_Missing");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RTC Error");
    delay(5000);
    return;
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  readConfigFromSD();
}

void loop() {
  float T1 = dht.readTemperature();
  float T2 = dht1.readTemperature();
  float T3 = dht2.readTemperature();
  float T4 = dht3.readTemperature();
  if (unit) { rType = "F"; }
  if (unit) { T1 = Fahrenheit(T1); }
  if (unit) { T2 = Fahrenheit(T2); }
  if (unit) { T3 = Fahrenheit(T3); }
  if (unit) { T4 = Fahrenheit(T4); }
  int H1 = dht.readHumidity();
  int H2 = dht1.readHumidity();
  int H3 = dht2.readHumidity();
  int H4 = dht3.readHumidity();

  DateTime now = rtc.now();
  int year = now.year();
  int month = now.month();
  int day = now.day();
  int hour = now.hour();
  int minute = now.minute();
  int second = now.second();
  String time = String(hour) + ":" + String(minute) + ":" + String(second);
  String date = String(year) + "/" + String(month) + "/" + String(day);
  bool temp1 = false;
  bool temp2 = false;
  bool temp3 = false;
  bool temp4 = false;
  bool humi1 = false;
  bool humi2 = false;
  bool humi3 = false;
  bool humi4 = false;

  if (T1C.toFloat() != 0 || H1C.toInt() != 0 && NoOfSensor.toInt() >= 1) {
    T1 = T1 + T1C.toFloat();
    H1 = H1 + H1C.toInt();
  }
  if (T2C.toFloat() != 0 || H2C.toInt() != 0 && NoOfSensor.toInt() >= 2) {
    T2 = T2 + T2C.toFloat();
    H2 = H2 + H2C.toInt();
  }
  if (T3C.toFloat() != 0 || H3C.toInt() != 0 && NoOfSensor.toInt() >= 3) {
    T3 = T3 + T3C.toFloat();
    H3 = H3 + H3C.toInt();
  }
  if (T4C.toFloat() != 0 || H4C.toInt() != 0 && NoOfSensor.toInt() == 4) {
    T4 = T4 + T4C.toFloat();
    H4 = H4 + H4C.toInt();
  }
  for (int i = 0; i < 20; i++) {
    head[i] = "";
  }
  for (int i = 0; i < 45; i++) {
    body[i] = "";
  }
  if (Serial.available() > 0) {
    String inputLine = Serial.readStringUntil('\n');
    int headCount = splitString(inputLine, '_', head);
    String name = head[0];
    if (name.equals(deviceName)) {
      String cmd = head[1];
      if (cmd.equals("ChangeName")) {
        splitString(head[headCount - 1], ' ', body);
        String pass = body[0];
        if (pass.equals(password)) {
          deviceName = body[1];
        } else {
          Serial.println(deviceName + "Error_PassIncorrect");
        }
      } else if (cmd.equals("Config")) {
        splitString(head[headCount - 1], ' ', body);
        logType = body[0];
        NoOfSensor = body[1];
        String fahr = body[2];
        if (fahr.equals("F")) {
          unit = true;
        } else {
          unit = false;
        }
        if (logType.equals("Temperature")) {
          if (NoOfSensor.equals("1")) {
            T1SV = body[3];
            T1UL = body[4];
            T1LL = body[5];
            T1C = body[6];
            interval = body[7].toInt();
            Serial.println(deviceName + "_DONE");
          } else if (NoOfSensor.equals("2")) {
            T1SV = body[3];
            T1UL = body[4];
            T1LL = body[5];
            T1C = body[6];
            T2SV = body[7];
            T2UL = body[8];
            T2LL = body[9];
            T2C = body[10];
            interval = body[11].toInt();
            Serial.println(deviceName + "_DONE");
          } else if (NoOfSensor.equals("3")) {
            T1SV = body[3];
            T1UL = body[4];
            T1LL = body[5];
            T1C = body[6];
            T2SV = body[7];
            T2UL = body[8];
            T2LL = body[9];
            T2C = body[10];
            T3SV = body[11];
            T3UL = body[12];
            T3LL = body[13];
            T3C = body[14];
            interval = body[15].toInt();
            Serial.println(deviceName + "_DONE");
          } else if (NoOfSensor.equals("4")) {
            T1SV = body[3];
            T1UL = body[4];
            T1LL = body[5];
            T1C = body[6];
            T2SV = body[7];
            T2UL = body[8];
            T2LL = body[9];
            T2C = body[10];
            T3SV = body[11];
            T3UL = body[12];
            T3LL = body[13];
            T3C = body[14];
            T4SV = body[15];
            T4UL = body[16];
            T4LL = body[17];
            T4C = body[18];
            interval = body[19].toInt();
            Serial.println(deviceName + "_DONE");
          }
        } else if (logType.equals("Humidity")) {
          if (NoOfSensor.equals("1")) {
            H1SV = body[2];
            H1UL = body[3];
            H1LL = body[4];
            H1C = body[5];
            interval = body[6].toInt();
            Serial.println(deviceName + "_DONE");
          } else if (NoOfSensor.equals("2")) {
            H1SV = body[2];
            H1UL = body[3];
            H1LL = body[4];
            H1C = body[5];
            H2SV = body[6];
            H2UL = body[7];
            H2LL = body[8];
            H2C = body[9];
            interval = body[10].toInt();
            Serial.println(deviceName + "_DONE");
          } else if (NoOfSensor.equals("3")) {
            H1SV = body[2];
            H1UL = body[3];
            H1LL = body[4];
            H1C = body[5];
            H2SV = body[6];
            H2UL = body[7];
            H2LL = body[8];
            H2C = body[9];
            H3SV = body[10];
            H3UL = body[11];
            H3LL = body[12];
            H3C = body[13];
            interval = body[14].toInt();
            Serial.println(deviceName + "_DONE");
          } else if (NoOfSensor.equals("4")) {
            H1SV = body[2];
            H1UL = body[3];
            H1LL = body[4];
            H1C = body[5];
            H2SV = body[6];
            H2UL = body[7];
            H2LL = body[8];
            H2C = body[9];
            H3SV = body[10];
            H3UL = body[11];
            H3LL = body[12];
            H3C = body[13];
            H4SV = body[14];
            H4UL = body[15];
            H4LL = body[16];
            H4C = body[17];
            interval = body[18].toInt();
            Serial.println(deviceName + "_DONE");
          }
        } else if (logType.equals("Both")) {
          if (NoOfSensor.equals("1")) {
            T1SV = body[3];
            T1UL = body[4];
            T1LL = body[5];
            T1C = body[6];
            H1SV = body[7];
            H1UL = body[8];
            H1LL = body[9];
            H1C = body[10];
            interval = body[11].toInt();
            Serial.println(deviceName + "_DONE");
          } else if (NoOfSensor.equals("2")) {
            T1SV = body[3];
            T1UL = body[4];
            T1LL = body[5];
            T1C = body[6];
            T2SV = body[7];
            T2UL = body[8];
            T2LL = body[9];
            T2C = body[10];
            H1SV = body[11];
            H1UL = body[12];
            H1LL = body[13];
            H1C = body[14];
            H2SV = body[15];
            H2UL = body[16];
            H2LL = body[17];
            H2C = body[18];
            interval = body[19].toInt();
            Serial.println(deviceName + "_DONE");
          } else if (NoOfSensor.equals("3")) {
            T1SV = body[3];
            T1UL = body[4];
            T1LL = body[5];
            T1C = body[6];
            T2SV = body[7];
            T2UL = body[8];
            T2LL = body[9];
            T2C = body[10];
            T3SV = body[11];
            T3UL = body[12];
            T3LL = body[13];
            T3C = body[14];
            H1SV = body[15];
            H1UL = body[16];
            H1LL = body[17];
            H1C = body[18];
            H2SV = body[19];
            H2UL = body[20];
            H2LL = body[21];
            H2C = body[22];
            H3SV = body[23];
            H3UL = body[24];
            H3LL = body[25];
            H3C = body[26];
            interval = body[27].toInt();
            Serial.println(deviceName + "_DONE");
          } else if (NoOfSensor.equals("4")) {
            T1SV = body[3];
            T1UL = body[4];
            T1LL = body[5];
            T1C = body[6];
            T2SV = body[7];
            T2UL = body[8];
            T2LL = body[9];
            T2C = body[10];
            T3SV = body[11];
            T3UL = body[12];
            T3LL = body[13];
            T3C = body[14];
            T4SV = body[15];
            T4UL = body[16];
            T4LL = body[17];
            T4C = body[18];
            H1SV = body[19];
            H1UL = body[20];
            H1LL = body[21];
            H1C = body[22];
            H2SV = body[23];
            H2UL = body[24];
            H2LL = body[25];
            H2C = body[26];
            H3SV = body[27];
            H3UL = body[28];
            H3LL = body[29];
            H3C = body[30];
            H4SV = body[31];
            H4UL = body[32];
            H4LL = body[33];
            H4C = body[34];
            interval = body[35].toInt();
            Serial.println(deviceName + "_DONE");
          }
        }
      } else if (cmd.equals("Error")) {
        dataFile = SD.open("ErrorLog.txt", FILE_WRITE);
        if (dataFile) {
          dataFile.print(inputLine);
          dataFile.print("_");
          dataFile.print(time);
          dataFile.print("_");
          dataFile.print(date);
          dataFile.println();
          dataFile.close();
        } else {
          Serial.println(deviceName + "_Error_OpenLog");
        }
      } else if (cmd.equals("WARN")) {
        dataFile = SD.open("Warning.txt", FILE_WRITE);
        if (dataFile) {
          dataFile.print(inputLine);
          dataFile.print("_");
          dataFile.print(time);
          dataFile.print("_");
          dataFile.print(date);
          dataFile.println();
          dataFile.close();
        } else {
          Serial.println(deviceName + "_Error_OpenWarn");
        }
      } else if (cmd.equals("ATHARVA")) {
        if (logType.equals("Temperature")) {
          if (NoOfSensor.equals("1")) {
            dataFile = SD.open("temp1.csv", FILE_READ);
            if (dataFile) {
              Serial.println(deviceName + "_DOWN");
              while (dataFile.available()) {
                Serial.write(dataFile.read());
              }
              Serial.println("EndOfFileMarker");
              dataFile.close();
            }
          } else if (NoOfSensor.equals("2")) {
            dataFile = SD.open("temp2.csv", FILE_READ);
            if (dataFile) {
              Serial.println(deviceName + "_DOWN");
              while (dataFile.available()) {
                Serial.write(dataFile.read());
              }
              Serial.println("EndOfFileMarker");
              dataFile.close();
            }
          } else if (NoOfSensor.equals("3")) {
            dataFile = SD.open("temp3.csv", FILE_READ);
            if (dataFile) {
              Serial.println(deviceName + "_DOWN");
              while (dataFile.available()) {
                Serial.write(dataFile.read());
              }
              Serial.println("EndOfFileMarker");
              dataFile.close();
            }
          } else if (NoOfSensor.equals("4")) {
            dataFile = SD.open("temp4.csv", FILE_READ);
            if (dataFile) {
              Serial.println(deviceName + "_DOWN");
              while (dataFile.available()) {
                Serial.write(dataFile.read());
              }
              Serial.println("EndOfFileMarker");
              dataFile.close();
            }
          }
        } else if (logType.equals("Humidity")) {
          if (NoOfSensor.equals("1")) {
            dataFile = SD.open("humi1.csv", FILE_READ);
            if (dataFile) {
              Serial.println(deviceName + "_DOWN");
              while (dataFile.available()) {
                Serial.write(dataFile.read());
              }
              Serial.println("EndOfFileMarker");
              dataFile.close();
            }
          } else if (NoOfSensor.equals("2")) {
            dataFile = SD.open("humi2.csv", FILE_READ);
            if (dataFile) {
              Serial.println(deviceName + "_DOWN");
              while (dataFile.available()) {
                Serial.write(dataFile.read());
              }
              Serial.println("EndOfFileMarker");
              dataFile.close();
            }
          } else if (NoOfSensor.equals("3")) {
            dataFile = SD.open("humi3.csv", FILE_READ);
            if (dataFile) {
              Serial.println(deviceName + "_DOWN");
              while (dataFile.available()) {
                Serial.write(dataFile.read());
              }
              Serial.println("EndOfFileMarker");
              dataFile.close();
            }
          } else if (NoOfSensor.equals("4")) {
            dataFile = SD.open("humi4.csv", FILE_READ);
            if (dataFile) {
              Serial.println(deviceName + "_DOWN");
              while (dataFile.available()) {
                Serial.write(dataFile.read());
              }
              Serial.println("EndOfFileMarker");
              dataFile.close();
            }
          }
        } else if (logType.equals("Both")) {
          if (NoOfSensor.equals("1")) {

            dataFile = SD.open("both1.csv", FILE_READ);
            if (dataFile) {
              Serial.println(deviceName + "_DOWN");
              while (dataFile.available()) {
                Serial.write(dataFile.read());
              }
              Serial.println("EndOfFileMarker");
              dataFile.close();
            }
          } else if (NoOfSensor.equals("2")) {
            dataFile = SD.open("both2.csv", FILE_READ);
            if (dataFile) {
              Serial.println(deviceName + "_DOWN");
              while (dataFile.available()) {
                Serial.write(dataFile.read());
              }
              Serial.println("EndOfFileMarker");
              dataFile.close();
            }
          } else if (NoOfSensor.equals("3")) {
            dataFile = SD.open("both3.csv", FILE_READ);
            if (dataFile) {
              Serial.println(deviceName + "_DOWN");
              while (dataFile.available()) {
                Serial.write(dataFile.read());
              }
              Serial.println("EndOfFileMarker");
              dataFile.close();
            }
          } else if (NoOfSensor.equals("4")) {
            dataFile = SD.open("both4.csv", FILE_READ);
            if (dataFile) {
              Serial.println(deviceName + "_DOWN");
              while (dataFile.available()) {
                Serial.write(dataFile.read());
              }
              Serial.println("EndOfFileMarker");
              dataFile.close();
            }
          }
        }
      } else if (cmd.equals("DATHARVAD")) {
        if (logType.equals("Temperature")) {
          if (NoOfSensor.equals("1")) {
            dataFile = SD.open("temp1.csv", FILE_WRITE);
            if (dataFile) {
              dataFile.truncate(0);
              dataFile.close();
            }
          } else if (NoOfSensor.equals("2")) {
            dataFile = SD.open("temp2.csv", FILE_WRITE);
            if (dataFile) {
              dataFile.truncate(0);
              dataFile.close();
            }
          } else if (NoOfSensor.equals("3")) {
            dataFile = SD.open("temp3.csv", FILE_WRITE);
            if (dataFile) {
              dataFile.truncate(0);
              dataFile.close();
            }
          } else if (NoOfSensor.equals("4")) {
            dataFile = SD.open("temp4.csv", FILE_WRITE);
            if (dataFile) {
              dataFile.truncate(0);
              dataFile.close();
            }
          }
        } else if (logType.equals("Humidity")) {
          if (NoOfSensor.equals("1")) {
            dataFile = SD.open("humi1.csv", FILE_WRITE);
            if (dataFile) {
              dataFile.truncate(0);
              dataFile.close();
            }
          } else if (NoOfSensor.equals("2")) {
            dataFile = SD.open("humi2.csv", FILE_WRITE);
            if (dataFile) {
            }
            dataFile.truncate(0);
            dataFile.close();
          } else if (NoOfSensor.equals("3")) {
            dataFile = SD.open("humi3.csv", FILE_WRITE);
            if (dataFile) {
              dataFile.truncate(0);
              dataFile.close();
            }
          } else if (NoOfSensor.equals("4")) {
            dataFile = SD.open("humi4.csv", FILE_WRITE);
            if (dataFile) {
              dataFile.truncate(0);
              dataFile.close();
            }
          }
        } else if (logType.equals("Both")) {
          if (NoOfSensor.equals("1")) {
            dataFile = SD.open("both1.csv", FILE_WRITE);
            if (dataFile) {
              dataFile.truncate(0);
              dataFile.close();
            }
          } else if (NoOfSensor.equals("2")) {
            dataFile = SD.open("both2.csv", FILE_WRITE);
            if (dataFile) {
              dataFile.truncate(0);
              dataFile.close();
            }
          } else if (NoOfSensor.equals("3")) {
            dataFile = SD.open("both3.csv", FILE_WRITE);
            if (dataFile) {
              dataFile.truncate(0);
              dataFile.close();
            }
          } else if (NoOfSensor.equals("4")) {
            dataFile = SD.open("both4.csv", FILE_WRITE);
            if (dataFile) {
              dataFile.truncate(0);
              dataFile.close();
            }
          }
        }
      }
      writeConfigToSD();
    }
  }
  logType = trim(logType);

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (logType.equals("Temperature")) {
      NoOfSensor = trim(NoOfSensor);
      if (NoOfSensor.equals("1")) {
        if (isnan(T1)) {
          Serial.println(deviceName + "_Error_S1_Fail");
          temp1 = true;
        }
        Serial.print(deviceName);
        Serial.print("_VALUES_" + rType + "_");
        if (temp1) {
          Serial.print("NA");
        } else {
          Serial.print(T1, 2);
        }
        dateTime();
        Serial.println();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(deviceName);
        lcd.setCursor(11, 0);
        if (hour < 10) {
          lcd.print(0 + hour, DEC);
        } else {
          lcd.print(hour, DEC);
        }
        lcd.print(':');
        if (minute < 10) {
          lcd.print(0 + minute, DEC);
        } else {
          lcd.print(minute, DEC);
        }
        lcd.setCursor(0, 1);
        lcd.print("T:");
        if (temp1) {
          lcd.print("NA");
        } else {
          lcd.print(T1, 2);
        }
        lcd.setCursor(7, 1);
        if (temp1) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(11, 1);
        if (day < 10) {
          lcd.print(0 + day, DEC);
        } else {
          lcd.print(day, DEC);
        }
        lcd.print('/');
        if (month < 10) {
          lcd.print(0 + month, DEC);
        } else {
          lcd.print(month, DEC);
        }
        if (!temp1 && T1LL.toFloat() != 0 && T1UL.toFloat() != 0) {
          if (T1 > T1UL.toFloat()) {
            digitalWrite(r1, LOW);
            Serial.println(deviceName + "_WARN_T1_High_SV_" + T1SV + "_PV_" + T1);
            lcd.setCursor(8, 1);
            lcd.print("HT");
          } else if (T1 < T1LL.toFloat()) {
            digitalWrite(r2, LOW);
            Serial.println(deviceName + "_WARN_T1_Low_SV_" + T1SV + "_PV_" + T1);
            lcd.setCursor(8, 1);
            lcd.print("LT");
          } else if (T1 == T1SV.toFloat() || T1 < T1UL.toFloat() && T1 > T1LL.toFloat()) {
            digitalWrite(r1, HIGH);
            digitalWrite(r2, HIGH);
            lcd.setCursor(8, 1);
            lcd.print("  ");
          }
        }
        dataFile = SD.open("temp1.csv", FILE_WRITE);
        if (dataFile) {
          dataFile.print(deviceName);
          dataFile.print(", ");
          dataFile.print(T1);
          dataFile.print(", ");
          dataFile.print(rType);
          dataFile.print(", ");
          dataFile.print(time);
          dataFile.print(", ");
          dataFile.print(date);
          dataFile.println();
          dataFile.close();
        } else {
          Serial.println(deviceName + "_Error_OpenFileT1");
        }
      } else if (NoOfSensor.equals("2")) {
        if (isnan(T1)) {
          Serial.println(deviceName + "_Error_S1_Fail");
          temp1 = true;
        }
        if (isnan(T2)) {
          Serial.println(deviceName + "_Error_S2_Fail");
          temp2 = true;
        }
        Serial.print(deviceName);
        Serial.print("_VALUES_" + rType + "_");
        if (temp1) {
          Serial.print("NA");
        } else {
          Serial.print(T1, 2);
        }
        Serial.print(" ");
        if (temp2) {
          Serial.print("NA");
        } else {
          Serial.print(T2, 2);
        }
        dateTime();
        Serial.println();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T1:");
        if (temp1) {
          lcd.print("NA");
        } else {
          lcd.print(T1, 2);
        }
        lcd.setCursor(8, 0);
        if (temp1) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(11, 0);
        if (hour < 10) {
          lcd.print(0 + hour, DEC);
        } else {
          lcd.print(hour, DEC);
        }
        lcd.print(':');
        if (minute < 10) {
          lcd.print(0 + minute, DEC);
        } else {
          lcd.print(minute, DEC);
        }
        lcd.setCursor(0, 1);
        lcd.print("T2:");
        if (temp2) {
          lcd.print("NA");
        } else {
          lcd.print(T2, 2);
        }
        lcd.setCursor(8, 1);
        if (temp2) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(11, 1);
        if (day < 10) {
          lcd.print(0 + day, DEC);
        } else {
          lcd.print(day, DEC);
        }
        lcd.print('/');
        if (month < 10) {
          lcd.print(0 + month, DEC);
        } else {
          lcd.print(month, DEC);
        }

        if (!temp1 && T1LL.toFloat() != 0 && T1UL.toFloat() != 0) {
          if (T1 > T1UL.toFloat()) {
            digitalWrite(r1, LOW);
            Serial.println(deviceName + "_WARN_T1_High_SV_" + T1SV + "_PV_" + T1);
            lcd.setCursor(9, 0);
            lcd.print("H");
          } else if (T1 < T1LL.toFloat()) {
            digitalWrite(r2, LOW);
            Serial.println(deviceName + "_WARN_T1_Low_SV_" + T1SV + "_PV_" + T1);
            lcd.setCursor(9, 0);
            lcd.print("L");
          } else if (T1 == T1SV.toFloat() || T1 < T1UL.toFloat() && T1 > T1LL.toFloat()) {
            digitalWrite(r1, HIGH);
            digitalWrite(r2, HIGH);
            lcd.setCursor(9, 0);
            lcd.print("  ");
          }
        }
        if (!temp2 && T2LL.toFloat() != 0 && T2UL.toFloat() != 0) {
          if (T2 > T2UL.toFloat()) {
            digitalWrite(r3, LOW);
            Serial.println(deviceName + "_WARN_T2_High_SV_" + T2SV + "_PV_" + T2);
            lcd.setCursor(9, 1);
            lcd.print("H");
          } else if (T2 < T2LL.toFloat()) {
            digitalWrite(r4, LOW);
            Serial.println(deviceName + "_WARN_T2_Low_SV_" + T2SV + "_PV_" + T2);
            lcd.setCursor(9, 1);
            lcd.print("L");
          } else if (T2 == T2SV.toFloat() || T2 < T2UL.toFloat() && T2 > T2LL.toFloat()) {
            digitalWrite(r3, HIGH);
            digitalWrite(r4, HIGH);
            lcd.setCursor(9, 1);
            lcd.print(" ");
          }
        }
        dataFile = SD.open("temp2.csv", FILE_WRITE);
        if (dataFile) {
          dataFile.print(deviceName);
          dataFile.print(", ");
          dataFile.print(T1);
          dataFile.print(", ");
          dataFile.print(T2);
          dataFile.print(", ");
          dataFile.print(rType);
          dataFile.print(", ");
          dataFile.print(time);
          dataFile.print(", ");
          dataFile.print(date);
          dataFile.close();
        } else {
          Serial.println(deviceName + "_Error_OpenFileT2");
        }
      } else if (NoOfSensor.equals("3")) {
        if (isnan(T1)) {
          Serial.println(deviceName + "_Error_S1_Fail");
          temp1 = true;
        }
        if (isnan(T2)) {
          Serial.println(deviceName + "_Error_S2_Fail");
          temp2 = true;
        }
        if (isnan(T3)) {
          Serial.println(deviceName + "_Error_S3_Fail");
          temp3 = true;
        }
        Serial.print(deviceName);
        Serial.print("_VALUES_" + rType + "_");
        Serial.print("T1: ");
        if (temp1) {
          Serial.print("NA");
        } else {
          Serial.print(T1, 2);
        }
        Serial.print(" T2: ");
        if (temp2) {
          Serial.print("NA");
        } else {
          Serial.print(T2, 2);
        }
        Serial.print(" T3: ");
        if (temp3) {
          Serial.print("NA");
        } else {
          Serial.print(T3, 2);
        }
        dateTime();
        Serial.println();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T1   T2   T3");
        lcd.setCursor(0, 1);
        if (temp1) {
          lcd.print("NA");
        } else {
          lcd.print(T1);
        }
        lcd.setCursor(2, 1);
        if (temp1) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(3, 1);
        lcd.print("  ");
        lcd.setCursor(5, 1);
        if (temp2) {
          lcd.print("NA");
        } else {
          lcd.print(T2);
        }
        lcd.setCursor(7, 1);
        if (temp2) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(8, 1);
        lcd.print("  ");
        lcd.setCursor(10, 1);
        if (temp3) {
          lcd.print("NA");
        } else {
          lcd.print(T3);
        }
        lcd.setCursor(12, 1);
        if (temp3) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(13, 1);
        lcd.print("   ");

        if (!temp1 && T1LL.toFloat() != 0 && T1UL.toFloat() != 0) {
          if (T1 > T1UL.toFloat()) {
            digitalWrite(r1, LOW);
            Serial.println(deviceName + "_WARN_T1_High_SV_" + T1SV + "_PV_" + T1);
            lcd.setCursor(3, 1);
            lcd.print("H");
          } else if (T1 < T1LL.toFloat()) {
            digitalWrite(r2, LOW);
            Serial.println(deviceName + "_WARN_T1_Low_SV_" + T1SV + "_PV_" + T1);
            lcd.setCursor(3, 1);
            lcd.print("L");
          } else if (T1 == T1SV.toFloat() || T1 < T1UL.toFloat() && T1 > T1LL.toFloat()) {
            digitalWrite(r1, HIGH);
            digitalWrite(r2, HIGH);
            lcd.setCursor(3, 1);
            lcd.print(" ");
          }
        }
        if (!temp2 && T2LL.toFloat() != 0 && T2UL.toFloat() != 0) {
          if (T2 > T2UL.toFloat()) {
            digitalWrite(r3, LOW);
            Serial.println(deviceName + "_WARN_T2_High_SV_" + T2SV + "_PV_" + T2);
            lcd.setCursor(8, 1);
            lcd.print("H");
          } else if (T2 < T2LL.toFloat()) {
            digitalWrite(r4, LOW);
            Serial.println(deviceName + "_WARN_T2_Low_SV_" + T2SV + "_PV_" + T2);
            lcd.setCursor(8, 1);
            lcd.print("L");
          } else if (T2 == T2SV.toFloat() || T2 < T2UL.toFloat() && T2 > T2LL.toFloat()) {
            digitalWrite(r3, HIGH);
            digitalWrite(r4, HIGH);
            lcd.setCursor(8, 1);
            lcd.print(" ");
          }
        }
        if (!temp3 && T3LL.toFloat() != 0 && T3UL.toFloat() != 0) {
          if (T3 > T3UL.toFloat()) {
            digitalWrite(r5, LOW);
            Serial.println(deviceName + "_WARN_T3_High_SV_" + T3SV + "_PV_" + T3);
            lcd.setCursor(13, 1);
            lcd.print("H");
          } else if (T3 < T3LL.toFloat()) {
            digitalWrite(r6, LOW);
            Serial.println(deviceName + "_WARN_T3_Low_SV_" + T3SV + "_PV_" + T3);
            lcd.setCursor(13, 1);
            lcd.print("L");
          } else if (T3 == T3SV.toFloat() || T3 < T3UL.toFloat() && T3 > T3LL.toFloat()) {
            digitalWrite(r5, HIGH);
            digitalWrite(r6, HIGH);
            lcd.setCursor(13, 1);
            lcd.print(" ");
          }
        }
        dataFile = SD.open("temp3.csv", FILE_WRITE);
        if (dataFile) {
          dataFile.print(deviceName);
          dataFile.print(", ");
          dataFile.print(T1);
          dataFile.print(", ");
          dataFile.print(T2);
          dataFile.print(", ");
          dataFile.print(T3);
          dataFile.print(", ");
          dataFile.print(rType);
          dataFile.print(", ");
          dataFile.print(time);
          dataFile.print(", ");
          dataFile.print(date);
          dataFile.close();
        } else {
          Serial.println(deviceName + "_Error_OpenFileT3");
        }

      } else if (NoOfSensor.equals("4")) {
        if (isnan(T1)) {
          Serial.println(deviceName + "_Error_S1_Fail");
          temp1 = true;
        }
        if (isnan(T2)) {
          Serial.println(deviceName + "_Error_S2_Fail");
          temp2 = true;
        }
        if (isnan(T3)) {
          Serial.println(deviceName + "_Error_S3_Fail");
          temp3 = true;
        }
        if (isnan(T4)) {
          Serial.println(deviceName + "_Error_S4_Fail");
          temp4 = true;
        }
        Serial.print(deviceName);
        Serial.print("_VALUES_" + rType + "_");
        Serial.print("T1: ");
        if (temp1) {
          Serial.print("NA");
        } else {
          Serial.print(T1, 2);
        }
        Serial.print(" T2: ");
        if (temp2) {
          Serial.print("NA");
        } else {
          Serial.print(T2, 2);
        }
        Serial.print(" T3: ");
        if (temp3) {
          Serial.print("NA");
        } else {
          Serial.print(T3, 2);
        }
        Serial.print(" T4: ");
        if (temp4) {
          Serial.print("NA");
        } else {
          Serial.print(T4, 2);
        }
        dateTime();
        Serial.println();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T");
        lcd.setCursor(1, 0);
        if (temp1) {
          lcd.print("NA");
        } else {
          lcd.print(T1);
        }
        lcd.setCursor(3, 0);
        if (temp1) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(4, 0);
        lcd.print(" ");
        lcd.setCursor(5, 0);
        if (temp2) {
          lcd.print("NA");
        } else {
          lcd.print(T2);
        }
        lcd.setCursor(7, 0);
        if (temp2) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(8, 0);
        lcd.print(" ");
        lcd.setCursor(9, 0);
        if (temp3) {
          lcd.print("NA");
        } else {
          lcd.print(T3);
        }
        lcd.setCursor(11, 0);
        if (temp3) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(12, 0);
        lcd.print(" ");
        lcd.setCursor(13, 0);
        if (temp4) {
          lcd.print("NA");
        } else {
          lcd.print(T4);
        }
        lcd.setCursor(15, 0);
        if (temp4) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(0, 1);
        if (day < 10) {
          lcd.print(0 + day, DEC);
        } else {
          lcd.print(day, DEC);
        }
        lcd.print('/');
        if (month < 10) {
          lcd.print(0 + month, DEC);
        } else {
          lcd.print(month, DEC);
        }
        lcd.setCursor(11, 1);
        if (hour < 10) {
          lcd.print(0 + hour, DEC);
        } else {
          lcd.print(hour, DEC);
        }
        lcd.print(':');
        if (minute < 10) {
          lcd.print(0 + minute, DEC);
        } else {
          lcd.print(minute, DEC);
        }

        if (!temp1 && T1LL.toFloat() != 0 && T1UL.toFloat() != 0) {
          if (T1 > T1UL.toFloat()) {
            digitalWrite(r1, LOW);
            Serial.println(deviceName + "_WARN_T1_High_SV_" + T1SV + "_PV_" + T1);
          } else if (T1 < T1LL.toFloat()) {
            digitalWrite(r2, LOW);
            Serial.println(deviceName + "_WARN_T1_Low_SV_" + T1SV + "_PV_" + T1);
          } else if (T1 == T1SV.toFloat() || T1 < T1UL.toFloat() && T1 > T1LL.toFloat()) {
            digitalWrite(r1, HIGH);
            digitalWrite(r2, HIGH);
          }
        }
        if (!temp2 && T2LL.toFloat() != 0 && T2UL.toFloat() != 0) {
          if (T2 > T2UL.toFloat()) {
            digitalWrite(r3, LOW);
            Serial.println(deviceName + "_WARN_T2_High_SV_" + T2SV + "_PV_" + T2);
          } else if (T2 < T2LL.toFloat()) {
            digitalWrite(r4, LOW);
            Serial.println(deviceName + "_WARN_T2_Low_SV_" + T2SV + "_PV_" + T2);
          } else if (T2 == T2SV.toFloat() || T2 < T2UL.toFloat() && T2 > T2LL.toFloat()) {
            digitalWrite(r3, HIGH);
            digitalWrite(r4, HIGH);
          }
        }
        if (!temp3 && T3LL.toFloat() != 0 && T3UL.toFloat() != 0) {
          if (T3 > T3UL.toFloat()) {
            digitalWrite(r5, LOW);
            Serial.println(deviceName + "_WARN_T3_High_SV_" + T3SV + "_PV_" + T3);
          } else if (T3 < T3LL.toFloat()) {
            digitalWrite(r6, LOW);
            Serial.println(deviceName + "_WARN_T3_Low_SV_" + T3SV + "_PV_" + T3);
          } else if (T3 == T3SV.toFloat() || T3 < T3UL.toFloat() && T3 > T3LL.toFloat()) {
            digitalWrite(r5, HIGH);
            digitalWrite(r6, HIGH);
          }
        }
        if (!temp4 && T4LL.toFloat() != 0 && T4UL.toFloat() != 0) {
          if (T4 > T4UL.toFloat()) {
            digitalWrite(r7, LOW);
            Serial.println(deviceName + "_WARN_T4_High_SV_" + T4SV + "_PV_" + T4);
          } else if (T4 < T4LL.toFloat()) {
            digitalWrite(r8, LOW);
            Serial.println(deviceName + "_WARN_T4_Low_SV_" + T4SV + "_PV_" + T4);
          } else if (T4 == T4SV.toFloat() || T4 < T4UL.toFloat() && T4 > T4LL.toFloat()) {
            digitalWrite(r7, HIGH);
            digitalWrite(r8, HIGH);
          }
        }
        dataFile = SD.open("temp4.csv", FILE_WRITE);
        if (dataFile) {
          dataFile.print(deviceName);
          dataFile.print(", ");
          dataFile.print(T1);
          dataFile.print(", ");
          dataFile.print(T2);
          dataFile.print(", ");
          dataFile.print(T3);
          dataFile.print(", ");
          dataFile.print(T4);
          dataFile.print(", ");
          dataFile.print(rType);
          dataFile.print(", ");
          dataFile.print(time);
          dataFile.print(", ");
          dataFile.print(date);
          dataFile.close();
        } else {
          Serial.println(deviceName + "_Error_OpenFileT4");
        }
      }
    } else if (logType.equals("Humidity")) {
      if (NoOfSensor.equals("1")) {
        if (isnan(H1)) {
          Serial.println(deviceName + "_Error_S1_Fail");
          humi1 = true;
        }
        Serial.print(deviceName);
        Serial.print("_VALUES_");
        Serial.print(" H1: ");
        if (humi1) {
          Serial.print("NA");
        } else {
          Serial.print(H1);
        }
        dateTime();
        Serial.println();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(deviceName);
        lcd.setCursor(11, 0);
        if (hour < 10) {
          lcd.print(0 + hour, DEC);
        } else {
          lcd.print(hour, DEC);
        }
        lcd.print(':');
        if (minute < 10) {
          lcd.print(0 + minute, DEC);
        } else {
          lcd.print(minute, DEC);
        }
        lcd.setCursor(0, 1);
        lcd.print("H:");
        if (humi1) {
          lcd.print("NA");
        } else {
          lcd.print(H1);
        }
        lcd.setCursor(4, 1);
        if (humi1) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(11, 1);
        if (day < 10) {
          lcd.print(0 + day, DEC);
        } else {
          lcd.print(day, DEC);
        }
        lcd.print('/');
        if (month < 10) {
          lcd.print(0 + month, DEC);
        } else {
          lcd.print(month, DEC);
        }

        if (!humi1 && H1LL.toInt() != 0 && H1UL.toInt() != 0) {
          if (H1 > H1UL.toInt()) {
            digitalWrite(r1, LOW);
            Serial.println(deviceName + "_WARN_H1_High_SV_" + H1SV + "_PV_" + H1);
            lcd.setCursor(5, 1);
            lcd.print("H");
          } else if (H1 < H1LL.toInt()) {
            digitalWrite(r2, LOW);
            Serial.println(deviceName + "_WARN_H1_Low_SV_" + H1SV + "_PV_" + H1);
            lcd.setCursor(5, 1);
            lcd.print("L");
          } else if (H1 == H1SV.toInt() || H1 < H1UL.toInt() && H1 > H1LL.toInt()) {
            digitalWrite(r1, HIGH);
            digitalWrite(r2, HIGH);
            lcd.setCursor(5, 1);
            lcd.print("  ");
          }
        }
        dataFile = SD.open("humi1.csv", FILE_WRITE);
        if (dataFile) {
          dataFile.print(deviceName);
          dataFile.print(", ");
          dataFile.print(H1);
          dataFile.print(", ");
          dataFile.print(time);
          dataFile.print(", ");
          dataFile.print(date);
          dataFile.close();
        } else {
          Serial.println(deviceName + "_Error_OpenFileH1");
        }

      } else if (NoOfSensor.equals("2")) {
        if (isnan(H1)) {
          Serial.println(deviceName + "_Error_S1_Fail");
          humi1 = true;
        }
        if (isnan(H2)) {
          Serial.println(deviceName + "_Error_S2_Fail");
          humi2 = true;
        }
        Serial.print(deviceName);
        Serial.print("_VALUES_");
        Serial.print(" H1: ");
        if (humi1) {
          Serial.print("NA");
        } else {
          Serial.print(H1);
        }
        Serial.print(" H2: ");
        if (humi2) {
          Serial.print("NA");
        } else {
          Serial.print(H2);
        }
        dateTime();
        Serial.println();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("H1:");
        if (humi1) {
          lcd.print("NA");
        } else {
          lcd.print(H1);
        }
        lcd.setCursor(5, 0);
        if (humi1) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(11, 0);
        if (hour < 10) {
          lcd.print(0 + hour, DEC);
        } else {
          lcd.print(hour, DEC);
        }
        lcd.print(':');
        if (minute < 10) {
          lcd.print(0 + minute, DEC);
        } else {
          lcd.print(minute, DEC);
        }
        lcd.setCursor(0, 1);
        lcd.print("H2:");
        if (humi2) {
          lcd.print("NA");
        } else {
          lcd.print(H2);
        }
        lcd.setCursor(5, 1);
        if (humi2) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(11, 1);
        if (day < 10) {
          lcd.print(0 + day, DEC);
        } else {
          lcd.print(day, DEC);
        }
        lcd.print('/');
        if (month < 10) {
          lcd.print(0 + month, DEC);
        } else {
          lcd.print(month, DEC);
        }

        if (!humi1 && H1LL.toInt() != 0 && H1UL.toInt() != 0) {
          if (H1 > H1UL.toInt()) {
            digitalWrite(r1, LOW);
            Serial.println(deviceName + "_WARN_H1_High_SV_" + H1SV + "_PV_" + H1);
            lcd.setCursor(6, 0);
            lcd.print("H");
          } else if (H1 < H1LL.toInt()) {
            digitalWrite(r2, LOW);
            Serial.println(deviceName + "_WARN_H1_Low_SV_" + H1SV + "_PV_" + H1);
            lcd.setCursor(6, 0);
            lcd.print("L");
          } else if (H1 == H1SV.toInt() || H1 < H1UL.toInt() && H1 > H1LL.toInt()) {
            digitalWrite(r1, HIGH);
            digitalWrite(r2, HIGH);
            lcd.setCursor(6, 0);
            lcd.print("  ");
          }
        }
        if (!humi2 && H2LL.toInt() != 0 && H2UL.toInt() != 0) {
          if (H2 > H2UL.toInt()) {
            digitalWrite(r3, LOW);
            Serial.println(deviceName + "_WARN_H2_High_SV_" + H2SV + "_PV_" + H2);
            lcd.setCursor(6, 1);
            lcd.print("H");
          } else if (H2 < H2LL.toInt()) {
            digitalWrite(r4, LOW);
            Serial.println(deviceName + "_WARN_H2_Low_SV_" + H2SV + "_PV_" + H2);
            lcd.setCursor(6, 1);
            lcd.print("L");
          } else if (H2 == H2SV.toInt() || H2 < H2UL.toInt() && H2 > H2LL.toInt()) {
            digitalWrite(r3, HIGH);
            digitalWrite(r4, HIGH);
            lcd.setCursor(6, 1);
            lcd.print(" ");
          }
        }
        dataFile = SD.open("humi2.csv", FILE_WRITE);
        if (dataFile) {
          dataFile.print(deviceName);
          dataFile.print(", ");
          dataFile.print(H1);
          dataFile.print(", ");
          dataFile.print(H2);
          dataFile.print(", ");
          dataFile.print(time);
          dataFile.print(", ");
          dataFile.print(date);
          dataFile.close();
        } else {
          Serial.println(deviceName + "_Error_OpenFileH2");
        }

      } else if (NoOfSensor.equals("3")) {
        if (isnan(H1)) {
          Serial.println(deviceName + "_Error_S1_Fail");
          humi1 = true;
        }
        if (isnan(H2)) {
          Serial.println(deviceName + "_Error_S2_Fail");
          humi2 = true;
        }
        if (isnan(H3)) {
          Serial.println(deviceName + "_Error_S3_Fail");
          humi3 = true;
        }
        Serial.print(deviceName);
        Serial.print("_VALUES_");
        Serial.print(" H1: ");
        if (humi1) {
          Serial.print("NA");
        } else {
          Serial.print(H1);
        }
        Serial.print(" H2: ");
        if (humi2) {
          Serial.print("NA");
        } else {
          Serial.print(H2);
        }
        Serial.print(" H3: ");
        if (humi3) {
          Serial.print("Na");
        } else {
          Serial.print(H3);
        }
        dateTime();
        Serial.println();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("H1   H2   H3");
        lcd.setCursor(0, 1);
        if (humi1) {
          lcd.print("NA");
        } else {
          lcd.print(H1);
        }
        lcd.setCursor(2, 1);
        if (humi1) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(3, 1);
        lcd.print("  ");
        lcd.setCursor(5, 1);
        if (humi2) {
          lcd.print("NA");
        } else {
          lcd.print(H2);
        }
        lcd.setCursor(7, 1);
        if (humi2) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(8, 1);
        lcd.print("  ");
        lcd.setCursor(10, 1);
        if (humi3) {
          lcd.print("NA");
        } else {
          lcd.print(H3);
        }
        lcd.setCursor(12, 1);
        if (humi3) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(13, 1);
        lcd.print("   ");
        if (!humi1 && H1LL.toInt() != 0 && H1UL.toInt() != 0) {
          if (H1 > H1UL.toInt()) {
            digitalWrite(r1, LOW);
            Serial.println(deviceName + "_WARN_H1_High_SV_" + H1SV + "_PV_" + H1);
            lcd.setCursor(3, 1);
            lcd.print("H");
          } else if (H1 < H1LL.toInt()) {
            digitalWrite(r2, LOW);
            Serial.println(deviceName + "_WARN_H1_Low_SV_" + H1SV + "_PV_" + H1);
            lcd.setCursor(3, 1);
            lcd.print("L");
          } else if (H1 == H1SV.toInt() || H1 < H1UL.toInt() && H1 > H1LL.toInt()) {
            digitalWrite(r1, HIGH);
            digitalWrite(r2, HIGH);
            lcd.setCursor(3, 1);
            lcd.print(" ");
          }
        }
        if (!humi2 && H2LL.toInt() != 0 && H2UL.toInt() != 0) {
          if (H2 > H2UL.toInt()) {
            digitalWrite(r3, LOW);
            Serial.println(deviceName + "_WARN_H2_High_SV_" + H2SV + "_PV_" + H2);
            lcd.setCursor(8, 1);
            lcd.print("H");
          } else if (H2 < H2LL.toInt()) {
            digitalWrite(r4, LOW);
            Serial.println(deviceName + "_WARN_H2_Low_SV_" + H2SV + "_PV_" + H2);
            lcd.setCursor(8, 1);
            lcd.print("L");
          } else if (H2 == H2SV.toInt() || H2 < H2UL.toInt() && H2 > H2LL.toInt()) {
            digitalWrite(r3, HIGH);
            digitalWrite(r4, HIGH);
            lcd.setCursor(8, 1);
            lcd.print(" ");
          }
        }
        if (!humi3 && H3LL.toInt() != 0 && H3UL.toInt() != 0) {
          if (H3 > H3UL.toInt()) {
            digitalWrite(r5, LOW);
            Serial.println(deviceName + "_WARN_H3_High_SV_" + H3SV + "_PV_" + H3);
            lcd.setCursor(13, 1);
            lcd.print("H");
          } else if (H3 < H3LL.toInt()) {
            digitalWrite(r6, LOW);
            Serial.println(deviceName + "_WARN_H3_Low_SV_" + H3SV + "_PV_" + H3);
            lcd.setCursor(13, 1);
            lcd.print("L");
          } else if (H3 == H3SV.toInt() || H3 < H3UL.toInt() && H3 > H3LL.toInt()) {
            digitalWrite(r5, HIGH);
            digitalWrite(r6, HIGH);
            lcd.setCursor(13, 1);
            lcd.print(" ");
          }
        }
        dataFile = SD.open("humi3.csv", FILE_WRITE);
        if (dataFile) {
          dataFile.print(deviceName);
          dataFile.print(", ");
          dataFile.print(H1);
          dataFile.print(", ");
          dataFile.print(H2);
          dataFile.print(", ");
          dataFile.print(H3);
          dataFile.print(", ");
          dataFile.print(time);
          dataFile.print(", ");
          dataFile.print(date);
          dataFile.close();
        } else {
          Serial.println(deviceName + "_Error_OpenFileH3");
        }

      } else if (NoOfSensor.equals("4")) {
        if (isnan(H1)) {
          Serial.println(deviceName + "_Error_S1_Fail");
          humi1 = true;
        }
        if (isnan(H2)) {
          Serial.println(deviceName + "_Error_S2_Fail");
          humi2 = true;
        }
        if (isnan(H3)) {
          Serial.println(deviceName + "_Error_S3_Fail");
          humi3 = true;
        }
        if (isnan(H4)) {
          Serial.println(deviceName + "_Error_S4_Fail");
          humi4 = true;
        }
        Serial.print(deviceName);
        Serial.print("_VALUES_");
        if (humi1) {
          Serial.print("NA");
        } else {
          Serial.print(H1);
        }
        Serial.print(" ");
        if (humi2) {
          Serial.print("NA");
        } else {
          Serial.print(H2);
        }
        Serial.print(" ");
        if (humi3) {
          Serial.print("NA");
        } else {
          Serial.print(H3);
        }
        Serial.print(" ");
        if (humi4) {
          Serial.print("NA");
        } else {
          Serial.print(H4);
        }
        dateTime();
        Serial.println();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T");
        lcd.setCursor(1, 0);
        if (humi1) {
          lcd.print("NA");
        } else {
          lcd.print(H1);
        }
        lcd.setCursor(3, 0);
        if (humi1) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(4, 0);
        lcd.print(" ");
        lcd.setCursor(5, 0);
        if (humi2) {
          lcd.print("NA");
        } else {
          lcd.print(H2);
        }
        lcd.setCursor(7, 0);
        if (humi2) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(8, 0);
        lcd.print(" ");
        lcd.setCursor(9, 0);
        if (humi3) {
          lcd.print("NA");
        } else {
          lcd.print(H3);
        }
        lcd.setCursor(11, 0);
        if (humi3) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(12, 0);
        lcd.print(" ");
        lcd.setCursor(13, 0);
        if (humi4) {
          lcd.print("NA");
        } else {
          lcd.print(H4);
        }
        lcd.setCursor(15, 0);
        if (humi4) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(0, 1);
        if (day < 10) {
          lcd.print(0 + day, DEC);
        } else {
          lcd.print(day, DEC);
        }
        lcd.print('/');
        if (month < 10) {
          lcd.print(0 + month, DEC);
        } else {
          lcd.print(month, DEC);
        }
        lcd.setCursor(11, 1);
        if (hour < 10) {
          lcd.print(0 + hour, DEC);
        } else {
          lcd.print(hour, DEC);
        }
        lcd.print(':');
        if (minute < 10) {
          lcd.print(0 + minute, DEC);
        } else {
          lcd.print(minute, DEC);
        }

        if (!humi1 && H1LL.toInt() != 0 && H1UL.toInt() != 0) {
          if (H1 > H1UL.toInt()) {
            digitalWrite(r7, LOW);
            Serial.println(deviceName + "_WARN_H1_High_SV_" + H1SV + "_PV_" + H1);
          } else if (H1 < H1LL.toInt()) {
            digitalWrite(r8, LOW);
            Serial.println(deviceName + "_WARN_H1_Low_SV_" + H1SV + "_PV_" + H1);
          } else if (H1 == H1SV.toInt() || H1 < H1UL.toInt() && H1 > H1LL.toInt()) {
            digitalWrite(r7, HIGH);
            digitalWrite(r8, HIGH);
          }
        }
        if (!humi2 && H2LL.toInt() != 0 && H2UL.toInt() != 0) {
          if (H2 > H2UL.toInt()) {
            digitalWrite(r7, LOW);
            Serial.println(deviceName + "_WARN_H2_High_SV_" + H2SV + "_PV_" + H2);
          } else if (H2 < H2LL.toInt()) {
            digitalWrite(r8, LOW);
            Serial.println(deviceName + "_WARN_H2_Low_SV_" + H2SV + "_PV_" + H2);
          } else if (H2 == H2SV.toInt() || H2 < H2UL.toInt() && H2 > H2LL.toInt()) {
            digitalWrite(r7, HIGH);
            digitalWrite(r8, HIGH);
          }
        }
        if (!humi3 && H3LL.toInt() != 0 && H3UL.toInt() != 0) {
          if (H3 > H3UL.toInt()) {
            digitalWrite(r7, LOW);
            Serial.println(deviceName + "_WARN_H3_High_SV_" + H3SV + "_PV_" + H3);
          } else if (H3 < H3LL.toInt()) {
            digitalWrite(r8, LOW);
            Serial.println(deviceName + "_WARN_H3_Low_SV_" + H3SV + "_PV_" + H3);
          } else if (H3 == H3SV.toInt() || H3 < H3UL.toInt() && H3 > H3LL.toInt()) {
            digitalWrite(r7, HIGH);
            digitalWrite(r8, HIGH);
          }
        }
        if (!humi4 && H4LL.toInt() != 0 && H4UL.toInt() != 0) {
          if (H4 > H4UL.toInt()) {
            digitalWrite(r7, LOW);
            Serial.println(deviceName + "_WARN_H4_High_SV_" + H4SV + "_PV_" + H4);
          } else if (H4 < H4LL.toInt()) {
            digitalWrite(r8, LOW);
            Serial.println(deviceName + "_WARN_H4_Low_SV_" + H4SV + "_PV_" + H4);
          } else if (H4 == H4SV.toInt() || H4 < H4UL.toInt() && H4 > H4LL.toInt()) {
            digitalWrite(r7, HIGH);
            digitalWrite(r8, HIGH);
          }
        }
        dataFile = SD.open("humi4.csv", FILE_WRITE);
        if (dataFile) {
          dataFile.print(deviceName);
          dataFile.print(", ");
          dataFile.print(H1);
          dataFile.print(", ");
          dataFile.print(H2);
          dataFile.print(", ");
          dataFile.print(H3);
          dataFile.print(", ");
          dataFile.print(H4);
          dataFile.print(", ");
          dataFile.print(time);
          dataFile.print(", ");
          dataFile.print(date);
          dataFile.close();
        } else {
          Serial.println(deviceName + "_Error_OpenFileH4");
        }
      }
    } else if (logType.equals("Both")) {
      if (NoOfSensor.equals("1")) {
        if (isnan(T1) || isnan(H1)) {
          Serial.println(deviceName + "_Error_S1_Fail");
          temp1 = true;
          humi1 = true;
        }
        Serial.print(deviceName);
        Serial.print("_VALUES_" + rType + "_");
        if (temp1) {
          Serial.print("NA");
        } else {
          Serial.print(T1, 2);
        }
        Serial.print(" ");
        if (humi1) {
          Serial.print("NA");
        } else {
          Serial.print(H1);
        }
        dateTime();
        Serial.println();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T:");
        if (temp1) {
          lcd.print("NA");
        } else {
          lcd.print(T1, 2);
        }
        lcd.setCursor(7, 0);
        if (temp1) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(11, 0);
        if (hour < 10) {
          lcd.print(0 + hour, DEC);
        } else {
          lcd.print(hour, DEC);
        }
        lcd.print(':');
        if (minute < 10) {
          lcd.print(0 + minute, DEC);
        } else {
          lcd.print(minute, DEC);
        }
        lcd.setCursor(0, 1);
        lcd.print("H:");
        if (humi1) {
          lcd.print("NA");
        } else {
          lcd.print(H1);
        }
        lcd.setCursor(4, 1);
        if (humi1) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(11, 1);
        if (day < 10) {
          lcd.print(0 + day, DEC);
        } else {
          lcd.print(day, DEC);
        }
        lcd.print('/');
        if (month < 10) {
          lcd.print(0 + month, DEC);
        } else {
          lcd.print(month, DEC);
        }
        if (!temp1 && T1LL.toFloat() != 0 && T1UL.toFloat() != 0) {
          if (T1 > T1UL.toFloat()) {
            digitalWrite(r1, LOW);
            Serial.println(deviceName + "_WARN_T1_High_SV_" + T1SV + "_PV_" + T1);
            lcd.setCursor(8, 0);
            lcd.print("HT");
          } else if (T1 < T1LL.toFloat()) {
            digitalWrite(r2, LOW);
            Serial.println(deviceName + "_WARN_T1_Low_SV_" + T1SV + "_PV_" + T1);
            lcd.setCursor(8, 0);
            lcd.print("LT");
          } else if (T1 == T1SV.toFloat() || T1 < T1UL.toFloat() && T1 > T1LL.toFloat()) {
            digitalWrite(r1, HIGH);
            digitalWrite(r2, HIGH);
            lcd.setCursor(8, 0);
            lcd.print("  ");
          }
        }
        if (!humi1 && H1LL.toInt() != 0 && H1UL.toInt() != 0) {
          if (H1 > H1UL.toInt()) {
            digitalWrite(r1, LOW);
            Serial.println(deviceName + "_WARN_H1_High_SV_" + H1SV + "_PV_" + H1);
            lcd.setCursor(5, 1);
            lcd.print("H");
          } else if (H1 < H1LL.toInt()) {
            digitalWrite(r2, LOW);
            Serial.println(deviceName + "_WARN_H1_Low_SV_" + H1SV + "_PV_" + H1);
            lcd.setCursor(5, 1);
            lcd.print("L");
          } else if (H1 == H1SV.toInt() || H1 < H1UL.toInt() && H1 > H1LL.toInt()) {
            digitalWrite(r1, HIGH);
            digitalWrite(r2, HIGH);
            lcd.setCursor(5, 1);
            lcd.print(" ");
          }
        }
        dataFile = SD.open("both1.csv", FILE_WRITE);
        if (dataFile) {
          dataFile.print(deviceName);
          dataFile.print(", ");
          dataFile.print(T1);
          dataFile.print(", ");
          dataFile.print(H1);
          dataFile.print(", ");
          dataFile.print(rType);
          dataFile.print(", ");
          dataFile.print(time);
          dataFile.print(", ");
          dataFile.print(date);
          dataFile.close();
        } else {
          Serial.println(deviceName + "_Error_OpenFileB1");
        }

      } else if (NoOfSensor.equals("2")) {
        if (isnan(T1) || isnan(H1)) {
          Serial.println(deviceName + "_Error_S1_Fail");
          temp1 = true;
          humi1 = true;
        }
        if (isnan(T2) || isnan(H2)) {
          Serial.println(deviceName + "_Error_S2_Fail");
          temp2 = true;
          humi2 = true;
        }
        Serial.print(deviceName);
        Serial.print("_VALUES_" + rType + "_");
        if (temp1) {
          Serial.print("NA");
        } else {
          Serial.print(T1, 2);
        }
        Serial.print(" ");
        if (humi1) {
          Serial.print("NA");
        } else {
          Serial.print(H1);
        }
        Serial.print(" ");
        if (temp2) {
          Serial.print("NA");
        } else {
          Serial.print(T2, 2);
        }
        Serial.print(" ");
        if (humi2) {
          Serial.print("NA");
        } else {
          Serial.print(H2);
        }
        dateTime();
        Serial.println();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T1:");
        lcd.setCursor(3, 0);
        if (temp1) {
          lcd.print("NA");
        } else {
          lcd.print(T1);
        }
        lcd.setCursor(5, 0);
        if (temp1) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(6, 0);
        lcd.print("   ");
        lcd.setCursor(9, 0);
        lcd.print("T2:");
        lcd.setCursor(12, 0);
        if (temp2) {
          lcd.print("NA");
        } else {
          lcd.print(T2);
        }
        lcd.setCursor(14, 0);
        if (temp2) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(15, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print("H1:");
        lcd.setCursor(3, 1);
        if (humi1) {
          lcd.print("NA");
        } else {
          lcd.print(H1);
        }
        lcd.setCursor(5, 1);
        if (humi1) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(6, 1);
        lcd.print("   ");
        lcd.setCursor(9, 1);
        lcd.print("H2:");
        lcd.setCursor(12, 1);
        if (humi2) {
          lcd.print("NA");
        } else {
          lcd.print(H2);
        }
        lcd.setCursor(14, 1);
        if (humi2) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(15, 1);
        lcd.print(" ");
        if (!temp1 && T1LL.toFloat() != 0 && T1UL.toFloat() != 0) {
          if (T1 > T1UL.toFloat()) {
            digitalWrite(r1, LOW);
            Serial.println(deviceName + "_WARN_T1_High_SV_" + T1SV + "_PV_" + T1);
            lcd.setCursor(6, 0);
            lcd.print("H");
          } else if (T1 < T1LL.toFloat()) {
            digitalWrite(r2, LOW);
            Serial.println(deviceName + "_WARN_T1_Low_SV_" + T1SV + "_PV_" + T1);
            lcd.setCursor(6, 0);
            lcd.print("L");
          } else if (T1 == T1SV.toFloat() || T1 < T1UL.toFloat() && T1 > T1LL.toFloat()) {
            digitalWrite(r1, HIGH);
            digitalWrite(r2, HIGH);
            lcd.setCursor(6, 0);
            lcd.print(" ");
          }
        }
        if (!temp2 && T2LL.toFloat() != 0 && T2UL.toFloat() != 0) {
          if (T2 > T2UL.toFloat()) {
            digitalWrite(r3, LOW);
            Serial.println(deviceName + "_WARN_T2_High_SV_" + T2SV + "_PV_" + T2);
            lcd.setCursor(15, 0);
            lcd.print("H");
          } else if (T2 < T2LL.toFloat()) {
            digitalWrite(r4, LOW);
            Serial.println(deviceName + "_WARN_T2_Low_SV_" + T2SV + "_PV_" + T2);
            lcd.setCursor(15, 0);
            lcd.print("L");
          } else if (T2 == T2SV.toFloat() || T2 < T2UL.toFloat() && T2 > T2LL.toFloat()) {
            digitalWrite(r3, HIGH);
            digitalWrite(r4, HIGH);
            lcd.setCursor(15, 0);
            lcd.print(" ");
          }
        }
        if (!humi1 && H1LL.toInt() != 0 && H1UL.toInt() != 0) {
          if (H1 > H1UL.toInt()) {
            digitalWrite(r1, LOW);
            Serial.println(deviceName + "_WARN_H1_High_SV_" + H1SV + "_PV_" + H1);
            lcd.setCursor(6, 1);
            lcd.print("H");
          } else if (H1 < H1LL.toInt()) {
            digitalWrite(r2, LOW);
            Serial.println(deviceName + "_WARN_H1_Low_SV_" + H1SV + "_PV_" + H1);
            lcd.setCursor(6, 1);
            lcd.print("L");
          } else if (H1 == H1SV.toInt() || H1 < H1UL.toInt() && H1 > H1LL.toInt()) {
            digitalWrite(r1, HIGH);
            digitalWrite(r2, HIGH);
            lcd.setCursor(6, 1);
            lcd.print(" ");
          }
        }
        if (!humi2 && H2LL.toInt() != 0 && H2UL.toInt() != 0) {
          if (H2 > H2UL.toInt()) {
            digitalWrite(r3, LOW);
            Serial.println(deviceName + "_WARN_H2_High_SV_" + H2SV + "_PV_" + H2);
            lcd.setCursor(15, 1);
            lcd.print("H");
          } else if (H2 < H2LL.toInt()) {
            digitalWrite(r4, LOW);
            Serial.println(deviceName + "_WARN_H2_Low_SV_" + H2SV + "_PV_" + H2);
            lcd.setCursor(15, 1);
            lcd.print("L");
          } else if (H2 == H2SV.toInt() || H2 < H2UL.toInt() && H2 > H2LL.toInt()) {
            digitalWrite(r3, HIGH);
            digitalWrite(r4, HIGH);
            lcd.setCursor(15, 1);
            lcd.print(" ");
          }
        }
        dataFile = SD.open("both2.csv", FILE_WRITE);
        if (dataFile) {
          dataFile.print(deviceName);
          dataFile.print(", ");
          dataFile.print(T1);
          dataFile.print(", ");
          dataFile.print(H1);
          dataFile.print(", ");
          dataFile.print(T2);
          dataFile.print(", ");
          dataFile.print(H2);
          dataFile.print(", ");
          dataFile.print(rType);
          dataFile.print(", ");
          dataFile.print(time);
          dataFile.print(", ");
          dataFile.print(date);
          dataFile.close();
        } else {
          Serial.println(deviceName + "_Error_OpenFileB2");
        }

      } else if (NoOfSensor.equals("3")) {
        if (isnan(T1) || isnan(H1)) {
          Serial.println(deviceName + "_Error_S1_Fail");
          temp1 = true;
          humi1 = true;
        }
        if (isnan(T2) || isnan(H2)) {
          Serial.println(deviceName + "_Error_S2_Fail");
          temp2 = true;
          humi2 = true;
        }
        if (isnan(T3) || isnan(H3)) {
          Serial.println(deviceName + "_Error_S3_Fail");
          temp3 = true;
          humi3 = true;
        }
        Serial.print(deviceName);
        Serial.print("_VALUES_" + rType + "_");
        if (temp1) {
          Serial.print("NA");
        } else {
          Serial.print(T1, 2);
        }
        Serial.print(" ");
        if (humi1) {
          Serial.print("NA");
        } else {
          Serial.print(H1);
        }
        Serial.print(" ");
        if (temp2) {
          Serial.print("NA");
        } else {
          Serial.print(T2, 2);
        }
        Serial.print(" ");
        if (humi2) {
          Serial.print("NA");
        } else {
          Serial.print(H2);
        }
        Serial.print(" ");
        if (temp3) {
          Serial.print("NA");
        } else {
          Serial.print(T3, 2);
        }
        Serial.print(" ");
        if (humi3) {
          Serial.print("NA");
        } else {
          Serial.print(H3);
        }
        dateTime();
        Serial.println();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T:");
        lcd.setCursor(2, 0);
        if (temp1) {
          lcd.print("NA");
        } else {
          lcd.print(T1);
        }
        lcd.setCursor(4, 0);
        if (temp1) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(5, 0);
        lcd.print("  ");
        lcd.setCursor(7, 0);
        if (temp2) {
          lcd.print("NA");
        } else {
          lcd.print(T2);
        }
        lcd.setCursor(9, 0);
        if (temp2) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(10, 0);
        lcd.print("  ");
        lcd.setCursor(12, 0);
        if (temp3) {
          lcd.print("NA");
        } else {
          lcd.print(T3);
        }
        lcd.setCursor(14, 0);
        if (temp3) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(15, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print("H:");
        lcd.setCursor(2, 1);
        if (humi1) {
          lcd.print("NA");
        } else {
          lcd.print(H1);
        }
        lcd.setCursor(4, 1);
        if (humi1) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(5, 1);
        lcd.print("  ");
        lcd.setCursor(7, 1);
        if (humi2) {
          lcd.print("NA");
        } else {
          lcd.print(H2);
        }
        lcd.setCursor(9, 1);
        if (humi2) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(10, 1);
        lcd.print("  ");
        lcd.setCursor(12, 1);
        if (humi3) {
          lcd.print("NA");
        } else {
          lcd.print(H3);
        }
        lcd.setCursor(14, 1);
        if (humi3) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(15, 1);
        lcd.print(" ");

        if (!temp1 && T1LL.toFloat() != 0 && T1UL.toFloat() != 0) {
          if (T1 > T1UL.toFloat()) {
            digitalWrite(r1, LOW);
            Serial.println(deviceName + "_WARN_T1_High_SV_" + T1SV + "_PV_" + T1);
            lcd.setCursor(5, 0);
            lcd.print("H");
          } else if (T1 < T1LL.toFloat()) {
            digitalWrite(r2, LOW);
            Serial.println(deviceName + "_WARN_T1_Low_SV_" + T1SV + "_PV_" + T1);
            lcd.setCursor(5, 0);
            lcd.print("L");
          } else if (T1 == T1SV.toFloat() || T1 < T1UL.toFloat() && T1 > T1LL.toFloat()) {
            digitalWrite(r1, HIGH);
            digitalWrite(r2, HIGH);
            lcd.setCursor(5, 0);
            lcd.print(" ");
          }
        }
        if (!temp2 && T2LL.toFloat() != 0 && T2UL.toFloat() != 0) {
          if (T2 > T2UL.toFloat()) {
            digitalWrite(r3, LOW);
            Serial.println(deviceName + "_WARN_T2_High_SV_" + T2SV + "_PV_" + T2);
            lcd.setCursor(10, 0);
            lcd.print("H");
          } else if (T2 < T2LL.toFloat()) {
            digitalWrite(r4, LOW);
            Serial.println(deviceName + "_WARN_T2_Low_SV_" + T2SV + "_PV_" + T2);
            lcd.setCursor(10, 0);
            lcd.print("L");
          } else if (T2 == T2SV.toFloat() || T2 < T2UL.toFloat() && T2 > T2LL.toFloat()) {
            digitalWrite(r3, HIGH);
            digitalWrite(r4, HIGH);
            lcd.setCursor(10, 0);
            lcd.print(" ");
          }
        }
        if (!temp3 && T3LL.toFloat() != 0 && T3UL.toFloat() != 0) {
          if (T3 > T3UL.toFloat()) {
            digitalWrite(r5, LOW);
            Serial.println(deviceName + "_WARN_T3_High_SV_" + T3SV + "_PV_" + T3);
            lcd.setCursor(15, 0);
            lcd.print("H");
          } else if (T3 < T3LL.toFloat()) {
            digitalWrite(r6, LOW);
            Serial.println(deviceName + "_WARN_T3_Low_SV_" + T3SV + "_PV_" + T3);
            lcd.setCursor(15, 0);
            lcd.print("L");
          } else if (T3 == T3SV.toFloat() || T3 < T3UL.toFloat() && T3 > T3LL.toFloat()) {
            digitalWrite(r5, HIGH);
            digitalWrite(r6, HIGH);
            lcd.setCursor(15, 0);
            lcd.print(" ");
          }
        }
        if (!humi1 && H1LL.toInt() != 0 && H1UL.toInt() != 0) {
          if (H1 > H1UL.toInt()) {
            digitalWrite(r1, LOW);
            Serial.println(deviceName + "_WARN_H1_High_SV_" + H1SV + "_PV_" + H1);
            lcd.setCursor(5, 1);
            lcd.print("H");
          } else if (H1 < H1LL.toInt()) {
            digitalWrite(r2, LOW);
            Serial.println(deviceName + "_WARN_H1_Low_SV_" + H1SV + "_PV_" + H1);
            lcd.setCursor(5, 1);
            lcd.print("L");
          } else if (H1 == H1SV.toInt() || H1 < H1UL.toInt() && H1 > H1LL.toInt()) {
            digitalWrite(r1, HIGH);
            digitalWrite(r2, HIGH);
            lcd.setCursor(5, 1);
            lcd.print(" ");
          }
        }
        if (!humi2 && H2LL.toInt() != 0 && H2UL.toInt() != 0) {
          if (H2 > H2UL.toInt()) {
            digitalWrite(r3, LOW);
            Serial.println(deviceName + "_WARN_H2_High_SV_" + H2SV + "_PV_" + H2);
            lcd.setCursor(10, 1);
            lcd.print("H");
          } else if (H2 < H2LL.toInt()) {
            digitalWrite(r4, LOW);
            Serial.println(deviceName + "_WARN_H2_Low_SV_" + H2SV + "_PV_" + H2);
            lcd.setCursor(10, 1);
            lcd.print("L");
          } else if (H2 == H2SV.toInt() || H2 < H2UL.toInt() && H2 > H2LL.toInt()) {
            digitalWrite(r3, HIGH);
            digitalWrite(r4, HIGH);
            lcd.setCursor(10, 1);
            lcd.print(" ");
          }
        }
        if (!humi3 && H3LL.toInt() != 0 && H3UL.toInt() != 0) {
          if (H3 > H3UL.toInt()) {
            digitalWrite(r5, LOW);
            Serial.println(deviceName + "_WARN_H3_High_SV_" + H3SV + "_PV_" + H3);
            lcd.setCursor(15, 1);
            lcd.print("H");
          } else if (H3 < H3LL.toInt()) {
            digitalWrite(r6, LOW);
            Serial.println(deviceName + "_WARN_H3_Low_SV_" + H3SV + "_PV_" + H3);
            lcd.setCursor(15, 1);
            lcd.print("L");
          } else if (H3 == H3SV.toInt() || H3 < H3UL.toInt() && H3 > H3LL.toInt()) {
            digitalWrite(r5, HIGH);
            digitalWrite(r6, HIGH);
            lcd.setCursor(15, 1);
            lcd.print(" ");
          }
        }
        dataFile = SD.open("both3.csv", FILE_WRITE);
        if (dataFile) {
          dataFile.print(deviceName);
          dataFile.print(", ");
          dataFile.print(T1);
          dataFile.print(", ");
          dataFile.print(H1);
          dataFile.print(", ");
          dataFile.print(T2);
          dataFile.print(", ");
          dataFile.print(H2);
          dataFile.print(", ");
          dataFile.print(T3);
          dataFile.print(", ");
          dataFile.print(H3);
          dataFile.print(", ");
          dataFile.print(rType);
          dataFile.print(", ");
          dataFile.print(time);
          dataFile.print(", ");
          dataFile.print(date);
          dataFile.close();
        } else {
          Serial.println(deviceName + "_Error_OpenFileB3");
        }

      } else if (NoOfSensor.equals("4")) {
        if (isnan(T1) || isnan(H1)) {
          Serial.println(deviceName + "_Error_S1_Fail");
          temp1 = true;
          humi1 = true;
        }
        if (isnan(T2) || isnan(H2)) {
          Serial.println(deviceName + "_Error_S2_Fail");
          temp2 = true;
          humi2 = true;
        }
        if (isnan(T3) || isnan(H3)) {
          Serial.println(deviceName + "_Error_S3_Fail");
          temp3 = true;
          humi3 = true;
        }
        if (isnan(T4) || isnan(H4)) {
          Serial.println(deviceName + "_Error_S4_Fail");
          temp4 = true;
          humi4 = true;
        }
        Serial.print(deviceName);
        Serial.print("_VALUES_" + rType + "_");
        if (temp1) {
          Serial.print("NA");
        } else {
          Serial.print(T1, 2);
        }
        Serial.print(" ");
        if (humi1) {
          Serial.print("NA");
        } else {
          Serial.print(H1);
        }
        Serial.print(" ");
        if (temp2) {
          Serial.print("NA");
        } else {
          Serial.print(T2, 2);
        }
        Serial.print(" ");
        if (humi2) {
          Serial.print("NA");
        } else {
          Serial.print(H2);
        }
        Serial.print(" ");
        if (temp3) {
          Serial.print("NA");
        } else {
          Serial.print(T3, 2);
        }
        Serial.print(" ");
        if (humi3) {
          Serial.print("NA");
        } else {
          Serial.print(H3);
        }
        Serial.print(" ");
        if (temp4) {
          Serial.print("NA");
        } else {
          Serial.print(T4, 2);
        }
        Serial.print(" ");
        if (humi4) {
          Serial.print("NA");
        } else {
          Serial.print(H4);
        }
        dateTime();
        Serial.println();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T");
        lcd.setCursor(1, 0);
        if (temp1) {
          lcd.print("NA");
        } else {
          lcd.print(T1);
        }
        lcd.setCursor(3, 0);
        if (temp1) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(4, 0);
        lcd.print(" ");
        lcd.setCursor(5, 0);
        if (temp2) {
          lcd.print("NA");
        } else {
          lcd.print(T2);
        }
        lcd.setCursor(7, 0);
        if (temp2) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(8, 0);
        lcd.print(" ");
        lcd.setCursor(9, 0);
        if (temp3) {
          lcd.print("NA");
        } else {
          lcd.print(T3);
        }
        lcd.setCursor(11, 0);
        if (temp3) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(12, 0);
        lcd.print(" ");
        lcd.setCursor(13, 0);
        if (temp4) {
          lcd.print("NA");
        } else {
          lcd.print(T4);
        }
        lcd.setCursor(15, 0);
        if (temp4) {
          lcd.print(" ");
        } else {
          lcd.print(rType);
        }
        lcd.setCursor(0, 1);
        lcd.print("H");
        lcd.setCursor(1, 1);
        if (humi1) {
          lcd.print("NA");
        } else {
          lcd.print(H1);
        }
        lcd.setCursor(3, 1);
        if (humi1) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(4, 1);
        lcd.print(" ");
        lcd.setCursor(5, 1);
        if (humi2) {
          lcd.print("NA");
        } else {
          lcd.print(H2);
        }
        lcd.setCursor(7, 1);
        if (humi2) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(8, 1);
        lcd.print(" ");
        lcd.setCursor(9, 1);
        if (humi3) {
          lcd.print("NA");
        } else {
          lcd.print(H3);
        }
        lcd.setCursor(11, 1);
        if (humi3) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        lcd.setCursor(12, 1);
        lcd.print(" ");
        lcd.setCursor(13, 1);
        if (humi4) {
          lcd.print("NA");
        } else {
          lcd.print(H4);
        }
        lcd.setCursor(15, 0);
        if (humi4) {
          lcd.print(" ");
        } else {
          lcd.print("%");
        }
        if (!temp1 && T1LL.toFloat() != 0 && T1UL.toFloat() != 0) {
          if (T1 > T1UL.toFloat()) {
            digitalWrite(r1, LOW);
            Serial.println(deviceName + "_WARN_T1_High_SV_" + T1SV + "_PV_" + T1);
          } else if (T1 < T1LL.toFloat()) {
            digitalWrite(r2, LOW);
            Serial.println(deviceName + "_WARN_T1_Low_SV_" + T1SV + "_PV_" + T1);
          } else if (T1 == T1SV.toFloat() || T1 < T1UL.toFloat() && T1 > T1LL.toFloat()) {
            digitalWrite(r1, HIGH);
            digitalWrite(r2, HIGH);
          }
        }
        if (!temp2 && T2LL.toFloat() != 0 && T2UL.toFloat() != 0) {
          if (T2 > T2UL.toFloat()) {
            digitalWrite(r3, LOW);
            Serial.println(deviceName + "_WARN_T2_High_SV_" + T2SV + "_PV_" + T2);
          } else if (T2 < T2LL.toFloat()) {
            digitalWrite(r4, LOW);
            Serial.println(deviceName + "_WARN_T2_Low_SV_" + T2SV + "_PV_" + T2);
          } else if (T2 == T2SV.toFloat() || T2 < T2UL.toFloat() && T2 > T2LL.toFloat()) {
            digitalWrite(r3, HIGH);
            digitalWrite(r4, HIGH);
          }
        }
        if (!temp3 && T3LL.toFloat() != 0 && T3UL.toFloat() != 0) {
          if (T3 > T3UL.toFloat()) {
            digitalWrite(r5, LOW);
            Serial.println(deviceName + "_WARN_T3_High_SV_" + T3SV + "_PV_" + T3);
          } else if (T3 < T3LL.toFloat()) {
            digitalWrite(r6, LOW);
            Serial.println(deviceName + "_WARN_T3_Low_SV_" + T3SV + "_PV_" + T3);
          } else if (T3 == T3SV.toFloat() || T3 < T3UL.toFloat() && T3 > T3LL.toFloat()) {
            digitalWrite(r5, HIGH);
            digitalWrite(r6, HIGH);
          }
        }
        if (!temp4 && T4LL.toFloat() != 0 && T4UL.toFloat() != 0) {
          if (T4 > T4UL.toFloat()) {
            digitalWrite(r7, LOW);
            Serial.println(deviceName + "_WARN_T4_High_SV_" + T4SV + "_PV_" + T4);
          } else if (T4 < T4LL.toFloat()) {
            digitalWrite(r8, LOW);
            Serial.println(deviceName + "_WARN_T4_Low_SV_" + T4SV + "_PV_" + T4);
          } else if (T4 == T4SV.toFloat() || T4 < T4UL.toFloat() && T4 > T4LL.toFloat()) {
            digitalWrite(r7, HIGH);
            digitalWrite(r8, HIGH);
          }
        }
        if (!humi1 && H1LL.toInt() != 0 && H1UL.toInt() != 0) {
          if (H1 > H1UL.toInt()) {
            digitalWrite(r1, LOW);
            Serial.println(deviceName + "_WARN_H1_High_SV_" + H1SV + "_PV_" + H1);
          } else if (H1 < H1LL.toInt()) {
            digitalWrite(r2, LOW);
            Serial.println(deviceName + "_WARN_H1_Low_SV_" + H1SV + "_PV_" + H1);
          } else if (H1 == H1SV.toInt() || H1 < H1UL.toInt() && H1 > H1LL.toInt()) {
            digitalWrite(r1, HIGH);
            digitalWrite(r2, HIGH);
          }
        }
        if (!humi2 && H2LL.toInt() != 0 && H2UL.toInt() != 0) {
          if (H2 > H2UL.toInt()) {
            digitalWrite(r3, LOW);
            Serial.println(deviceName + "_WARN_H2_High_SV_" + H2SV + "_PV_" + H2);
          } else if (H2 < H2LL.toInt()) {
            digitalWrite(r4, LOW);
            Serial.println(deviceName + "_WARN_H2_Low_SV_" + H2SV + "_PV_" + H2);
          } else if (H2 == H2SV.toInt() || H2 < H2UL.toInt() && H2 > H2LL.toInt()) {
            digitalWrite(r3, HIGH);
            digitalWrite(r4, HIGH);
          }
        }
        if (!humi3 && H3LL.toInt() != 0 && H3UL.toInt() != 0) {
          if (H3 > H3UL.toInt()) {
            digitalWrite(r5, LOW);
            Serial.println(deviceName + "_WARN_H3_High_SV_" + H3SV + "_PV_" + H3);
          } else if (H3 < H3LL.toInt()) {
            digitalWrite(r6, LOW);
            Serial.println(deviceName + "_WARN_H3_Low_SV_" + H3SV + "_PV_" + H3);
          } else if (H3 == H3SV.toInt() || H3 < H3UL.toInt() && H3 > H3LL.toInt()) {
            digitalWrite(r5, HIGH);
            digitalWrite(r6, HIGH);
          }
        }
        if (!humi4 && H4LL.toInt() != 0 && H4UL.toInt() != 0) {
          if (H4 > H4UL.toInt()) {
            digitalWrite(r7, LOW);
            Serial.println(deviceName + "_WARN_H4_High_SV_" + H4SV + "_PV_" + H4);
          } else if (H4 < H4LL.toInt()) {
            digitalWrite(r8, LOW);
            Serial.println(deviceName + "_WARN_H4_Low_SV_" + H4SV + "_PV_" + H4);
          } else if (H4 == H4SV.toInt() || H4 < H4UL.toInt() && H4 > H4LL.toInt()) {
            digitalWrite(r7, HIGH);
            digitalWrite(r8, HIGH);
          }
        }
        dataFile = SD.open("both4.csv", FILE_WRITE);
        if (dataFile) {
          dataFile.print(deviceName);
          dataFile.print(", ");
          dataFile.print(T1);
          dataFile.print(", ");
          dataFile.print(H1);
          dataFile.print(", ");
          dataFile.print(T2);
          dataFile.print(", ");
          dataFile.print(H2);
          dataFile.print(", ");
          dataFile.print(T3);
          dataFile.print(", ");
          dataFile.print(H3);
          dataFile.print(", ");
          dataFile.print(T4);
          dataFile.print(", ");
          dataFile.print(H4);
          dataFile.print(", ");
          dataFile.print(rType);
          dataFile.print(", ");
          dataFile.print(time);
          dataFile.print(", ");
          dataFile.print(date);
          dataFile.close();
        } else {
          Serial.println(deviceName + "_Error_OpenFileB4");
        }
      }
    }
  }
}

void dateTime() {
  DateTime now = rtc.now();

  Serial.print(" ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  printTwoDigits(now.month());
  Serial.print('/');
  printTwoDigits(now.day());
  Serial.print(" ");
  printTwoDigits(now.hour());
  Serial.print(':');
  printTwoDigits(now.minute());
  Serial.print(':');
  printTwoDigits(now.second());
}

void printTwoDigits(int number) {
  if (number < 10) {
    Serial.print('0');
  }
  Serial.print(number, DEC);
}


int splitString(String input, char delimiter, String output[]) {
  int count = 0;
  int lastIndex = 0;
  int length = input.length();

  for (int i = 0; i < length; i++) {
    if (input[i] == delimiter) {
      output[count] = input.substring(lastIndex, i);
      lastIndex = i + 1;
      count++;
    }
  }
  output[count] = input.substring(lastIndex);
  return count + 1;
}

float Fahrenheit(float celsius) {
  return (celsius * 9.0 / 5.0) + 32.0;
}

void writeConfigToSD() {
  dataFile = SD.open("config.txt", O_WRITE | O_CREAT | O_TRUNC);
  if (dataFile) {
    dataFile.print(deviceName + "_");
    dataFile.print(logType + "_");
    dataFile.print(NoOfSensor + "_");
    dataFile.print(rType + "_");
    dataFile.print(T1SV + "_");
    dataFile.print(H1SV + "_");
    dataFile.print(T1UL + "_");
    dataFile.print(T1LL + "_");
    dataFile.print(H1UL + "_");
    dataFile.print(H1LL + "_");
    dataFile.print(T1C + "_");
    dataFile.print(H1C + "_");
    dataFile.print(T2SV + "_");
    dataFile.print(H2SV + "_");
    dataFile.print(T2UL + "_");
    dataFile.print(T2LL + "_");
    dataFile.print(H2UL + "_");
    dataFile.print(H2LL + "_");
    dataFile.print(T2C + "_");
    dataFile.print(H2C + "_");
    dataFile.print(T3SV + "_");
    dataFile.print(H3SV + "_");
    dataFile.print(T3UL + "_");
    dataFile.print(T3LL + "_");
    dataFile.print(H3UL + "_");
    dataFile.print(H3LL + "_");
    dataFile.print(T3C + "_");
    dataFile.print(H3C + "_");
    dataFile.print(T4SV + "_");
    dataFile.print(H4SV + "_");
    dataFile.print(T4UL + "_");
    dataFile.print(T4LL + "_");
    dataFile.print(H4UL + "_");
    dataFile.print(H4LL + "_");
    dataFile.print(T4C + "_");
    dataFile.print(H4C);
    dataFile.close();
  } else {
    Serial.println("Error opening config file for writing");
  }
}

void readConfigFromSD() {
  dataFile = SD.open("config.txt", FILE_READ);
  if (dataFile) {
    String config = dataFile.readStringUntil('\n');
    splitString(config, '_', body);
    deviceName = body[0];
    logType = body[1];
    NoOfSensor = body[2];
    rType = body[3];
    T1SV = body[4];
    H1SV = body[5];
    T1UL = body[6];
    T1LL = body[7];
    H1UL = body[8];
    H1LL = body[9];
    T1C = body[10];
    H1C = body[11];
    T2SV = body[12];
    H2SV = body[13];
    T2UL = body[14];
    T2LL = body[15];
    H2UL = body[16];
    H2LL = body[17];
    T2C = body[18];
    H2C = body[19];
    T3SV = body[20];
    H3SV = body[21];
    T3UL = body[22];
    T3LL = body[23];
    H3UL = body[24];
    H3LL = body[25];
    T3C = body[26];
    H3C = body[27];
    T4SV = body[28];
    H4SV = body[29];
    T4UL = body[30];
    T4LL = body[31];
    H4UL = body[32];
    H4LL = body[33];
    T4C = body[34];
    H4C = body[35];
    dataFile.close();
  } else {
    Serial.println("Error opening config file");
  }
}

String trim(String str) {
  int startIdx = 0;
  while (str.charAt(startIdx) == ' ' || str.charAt(startIdx) == '\t') {
    startIdx++;
  }
  int endIdx = str.length() - 1;
  while (endIdx >= 0 && (str.charAt(endIdx) == ' ' || str.charAt(endIdx) == '\t')) {
    endIdx--;
  }
  return str.substring(startIdx, endIdx + 1);
}