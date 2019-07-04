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

  if (value < 900) {                            //ให่ค่าที่อ่านได้ต่ำกว่า 900 เท่ากับ 900
    value = 900;
    } else if (value > 4095) {                   //ให่ค่าที่อ่านได้สูงกว่า 4095 เท่ากับ 4095
    value = 4095;
    }
    
  int PercentValue = map(value, 900, 4095, 100, 0);    //หาเปอร์เซนต์ ให้ค่าต่ำสุดคือ 900 และสูงสุดคือ 4096
    
  if (PercentValue > 80) {                            //ถ้าค่าความชื้นดินมากกว่า 80% (ดินเปียกมาก)
    digitalWrite(R_LEDPIN, LOW); digitalWrite(G_LEDPIN, HIGH); digitalWrite(B_LEDPIN, LOW);  //เปิดไฟ LED สีเขียว
    
  } else if (PercentValue <= 80 and PercentValue >= 40 ) {  //ถ้าค่าความชื้นดินอยู่ระหว่าง 40-80% (ดินชุ่ม)
    digitalWrite(R_LEDPIN, LOW); digitalWrite(G_LEDPIN, LOW); digitalWrite(B_LEDPIN, HIGH);  //เปิดไฟ LED สีน้ำเงิน
    
  } else if (PercentValue < 40) {                     //ถ้าค่าความชื้นดินน้อยกว่า 40% (ดินแห้ง)
    digitalWrite(R_LEDPIN, HIGH); digitalWrite(G_LEDPIN, LOW); digitalWrite(B_LEDPIN, LOW);  //เปิดไฟ LED สีแดง
    
    }

  Serial.print("Soil Moisture = ");                   //แสดงค่าในคอนโซล
  Serial.print(PercentValue);                         //แสดงค่าในคอนโซล
  Serial.print("% : Soil value = ");                  //แสดงค่าในคอนโซล
  Serial.println(value);                              //แสดงค่าในคอนโซล
  delay(1000);                                        //รอ 1 วินาที
}


