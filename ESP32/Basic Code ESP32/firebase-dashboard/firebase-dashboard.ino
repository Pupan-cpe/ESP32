#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
#include <SimpleDHT.h> 
#include <time.h>
#include "esp_deep_sleep.h"

/* ############################################################### */

// Config Firebase
#define FIREBASE_HOST "smtled-f6bc8.firebaseio.com"
#define FIREBASE_AUTH "u7j7XhFN57gnDpmIfUrE5bj9tiy4RVKm3bnNHAQc"
#define WIFI_SSID "plutter"
#define WIFI_PASSWORD "visit012"

/* ############################################################### */

int DHTPIN = 32;
int LDRPIN = 35;
SimpleDHT11 dht11;              //ระบุรุ่นเซ็นเซอร์รุ่น DHT11
byte temperature = 0;           //กำหนดตัวแปรเก็บค่าอุณหภูมิ
byte humidity = 0;              //กำหนดตัวแปรเก็บค่าความชื้นสัมสัทธ์

char ntp_server1[20] = "time.navy.mi.th";
char ntp_server2[20] = "clock.nectec.or.th";
char ntp_server3[20] = "th.pool.ntp.org";

RTC_DATA_ATTR int bootCount = 0; //เก็บจำนวนการบูตแต่ละครั้ง

void setup() {
  Serial.begin(115200);

  esp_deep_sleep_enable_timer_wakeup(5 * 60 * 1000 * 1000); // กำหนดให้ตื่นเมื่อครบ 5 นาทีข้างหน้า

  WiFi.mode(WIFI_STA);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("Waiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Now: " + NowString());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  bootCount++;  //เพิ่มจำนวนการบูตแต่ละครั้ง
  
}

void loop() {

  int ldrvalue = analogRead(LDRPIN);
  
  dht11.read(DHTPIN, &temperature, &humidity, NULL);  //อ่านค่าจากเซ็นเซอร์
  while (temperature == 0) {
    dht11.read(DHTPIN, &temperature, &humidity, NULL);  //อ่านค่าจากเซ็นเซอร์
    Serial.println("DHT11 get failed");
    delay(1500);
  }

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" *C ");
  Serial.print("ldr: ");
  Serial.print(ldrvalue);
  Serial.println();

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["temperature"] = temperature;
  root["humidity"] = humidity;
  root["time"] = NowString();
  root["ldr"] = ldrvalue;

// append a new value to /logDHT
  String name = Firebase.push("DHT11Data", root);
  
// handle error
  if (Firebase.failed()) {
      Serial.print("pushing /logDHT failed:");
      Serial.println(Firebase.error());  
      return;
  }
  
  Serial.print("pushed: /logDHT/");
  Serial.println(name);
  
  delay(1000);
  Serial.println("Sleep mode");
  esp_deep_sleep_start(); // เริ่มเข้าสู่โหมด Deep Sleep
}

String NowString() {
  
  time_t now = time(nullptr);
  struct tm* newtime = localtime(&now);
  String myyear = String(newtime->tm_year + 1900);

  //some time get problem
  while (myyear == "1970") {
    time_t now = time(nullptr);
    struct tm* newtime = localtime(&now);
    myyear = String(newtime->tm_year + 1900);
    delay(1500);
  }
  
  String tmpNow = "";
  tmpNow += String(newtime->tm_year + 1900);
  tmpNow += "-";
  tmpNow += String(newtime->tm_mon + 1);
  tmpNow += "-";
  tmpNow += String(newtime->tm_mday);
  tmpNow += " ";
  tmpNow += String(newtime->tm_hour);
  tmpNow += ":";
  tmpNow += String(newtime->tm_min);
  tmpNow += ":";
  tmpNow += String(newtime->tm_sec);
  return tmpNow;
}
