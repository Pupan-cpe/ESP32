#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>

/* ############################################################### */

// Config Firebase
#define FIREBASE_HOST "projectx-23875.firebaseio.com"
#define FIREBASE_AUTH "wZz7qarwgld9YY4i1NUa8Hpbaor3wah9pIeIWu2V"
#define WIFI_SSID "Kerdsawas 2.4G"
#define WIFI_PASSWORD "aoyaomaum"

/* ############################################################### */

int LEDPIN = 4;                             //GPIO4 LED                           
int SWPIN = 18;                             //GPIO15 (ADC2_CH3)
boolean state = 0;                          //กำหนดค่าตัวแปร state เท่ากับ 0 เป็นค่าเร่มต้น (สถานะปิด)
boolean oldState;                           //กำหนดค่าตัวแปรเป็นตัวเลข 0 กับ 1
boolean data;                               //กำหนดค่าตัวแปรเป็นตัวเลข 0 กับ 1

void setup() {
  pinMode(LEDPIN, OUTPUT);                  //กำหนดขาเชื่อมต่อ LED
  pinMode(SWPIN, INPUT_PULLUP);             //กำหนดขาดิจิตอล SWPIN เป็นขาอ่านข้อมูลการกดสวิตซ์ 
  
  Serial.begin(115200);

  //เชื่อมต่อ WIFI
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  //เชื่อมต่อ Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  Serial.println("Start stream led control");
  Firebase.stream("/LED1", [](FirebaseStream stream) {
    Serial.println("Event: " + stream.getEvent());
    Serial.println("Path: " + stream.getPath());
    Serial.println("Data: " + stream.getDataString());
    if (stream.getEvent() == "put") {
      if (stream.getPath() == "/") {
        digitalWrite(LEDPIN, stream.getDataInt());
      } //if
    } //if
  }); //firebase
}

void loop() {
  
  data = digitalRead(SWPIN);                //อ่านค่าข้อมูลขาดิจิตอล SWPIN ในรูปแบบตัวเลข 0 และ 1 (0 คือการกดสวิตซ์)
  if (data == 0 and oldState == 1) {        //ถ้ามีการกดสวิตซ์ มีค่าที่อ่านได้เท่ากับ 0 และมีการกดเพียงครั้งเดียว มีค่าที่อ่านได้ก่อนหน้าเท่ากับ 1
    delay (10);                             //รอ 10 มิลลิวินาที เพื่อป้องกันการอ่านค่าผิดพลาด ที่เกิดขั้นในระหว่างการเกิดผิวหน้าสัมผัสตอนกดสวิตย์
      if (digitalRead(SWPIN) == 0) {        //อ่านค่าข้อมูลขาดิจิตอล SWPIN อีกครั้งถ้าพบว่าค่ายังเท่ากับ 0 แสดงว่ามีการกดสวิตซ์จริง
        Serial.print("SW current state: "); //แสดงข้อความในคอนโซล
        Serial.println(!state);             //แสดงสถานะ state ในคอนโซล
        state = !state;                     //กำหนดตัวแปร state ให้มีค่าตรงข้ามกับค่าปัจจุบัน (0 คือปิดไฟ และ 1 คือเปิดไฟ)
        Firebase.setInt("/LED1", state);    //อัพเดทข้อมูลบนสถานะของ "LED1" บน Firebase
      }//if
  }//if
  
    oldState = data;                        //กำหนดตัวแปร oldState เท่ากับค่าที่อ่านได้จากข้อมูลการกดสวิตซ์ปัจจุบัน
}
