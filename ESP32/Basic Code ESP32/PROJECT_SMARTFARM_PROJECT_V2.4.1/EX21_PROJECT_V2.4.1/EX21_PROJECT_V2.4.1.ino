/* 
 * Demo SMARTFARM PROJECT
 * VERSION: 2.4.1 (6 May 2018)
 * BY     : VISIT WIANGNAK
 * EMAIL  : VISIT01@GMAIL.COM
 * 
 * MCU    : ESP32(ESP-WROOM-32)
 * 
 * Output : Wifi LED(2:HIGH=ON,LOW=OFF)
 *        : Relay0(LOW=ON,HIGH=OFF)
 * 
 * Demo   : BLYNK -> "SMARTFARM"
 *        : Blynk library V0.5.0
 *        : DHT11 (Temperature, Humidity)
 *        : LDR (Light sensor)
 *        : SOIL sensor
 *        : Relay
 *        
 * Widget : Widget_Humidity V1            
 *        : Widget_Temperature V2        
 *        : Widget_SoilValue V3           
 *        : Widget_SoilLevel V4          
 *        : Widget_LightValue V5        
 *        : Widget_LightLevel V6         
 *        : Widget_RelayControl V7      
 *        : Widget_LedStatus V8          
 *        : Widget_RelayStatus V9        
 *        : Widget_WifiSignal V10
 *        : Widget_WifiValue V11
 *        : Widget_Terminal V12
 */
/* ############################################################################### */

/* ############################################################################### */
// *** CONFIG HERE ***

//WIFI CONFIG
char ssid[] = "plutter";
char pass[] = "visit012";

//BLYNK TOKEN
char auth[] = "589a20ebd2b043f5b3e57e4378340a06";
char server[] = "128.199.204.127";
unsigned int port = 8080;

//LINE TOKEN
#define TokenLine "Ynzzovc8dway85pRSJKNvtUIAzlkPnvBPs9MUpaBWRA"

//WATER INJECT CONFIG
int PercentWaterInject = 40;  //ถ้าค่าความชื้นดินน้อยกว่า 40% จะเริ่มรดน้ำ
int TimeWaterInject = 10000;   //ตั้งเวลารดน้ำ 10 วินาที
int TimeSoilCheck = 60000;    //เช็คความชื้นดินทุกๆ 1 นาที

//FIREBASE CONFIG
#define FIREBASE_HOST "smtled-f6bc8.firebaseio.com"
#define FIREBASE_AUTH "u7j7XhFN57gnDpmIfUrE5bj9tiy4RVKm3bnNHAQc"

/* ############################################################################### */

/* ############################################################################### */
//INCLUDE LIBRARY
//#define BLYNK_DEBUG // Optional, this enables lots of prints
#define BLYNK_MAX_READBYTES 1024
#define BLYNK_TIMEOUT_MS  750  // Special setting
#define BLYNK_HEARTBEAT   17   // Special setting
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp32.h>
#include <SimpleDHT.h>
#include <IOXhop_FirebaseESP32.h>
#include <SimpleDHT.h> 
#include <time.h>

/* ############################################################################### */
//ESP32 PIN ASSIGNMENT

#define DHTPIN 32                //Humidity , Temperature
#define SOILSENSOR 34            //Soil sensor         
#define LDRSENSOR 35             //Light sensor           
#define SWPIN 25                 //Swith
#define LEDSTATUS1 26            //LED Status1
#define RELAYPIN 27              //Relay Control
#define LEDBUILIN 2              //LED Builin
#define LEDSTATUS2 14            //LED Status2
#define INJECTPIN 5              //MONITOR WATER INJECT PIN

/* ############################################################################### */
//Blynk Vitural PIN Assignment

#define Widget_Humidity V1            
#define Widget_Temperature V2        
#define Widget_SoilValue V3           
#define Widget_SoilLevel V4          
#define Widget_LightValue V5        
#define Widget_LightLevel V6         
#define Widget_RelayControl V7      
#define Widget_LedStatus V8          
#define Widget_RelayStatus V9        
#define Widget_WifiSignal V10
#define Widget_WifiValue V11
#define Widget_Terminal V12

/* ############################################################################### */
//Blynk Widget

WidgetLED ledStatus(Widget_LedStatus);
WidgetLED ledRelay(Widget_RelayStatus);
WidgetTerminal terminal(Widget_Terminal);

