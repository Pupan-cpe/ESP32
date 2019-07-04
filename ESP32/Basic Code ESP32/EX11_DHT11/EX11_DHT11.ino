#include <SimpleDHT.h>          //โหลดไลบารี่ DHT11

int DHTPIN = 15;                //GPIO15 (ADC2_CH3)    //DHT11 sensor

SimpleDHT11 dht11;              //ระบุรุ่นเซ็นเซอร์รุ่น DHT11
byte temperature = 0;           //กำหนดตัวแปรเก็บค่าอุณหภูมิ
byte humidity = 0;              //กำหนดตัวแปรเก็บค่าความชื้นสัมสัทธ์


void setup() {
  
  Serial.begin(115200);         //ตั้งค่าคอนโซล
  
}

void loop() {
  
  dht11.read(DHTPIN, &temperature, &humidity, NULL);  //อ่านค่าจากเซ็นเซอร์
  
  delay(1500);                                    //รอ 1.5 วินาที
  
  Serial.print("DHT11 read: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" %H");
}


