#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_deep_sleep.h"
#include <SimpleDHT.h>          //โหลดไลบารี่ DHT11

//########## CONFIGURATION : MODIFY HERE ##########

char ssid[] = "plutter";       //ขื่อวายฟาย SSID
char pass[] = "visit012";      //รหัสผ่านวายฟาย

#define TokenLine "0kWBLPCHq5MYSuhdF8WMyVoRGnkrSfH9YQdhr4s9pHP" //รหัสเชื่อมต่อ Line
#define SWPIN 5                //กำหนดขาสวิตซ์ที่เชื่อมต่อกับสวิตซ์
int DHTSENSOR = 15;            //GPIO15 (ADC2_CH3)

//#################################################

SimpleDHT11 dht11;              //ระบุรุ่นเซ็นเซอร์รุ่น DHT11
unsigned long last1, last2, last3;

void setup() {

  Serial.println("Wake up.");
  esp_deep_sleep_enable_timer_wakeup(1 * 60 * 1000 * 1000); // กำหนดให้ตื่นเมื่อครบ 1 นาทีข้างหน้า
  
  pinMode(SWPIN, INPUT_PULLUP);               //กำหนดให้ขาดิจิตอล SWPIN รับข้อมูลจากสวิตซ์
  Serial.begin(115200);                       //ตั้งค่าคอนโซล
  WiFi.begin(ssid, pass);                     //เริ่มการเชื่อมต่อกับวายฟายแม่ข่าย
  Serial.print("WiFi Connecting");            //แสดง "WiFi Connecting" ในคอนโซล

//ตรวจเช็คสถานะการเขื่อมต่อวายฟาย ถ้าไม่สำเร็จให้แสดง "." ในคอนโซลจนกว่าจะเขื่อมต่อได้
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  //แสดงสถานะการวายฟายเชื่อต่อแล้ว และแสดงหมายเลขไอพีที่ได้รับในคอนโซล
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  //cal linenotify
  NotifyLine();
  
}

void loop() {
      
}

void NotifyLine() {

  byte temperature = 0;           //กำหนดตัวแปรเก็บค่าอุณหภูมิ
  byte humidity = 0;              //กำหนดตัวแปรเก็บค่าความชื้นสัมสัทธ์

  dht11.read(DHTSENSOR, &temperature, &humidity, NULL);  //อ่านค่าจากเซ็นเซอร์
  while (temperature == 0) {
    dht11.read(DHTSENSOR, &temperature, &humidity, NULL);  //อ่านค่าจากเซ็นเซอร์
    Serial.println("DHT11 get failed");
    delay(1500);
  }

  String t;
  t = "Temperature: " + String(temperature) + "C " + "| Humidity: " + String(humidity) + "RH";
  
  WiFiClientSecure client;

  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("Connection failed");
    return;   
  }

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(TokenLine) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + t).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + t;
  
  Serial.println(req);
  client.print(req);
  delay(20);
  Serial.println("-------------");
  while(client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  
  Serial.println(t);
  Serial.println("Go to sleep mode.");
  esp_deep_sleep_start(); // เริ่มเข้าสู่โหมด Deep Sleep
}