/* ############################################################################### */
//NTP TIME SERVER

char ntp_server1[20] = "time.navy.mi.th";
char ntp_server2[20] = "clock.nectec.or.th";
char ntp_server3[20] = "th.pool.ntp.org";
  
/* ############################################################################### */
//ระบุรุ่นเซ็นเซอร์วัดความชื้นสัมพัทธ์และอุณมหภูมิรุ่น DHT11

SimpleDHT11 dht11;              

/* ############################################################################### */
//เรียกใช้การตั้งเวลาของ Blynk

BlynkTimer timer;

/* ############################################################################### */
//หลังจากที่เชื่อมต่อกับ Blynk server สำเร็จ

int WifiSignal;
bool isFirstConnect = true;

BLYNK_CONNECTED() {

  //ให้หลอด LED บนบอร์ดติด (สีน้ำเงิน) เพื่อแสดงสถานะว่าเชื่อมต่อ WiFi แล้ว
  digitalWrite(LEDBUILIN, HIGH);

  //ถ้าเป็นการเชื่อมต่อ Blynk Server ครั้งแรก
  if(isFirstConnect)
  {
    //ซิงค์ค่าข้อมูลของทุก Pin จาก Blynk Server
    Blynk.syncAll();  
    
    //ซิงค์สถานะสุดท้ายของ Relay
    Blynk.syncVirtual(Widget_RelayControl);  
                                                       
    isFirstConnect = false;
  }

}

/* ############################################################################### */
//สร้างการทำงานของ Widget_RelayControl (V7) ซึ่งเป็นปุ่มกดบน blynk application 


BLYNK_WRITE(Widget_RelayControl)
{
  //ค้าที่ได้จาก Widget_RelayControl (V7) จะเป็น 0 และ 1
  //ถ้ามีการปดปุ่ม

  if (param.asInt() == 1) {
    
      //ป้องกันไม่ให้กดซ้ำ
      if (digitalRead(INJECTPIN) != 0) {
        
          //เรียกใช้ฟังก์ชันรดน้ำ
          digitalWrite(INJECTPIN, 0);
          
          //Update blynk pin
          //Blynk.virtualWrite(Widget_RelayControl, 0);
      } 
      
  } //if
}

/* ############################################################################### */

void setup() {

  /* ------------------------------------------------------------------------------- */ 

  //กำหนดโหมดใช้งาน
  pinMode(LEDBUILIN, OUTPUT);
  pinMode(LEDSTATUS1, OUTPUT);
  pinMode(RELAYPIN, OUTPUT);
  pinMode(LEDSTATUS2, OUTPUT);
  pinMode(INJECTPIN, OUTPUT);
  pinMode(SWPIN, INPUT_PULLUP);  

  //ตั้งสถานะเริ่มต้น เพื่อป้องกันการทำงานเองตอนบูต
  digitalWrite(RELAYPIN, HIGH);
  digitalWrite(LEDSTATUS1, LOW);
  digitalWrite(LEDBUILIN, LOW);
  digitalWrite(INJECTPIN, HIGH);

  Serial.begin(115200);
  
/* ------------------------------------------------------------------------------- */ 
// CREATE MULTITASK

    //TASK1: เช็คค่าความชื้นดินและรดน้ำต้นไม้
    xTaskCreate(&Task1, "Task1", 3000, NULL, 9, NULL);

    //TASK2: รออ่านค่า switch ถ้ากดปุ่มให้รดน้ำต้นไม้
    xTaskCreate(&Task2, "Task2", 2000, NULL, 9, NULL);

    //TASK3: สร้างไฟ LED กระพริบ ถ้ามีการรดน้ำต้นไม้
    xTaskCreate(&Task3, "Task3", 4000, NULL, 8, NULL);

    //TASK4: ลูปเช็คการเชื่อมต่อม wifi
    xTaskCreate(&Task4, "Task4", 4000, NULL, 10, NULL);

    //TASK5: รดน้ำต้นไม้ water inject
    xTaskCreate(&Task5, "Task5", 7000, NULL, 9, NULL);

/* ------------------------------------------------------------------------------- */ 
  //เชื่อมต่อ WiFi

  Blynk.connectWiFi(ssid, pass);

/* ------------------------------------------------------------------------------- */ 

  //ตั้งเวลาส่งข้อมูลเข้า Blynk ทุกๆ 30 วินาทีสำหรับทดสอบ (ใช้งานจริงอาจตั้งมากกว่านี้ก็ได้)
  //30 Secound = 3000 Milliseconds
   timer.setInterval(30000L, sendSensor);

 //ตั้งเวลาส่งข้อมูลเข้า Blynk ทุกๆ  1 นาทีสำหรับทดสอบ (ใช้งานจริงอาจตั้งมากกว่านี้ก็ได้)
  //60 Secound = 60000 Milliseconds
   timer.setInterval(60000L, sendSensor2);

  //ตั้งเวลาให้เช็คว่า Blynk เชื่อมต่ออยู่หรือไม่ทุกๆ 10 วินาที
  timer.setInterval(10000L, CheckConnection);

  //ส่งข้อมูลเข้า firebase ทุกๆ 5 นาที
  timer.setInterval(300000L, sendFirebase);

/* ------------------------------------------------------------------------------- */ 
  //เชื่อมต่อ Firebase

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); //Firebase connecting
  
