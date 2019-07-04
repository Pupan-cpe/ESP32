int R_LEDPIN = 5;
int G_LEDPIN = 18;
int B_LEDPIN = 19;
int SOILSENSOR = 15;                       //GPIO15 (ADC2_CH3)    //Soil sensor

void setup() {
  Serial.begin(115200);
  pinMode(R_LEDPIN, OUTPUT);               //กำหนดขา R_LEDPIN เป็นขาออกที่เชื่อมต่อ LED RED
  pinMode(G_LEDPIN, OUTPUT);               //กำหนดขา G_LEDPIN เป็นขาออกที่เชื่อมต่อ LED GREEN
  pinMode(B_LEDPIN, OUTPUT);               //กำหนดขา B_LEDPIN เป็นขาออกที่เชื่อมต่อ LED BLUE
}

void loop() {
  int value = analogRead(SOILSENSOR);           //อ่านค่าความชื้นดิน
  if (value < 2000) {                            //ถ้าค่าความชื้นดินมากกว่า 1000 (ดินเปียกมาก)
    digitalWrite(R_LEDPIN, LOW); digitalWrite(G_LEDPIN, HIGH); digitalWrite(B_LEDPIN, LOW);  //เปิดไฟ LED สีเขียว
    
  } else if (value >= 2000 and value <= 3500 ) {  //ถ้าค่าความชื้นดินอยู่ระหว่าง 1000 และ 3000 (ดินชุ่ม)
    digitalWrite(R_LEDPIN, LOW); digitalWrite(G_LEDPIN, LOW); digitalWrite(B_LEDPIN, HIGH);  //เปิดไฟ LED สีน้ำเงิน
    
  } else if (value >= 3500 and  value <= 4000) {                     //ถ้าค่าความชื้นดินมากกว่า 3000 (ดินแห้ง)
    digitalWrite(R_LEDPIN, HIGH); digitalWrite(G_LEDPIN, LOW); digitalWrite(B_LEDPIN, LOW);  //เปิดไฟ LED สีแดง
    
    } else if(value > 4000)
    {digitalWrite(R_LEDPIN, HIGH); digitalWrite(G_LEDPIN, LOW); digitalWrite(B_LEDPIN, 1);}

  Serial.println(value);
  delay(100);                                 //รอ 1 วินาที
}


