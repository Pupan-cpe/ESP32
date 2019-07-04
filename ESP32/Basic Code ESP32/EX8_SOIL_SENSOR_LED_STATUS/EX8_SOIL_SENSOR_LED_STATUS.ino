int LEDPIN = 4;
int SOILSENSOR = 15;                     //GPIO15 (ADC2_CH3)    //Soil sensor

void setup() {
  Serial.begin(115200);
  pinMode(LEDPIN, OUTPUT);               //กำหนดขา LEDPIN เป็นขาออกที่เชื่อมต่อ LED
}

void loop() {

  int value = analogRead(SOILSENSOR);    //อ่านค่าความชื้นดิน
  if (value < 2500) {                    //ถ้าค่าความชื้นดินมากกว่า 1200 (ชื้นมาก)
    digitalWrite(LEDPIN, LOW);           //ปิดไฟ LED
    Serial.println("NORMAL STATUS");
  } else {   
    digitalWrite(LEDPIN, HIGH);          //เปิดไฟ LED 
    Serial.println("WATER INJECT!"); 
  }
  
  Serial.print("Soil sensor values = ");
  Serial.println(value);
  delay(1000);                           //รอ 1 วินาที
}