/* ------------------------------------------------------------------------------- */ 
  //คำนวณสัญญาณ WiFi
  
  WifiSignal = WiFi.RSSI();

  //แสดงข้อความบนคอนโซล
  Serial.println();
  Serial.println("Welcom to smartfram IoT");
  Serial.print("WiFi Connected: ");
  Serial.println(WiFi.localIP());
  Serial.print("WiFi signal RSSI: ");
  Serial.print(WifiSignal);
  Serial.println("dBm ");

  //แสดงข้อความบน blynk terminal
  //terminal.println();
  //terminal.println("Welcom to smartfram IoT");
  //terminal.print("WiFi Connected: ");
  //terminal.println(WiFi.localIP());
  //terminal.print("WiFi signal RSSI: ");
  //terminal.print(WifiSignal);
  //terminal.println("dBm ");

/* ------------------------------------------------------------------------------- */ 
  //เชื่อมต่อ NTP time server
  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3);
  
  Serial.println("Waiting for time.");
  
  //แสดงข้อความบน blynk terminal
  //terminal.println("Waiting for time.");
  
  while (!time(nullptr)) {
    Serial.print(".");
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }

  //แสดงข้อความบนคอนโซล
  Serial.println();
  Serial.println("Current time: " + NowString());

  //แสดงข้อความบน blynk terminal
  //terminal.println("Current time: " + NowString());

/* ------------------------------------------------------------------------------- */
  //เชื่อมต่อ blynk serve
  
  Blynk.config(auth, server, port);
  Blynk.connect();  
  
 /* ------------------------------------------------------------------------------- */
 
}

/* ############################################################################### */

void loop() {
  
  Blynk.run();
  timer.run();

}

/* ############################################################################### */
/* ############################################################################### */

byte oldtemperature;
byte oldhumidity;
  
