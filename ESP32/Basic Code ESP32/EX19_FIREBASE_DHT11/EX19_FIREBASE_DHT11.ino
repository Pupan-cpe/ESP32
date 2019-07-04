#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
#include <SimpleDHT.h> 
#include <time.h>

/* ############################################################### */

// Config Firebase
#define FIREBASE_HOST "projectx-23875.firebaseio.com"
#define FIREBASE_AUTH "wZz7qarwgld9YY4i1NUa8Hpbaor3wah9pIeIWu2V"
#define WIFI_SSID "Kerdsawas 2.4G"
#define WIFI_PASSWORD "aoyaomaum"

/* ############################################################### */

int DHTPIN = 15;
SimpleDHT11 dht11;              //ระบุรุ่นเซ็นเซอร์รุ่น DHT11
byte temperature = 0;           //กำหนดตัวแปรเก็บค่าอุณหภูมิ
byte humidity = 0;              //กำหนดตัวแปรเก็บค่าความชื้นสัมสัทธ์

char ntp_server1[20] = "time.navy.mi.th";
char ntp_server2[20] = "clock.nectec.or.th";
char ntp_server3[20] = "th.pool.ntp.org";

void setup() {
  Serial.begin(115200);

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
  
}

void loop() {
  
  dht11.read(DHTPIN, &temperature, &humidity, NULL);  //อ่านค่าจากเซ็นเซอร์

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" *C ");
  Serial.println();

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["temperature"] = temperature;
  root["humidity"] = humidity;
  root["time"] = NowString();

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
  
  delay(5000);
}

String NowString() {
  time_t now = time(nullptr);
  struct tm* newtime = localtime(&now);

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
