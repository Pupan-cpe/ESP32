#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define led D0
#define D1 10
#include <SoftwareSerial.h>
SoftwareSerial NodeSerial(D2, D3); // RX | TX
char auth[] = "gqg9UeoF60NG4tzGX8bY8_6Fi9C6bA3l"; //Token key
//char server[] = "oasiskit.com";
char ssid[] = "Ais@wifi";
char pass[] = "0890546835";
int port = 8080;

String text_In;

void setup() {
  Serial.begin(115200);
  NodeSerial.begin(4800);
  Serial.println("กำลังเชื่อมต่อไปที่ Blynk Server");
  pinMode(led, OUTPUT); //กำหนดโหมด ว่าเป็น INPUT หรือ OUTPUT
  pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);

  pinMode(D1, OUTPUT);
  //WiFi.begin(ssid, pass); //เชื่อมต่อ WiFi
  Blynk.begin(auth, ssid, pass);
  //Blynk.config(auth, server, port); //กำหนด Token key , ชื่อ Server และ port
  //Blynk.connect(); //เชื่อมต่อไปยัง Blynk
}

BLYNK_CONNECTED() {  // ฟังก์ชันนี้ทำงานเมื่อต่อ Blynk ได้
  Serial.println("App Blynk ทำงาน!");
}

BLYNK_WRITE(V0) {
  text_In = param.asStr();  // Text Input Widget - Strings
  Blynk.virtualWrite(V1, text_In);

}



void loop() {

  Blynk.run();

  NodeSerial.print(text_In);
  NodeSerial.print("\n");
  delay(1000);
  
  // digitalWrite(D0, HIGH);
 
  if (text_In == "R3")
  {
    digitalWrite(led, HIGH);
    Serial.println(text_In);
  }
  if (text_In == "R1")
  {
    digitalWrite(led, LOW);
    Serial.println(text_In);
  }

}