void sendSensor() {

  //ถ้ามีการเรียกใช้ฟังก์ชันให้หลอด LED ติด
  digitalWrite(LEDSTATUS1, HIGH);

  //ให้หลอด LED บน app Blynk ติด
  ledStatus.on();

  //แสดงข้อความบนคอนโซล
  Serial.print(NowString());
  Serial.println(", Send data to blynk server");

/* ------------------------------------------------------------------------------- */

  byte temperature = 0;           //กำหนดค่าตัวแปรอุณหภูมิ
  byte humidity = 0;              //กำหนดค่าความชื้นสัมสัทธ์
  int count = 1;

  //อ่านค่าจากเซ็นเซอร์ DHT11
  dht11.read(DHTPIN, &temperature, &humidity, NULL);
      
  //ค่าค่าอุณหภูมิ = 0 ให้อ่านค่าจากเซ็นเซอร์ใหม่ 3 ครั้ง
  while (temperature == 0 && count <= 3) {

    //แสดงข้อความบนคอนโซล
    Serial.print(NowString());    
    Serial.println(", DHT11 get failed ");
    Serial.print(count);

    //อ่านค่าจากเซ็นเซอร์
    dht11.read(DHTPIN, &temperature, &humidity, NULL);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    count++;
  }
  
    if(temperature == 0) temperature = oldtemperature;
    if (humidity == 0) humidity = oldhumidity;

    //แสดงข้อความบนคอนโซล
    Serial.print(NowString());
    Serial.print(", Humidity: ");
    Serial.print(humidity);
    Serial.print("% | ");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("C");

/* ------------------------------------------------------------------------------- */
  //อ่านค่าจากเซ็นเซอร์อนาล๊อก Soil
  
  int CurrentSoilValue = analogRead(SOILSENSOR);
  
  if (CurrentSoilValue > 4000) CurrentSoilValue = 4000;
  if (CurrentSoilValue < 800) CurrentSoilValue = 800;
 
  int MapReadSoilValue = map(CurrentSoilValue, 800, 4000, 100, 0);

  //แสดงข้อความบนคอนโซล
  Serial.print(NowString());
  Serial.print(", Soil sensor value: ");
  Serial.println(MapReadSoilValue);

/* ------------------------------------------------------------------------------- */
  //อัพเดทข้อมูลไปยัง Blynk server 
  
  Blynk.virtualWrite(Widget_Humidity, humidity);
  Blynk.virtualWrite(Widget_Temperature, temperature);
  Blynk.virtualWrite(Widget_SoilValue, analogRead(SOILSENSOR));
  Blynk.virtualWrite(Widget_SoilLevel, MapReadSoilValue);
  
/* ------------------------------------------------------------------------------- */ 

   //แสดงข้อความบนคอนโซล
   Serial.print(NowString());
   Serial.println(", Send DHT11/Soil sensors to blynk server.");

 /* ------------------------------------------------------------------------------- */ 

   //backup temp & hum
   oldtemperature = temperature;
   oldhumidity = humidity;
   
   //ให้หลอด LED ที่แสดงสถานะดับ
   digitalWrite(LEDSTATUS1, LOW);

   //ให้หลอด LED บน app Blynk ดับ
   ledStatus.off();
}

/* ############################################################################### */

void sendSensor2() {

  //ถ้ามีการเรียกใช้ฟังก์ชันให้หลอด LED ติด
  digitalWrite(LEDSTATUS1, HIGH);

  //ให้หลอด LED บน app Blynk ติด
  ledStatus.on();

  //แสดงข้อความบนคอนโซล
  Serial.print(NowString());
  Serial.println(", Send data to blynk server");
  
/* ------------------------------------------------------------------------------- */

  //หาค่าสัญญาณ RSSI ของ WIFI แล้ว MAP ให้อยู่ในรูปของเปอร์เซ็นต์
  int CurrentWiFiSignal = WiFi.RSSI();
  
  if (CurrentWiFiSignal > -40) CurrentWiFiSignal = -40;
  if (CurrentWiFiSignal < -90) CurrentWiFiSignal = -90;

  WifiSignal = map(CurrentWiFiSignal, -40, -90, 0, 100);

  //แสดงข้อความบนคอนโซล
  Serial.print(NowString());
  Serial.print(", Wifi Signal: ");
  Serial.print(WifiSignal);
  Serial.println("%");

/* ------------------------------------------------------------------------------- */
  //อ่านค่าจากเซ็นเซอร์อนาล๊อก LDR

  int CurrentLightValue = analogRead(LDRSENSOR);
  
  if (CurrentLightValue > 4000) CurrentLightValue = 4000;
  if (CurrentLightValue < 400) CurrentLightValue = 400;
 
  int MapReadLightValue = map(CurrentLightValue, 400, 4000, 100, 0);

  //แสดงข้อความบนคอนโซล
  Serial.print(NowString());
  Serial.print(", Light sensor value: ");
  Serial.println(MapReadLightValue); 
 
/* ------------------------------------------------------------------------------- */
  //อัพเดทข้อมูลไปยัง Blynk server 
  
  Blynk.virtualWrite(Widget_LightValue, analogRead(LDRSENSOR));
  Blynk.virtualWrite(Widget_LightLevel, MapReadLightValue);
  Blynk.virtualWrite(Widget_WifiSignal, WifiSignal);
  Blynk.virtualWrite(Widget_WifiValue, WiFi.RSSI());
  
/* ------------------------------------------------------------------------------- */ 

   //แสดงข้อความบนคอนโซล
   Serial.print(NowString());
   Serial.println(", Send LDR sensors / WIFI to blynk server.");

 /* ------------------------------------------------------------------------------- */ 
   
   //ให้หลอด LED ที่แสดงสถานะดับ
   digitalWrite(LEDSTATUS1, LOW);

   //ให้หลอด LED บน app Blynk ดับ
   ledStatus.off();
}

