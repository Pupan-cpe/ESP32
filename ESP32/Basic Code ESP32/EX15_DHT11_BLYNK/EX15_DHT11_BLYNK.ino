#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <SimpleDHT.h>          //โหลดไลบารี่ DHT11

char auth[] = "d559f27024684e7cae0b79a18cc9733a";
char ssid[] = "plutter";
char pass[] = "visit012";

int LEDPIN = 4;                 //GPIO4 LED
int DHTSENSOR = 15;             //GPIO15 (ADC2_CH3)

SimpleDHT11 dht11;              //ระบุรุ่นเซ็นเซอร์รุ่น DHT11

BlynkTimer timer;

void sendSensor() {

  byte temperature = 0;           //กำหนดตัวแปรเก็บค่าอุณหภูมิ
  byte humidity = 0;              //กำหนดตัวแปรเก็บค่าความชื้นสัมสัทธ์

  dht11.read(DHTSENSOR, &temperature, &humidity, NULL);  //อ่านค่าจากเซ็นเซอร์
  while (temperature == 0) {
    dht11.read(DHTSENSOR, &temperature, &humidity, NULL);  //อ่านค่าจากเซ็นเซอร์
    Serial.println("DHT11 get failed");
    delay(1500);
  }
  
  Blynk.virtualWrite(V1, humidity);     //ส่งค่า humidity V1 ไปยัง blynk server
  Blynk.virtualWrite(V2, temperature);  //ส่งค่า humitemperature V2 ไปยัง blynk server
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("% | ");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("C");

}

void setup() {

  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, IPAddress(128,199,204,127), 8080);
  timer.setInterval(5000L, sendSensor);
  
}

void loop() {
  Blynk.run();
  timer.run();
}
