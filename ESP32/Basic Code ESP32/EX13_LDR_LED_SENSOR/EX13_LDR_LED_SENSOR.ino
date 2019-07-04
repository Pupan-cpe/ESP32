int LEDPIN = 4;                                     //GPIO4 LED
int LDRSENSOR = 15;                                 //GPIO15 (ADC2_CH3)    //LDR SENSOR

void setup() {
  Serial.begin(115200);
  pinMode(LEDPIN, OUTPUT);                          //กำหนดขา LEDPIN เป็นขาออกที่เชื่อมต่อ LED
}

void loop() {
  int value = analogRead(LDRSENSOR);                //อ่านค่าความเข้มแสง
  if (value < 300) {                                //ให่ค่าที่อ่านได้ต่ำกว่า 300 เท่ากับ 300
    value = 300;
    } else if (value > 4000) {                      //ให่ค่าที่อ่านได้สูงกว่า 4000 เท่ากับ 4000
    value = 4000; }
    
  int PercentValue = map(value, 300, 4000, 100, 0);   //หาเปอร์เซนต์ ให้ค่าต่ำสุดคือ 300 และสูงสุดคือ 4000
    
  if (PercentValue < 30) {                            //ถ้าค่าความเข้มแสงน้อยกว่า 30% (มืดมาก)
    digitalWrite(LEDPIN, HIGH);                       //เปิดไฟ LED
    Serial.println("LED TURN ON");
  } else { 
    digitalWrite(LEDPIN, LOW);                        //ปิดไฟ LED
    Serial.println("LED TURN OFF");
  }               

  Serial.print("LDR percent = ");                     //แสดงค่าในคอนโซล
  Serial.print(PercentValue);                         //แสดงค่าในคอนโซล
  Serial.print("% : LDR value = ");                   //แสดงค่าในคอนโซล
  Serial.println(value);                              //แสดงค่าในคอนโซล
  delay(500);                                         //รอ 0.5 วินาที
}