/* ############################################################################### */

void sendFirebase() {
  
/* ------------------------------------------------------------------------------- */

  //หาค่าสัญญาณ RSSI ของ WIFI แล้ว MAP ให้อยู่ในรูปของเปอร์เซ็นต์
  int CurrentWiFiSignal = WiFi.RSSI();
  
  if (CurrentWiFiSignal > -30) CurrentWiFiSignal = -30;
  if (CurrentWiFiSignal < -90) CurrentWiFiSignal = -90;

  WifiSignal = map(CurrentWiFiSignal, -90, -30, 0, 100);

/* ------------------------------------------------------------------------------- */

  byte temperature = 0;           //กำหนดค่าตัวแปรอุณหภูมิ
  byte humidity = 0;              //กำหนดค่าความชื้นสัมสัทธ์
  int count = 1;

/* ------------------------------------------------------------------------------- */

  //อ่านค่าจากเซ็นเซอร์ DHT11
  dht11.read(DHTPIN, &temperature, &humidity, NULL);
      
  //ค่าค่าอุณหภูมิ = 0 ให้อ่านค่าจากเซ็นเซอร์ใหม่ 3 ครั้ง
  while (temperature == 0 && count <= 3) {

    //อ่านค่าจากเซ็นเซอร์
    dht11.read(DHTPIN, &temperature, &humidity, NULL);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    count++;
  }
    if(temperature == 0) temperature = oldtemperature;
    if (humidity == 0) humidity = oldhumidity;

/* ------------------------------------------------------------------------------- */
  //อ่านค่าจากเซ็นเซอร์อนาล๊อก Soil
  
  int CurrentSoilValue = analogRead(SOILSENSOR);
  
  if (CurrentSoilValue > 4000) CurrentSoilValue = 4000;
  if (CurrentSoilValue < 900) CurrentSoilValue = 900;
 
  int MapReadSoilValue = map(CurrentSoilValue, 900, 4000, 100, 0);

/* ------------------------------------------------------------------------------- */
  //อ่านค่าจากเซ็นเซอร์อนาล๊อก LDR

  int CurrentLightValue = analogRead(LDRSENSOR);
  
  if (CurrentLightValue > 4000) CurrentLightValue = 4000;
  if (CurrentLightValue < 400) CurrentLightValue = 400;
 
  int MapReadLightValue = map(CurrentLightValue, 400, 4000, 100, 0);

/* ------------------------------------------------------------------------------- */ 
//อัพเดทข้อมูลให้ Firebase ในรูปแบบ JSON

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["temperature"] = temperature;
  root["humidity"] = humidity;
  root["time"] = NowString();
  root["ldr"] = MapReadLightValue;
  root["soil"] = MapReadSoilValue;
  root["wifi"] = WifiSignal;

/* ------------------------------------------------------------------------------- */ 

// ส่งข้อมูลใหม่เป็นเก็บใน Firebase /SmartFarm
  String name = Firebase.push("SmartFarm", root);

/* ------------------------------------------------------------------------------- */ 
// ถ้ามี error

  if (Firebase.failed()) {

      //แสดงข้อความบนคอนโซล
      Serial.print(NowString());
      Serial.print(", Firebase pushing /SmartFarm failed:");
      Serial.println(Firebase.error());      
      return;
  }

/* ------------------------------------------------------------------------------- */ 
  //แสดงข้อความบนคอนโซล
  Serial.print(NowString());
  Serial.println(", Firebase connected.");
  Serial.print(NowString());
  Serial.print(", Firebase pushed: /SmartFarm/");
  Serial.println(name);

 /* ------------------------------------------------------------------------------- */ 

   //backup temp & hum
   oldtemperature = temperature;
   oldhumidity = humidity;
}

/* ############################################################################### */
//เช็คการเชือมต่อของ blynk

void CheckConnection(){

   //ถ้าไม่สามารถ connect ไปยัง blynk server ได้
  if(!Blynk.connected()){

    //แสดงข้อความบนคอนโซล
    Serial.println("Not connected to Blynk server.");
    Serial.println("Connecting to Blynk server...");
    
    Blynk.connect();  // try to connect to server with default timeout
  }
  
}

/* ############################################################################### */

void NotifyLine(String t) {
  
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
  vTaskDelay(20 / portTICK_PERIOD_MS);
  Serial.println("-------------");
  while(client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }

}

/* ############################################################################### */

String NowString() {

  int getcount = 1;
  time_t now = time(nullptr);
  struct tm* newtime = localtime(&now);
  String myyear = String(newtime->tm_year + 1900);

  //ถ้าปียังเป็นปี 1970 ให้ดึงค่าเวลาใหม่ พยายามสูงสุด 4 ครั้ง
  while (myyear == "1970" && getcount <= 4) {
    
    time_t now = time(nullptr);
    struct tm* newtime = localtime(&now);
    myyear = String(newtime->tm_year + 1900);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    
    getcount++;
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

/* ############################################################################### */
/* 
 *  MULTITASK
 *  TASK1: ลูปเช็คค่าความชื้นดิน ถ้าน้อยกว่าเปอร์เซ็นต์ที่ตั้งไว้ ให้เปิดปั๋มรดน้ำตามเวลาที่กำหนด
 *  
/* ------------------------------------------------------------------------------- */

void Task1(void *p) {
  
  while(1) {
    
     int ReadSoilValue = analogRead(SOILSENSOR);         //อ่านค่าความชื้นดิน

     if (ReadSoilValue > 4095) ReadSoilValue = 4095;
     if (ReadSoilValue < 1000) ReadSoilValue = 1000;
 
     int MapReadSoilValue = map(ReadSoilValue, 1000, 4095, 100, 0);

    //แสดงข้อความบนคอนโซล 
     Serial.print(NowString());
     Serial.print(", Soil value ");
     Serial.print(MapReadSoilValue);
     Serial.println("%");

    //ค่า default PercentWaterInject = 40%
    // PercentWaterInject ต้องไม่เท่ากับ 0
    if (MapReadSoilValue < PercentWaterInject and MapReadSoilValue != 0) {
      
        //เรียกใช้ฟังก์ชันรดน้ำ
        digitalWrite(INJECTPIN, 0);
      
    } //if

    //เข้าโหมดรอตามเวลาที่กำหนด
    vTaskDelay(TimeSoilCheck / portTICK_PERIOD_MS); //ตัวอย่าง default = 60000 = 1 นาที
    
  } //while
}

/* ############################################################################### */
/* 
 *  MULTITASK
 *  TASK2: ลูปเช็คสถานะของสวิตซ์ ถ้ามีการกดสวิตซ์จะเป็นการเปิดปั๊มรดน้ำ
 *  
/* ------------------------------------------------------------------------------- */
void Task2(void *p) {

  boolean state = 0;                //กำหนดค่าตัวแปร state เท่ากับ 0 เป็นค่าเร่มต้น (สถานะปิด)
  boolean oldState;                 //กำหนดค่าตัวแปรเป็นตัวเลข 0 กับ 1
  boolean data;                     //กำหนดค่าตัวแปรเป็นตัวเลข 0 กับ 1

  while(1) {
    data = digitalRead(SWPIN);                //อ่านค่าข้อมูลขาดิจิตอล SWPIN ในรูปแบบตัวเลข 0 และ 1 (0 คือการกดสวิตซ์)
  
    if (data == 0 and oldState == 1) {        //ถ้ามีการกดสวิตซ์ มีค่าที่อ่านได้เท่ากับ 0 และมีการกดเพียงครั้งเดียว มีค่าที่อ่านได้ก่อนหน้าเท่ากับ 1
      vTaskDelay(10 / portTICK_PERIOD_MS);    //รอ 10 มิลลิวินาที เพื่อป้องกันการอ่านค่าผิดพลาด ที่เกิดขั้นในระหว่างการเกิดผิวหน้าสัมผัสตอนกดสวิตย์
        if (digitalRead(SWPIN) == 0) {        //อ่านค่าข้อมูลขาดิจิตอล SWPIN อีกครั้งถ้าพบว่าค่ายังเท่ากับ 0 แสดงว่ามีการกดสวิตซ์จริง

        //เรียกใช้ฟังก์ชันรดน้ำ
        digitalWrite(INJECTPIN, 0);
          
      } //if
    } //if
    
    //เข้าโหมดรอ
    vTaskDelay(100 / portTICK_PERIOD_MS);
    oldState = data;                     //กำหนดตัวแปร oldState เท่ากับค่าที่อ่านไ
    
  } //while
} //Task2

/* ############################################################################### */
/* 
 *  MULTITASK
 *  TASK3: ลูปเช็คค่าขา relay ถ้าเปลี่ยนสถานะเป็น LOW แสดงว่าปั๊มทำงานให้หลอด LED กระพริบ
 *  
/* ------------------------------------------------------------------------------- */

void Task3(void *p) {
  
  while(1) {

    //อ่านค่าสถานะของ Relay, 0 ปั้ม่ทำงาน , 1 ปั๊มไม่ทำงาน     
    //ถ้าปั้มทำงาน
    if (digitalRead(INJECTPIN) == 0) {
      
      //............................................................................
      //ให้หลอด LED ติด
      digitalWrite(LEDSTATUS2, HIGH);
  
      //ให้หลอด LED บน app Blynk ติด
      ledStatus.on();

      //รอ .5 วินาที
      vTaskDelay(500 / portTICK_PERIOD_MS);

      //............................................................................
    } //if 

     //ให้หลอด LED ดับ
     digitalWrite(LEDSTATUS2, LOW);

     //ให้หลอด LED บน app Blynk ดับ
     ledStatus.off();

     //รอ .5 วินาที
      vTaskDelay(500 / portTICK_PERIOD_MS);
    
  } //while
}

/* ############################################################################### */
/* 
 *  MULTITASK
 *  TASK4: ลูปเช็คสัญญาณ wifi ถ้าติดต่อไม่ได้ให้ต่อใหม่
 *  
/* ------------------------------------------------------------------------------- */

void Task4(void *p) {
  
  while(1) {

      //เช็คว่าติดต่อ wifi ได้หรือไม่ ถ้าไม่ได้ ให้ reconnect ใหม่
      if(WiFi.status() != WL_CONNECTED) {
        
        digitalWrite(LEDBUILIN, LOW);
        Serial.println("WiFi Disconnected.");
                                     
        WiFi.begin((char*)ssid, (char*)pass);
        
      } else {
        digitalWrite(LEDBUILIN, HIGH);
                                                          
      }

      vTaskDelay(5000 / portTICK_PERIOD_MS);
    
  } //while
}

/* ############################################################################### */
/* 
 *  MULTITASK
 *  TASK5: รดน้ำต้นไม้
 *  
/* ------------------------------------------------------------------------------- */

void Task5(void *p) {
  
  while(1) {

    if (digitalRead(INJECTPIN) == 0) {

      //............................................................................
  
      //เปิดปั๋มรดน้ำต้นไม้
      digitalWrite(RELAYPIN, LOW);
      
      //อัพเดทสถานะของ LED Relay บน Blynk server
      ledRelay.on();
      Blynk.virtualWrite(Widget_RelayControl, 1);
        
      //แสดงข้อความบน blynk terminal
      terminal.print(NowString());
      terminal.println(", Plant watering...");
        
      //แสดงข้อความบนคอนโซล 
      Serial.print(NowString());
      Serial.println(", Plant watering...");
        
      //ส่งข้อความให้ line notify แจ้งเตือน
      NotifyLine("Plant watering...");
      
      //............................................................................
        
      //เปิดปั๊มรดน้ำตามเวลาที่กำหนด
      vTaskDelay(TimeWaterInject / portTICK_PERIOD_MS); //ตัวอย่าง default = 10000 = 10 วินาที
        
      //............................................................................
      //ปิดปั๋มรดน้ำต้นไม้
      digitalWrite(RELAYPIN, HIGH);
      
      //เปลี่ยนสถานะ INJECTPIN เท่ากับ 1 เพื่อหยุดรดน้ำต้นไม้
      digitalWrite(INJECTPIN, 1);
        
      //อัพเดทสถานะของ LED Relay บน Blynk server
      ledRelay.off();

      Blynk.virtualWrite(Widget_RelayControl, 0);
        
      //แสดงข้อความบนคอนโซล 
      Serial.print(NowString());
      Serial.println(", Stop watering.");
        
      //ส่งข้อความให้ line notify แจ้งเตือน
      NotifyLine("Stop watering.");
    
      //............................................................................
      //ส่งอัพเดทค่า DHT11/Soil Sensor ไปให้ Blynk Server
     /// sendSensor();
      
      //ส่งอัพเดทค่า LDR Sensor / WIFI  ไปให้ Blynk Server
    ///  sendSensor2();

      terminal.print(NowString());
      terminal.println(", Stop watering.");
      
    }//if
  } //while
}


