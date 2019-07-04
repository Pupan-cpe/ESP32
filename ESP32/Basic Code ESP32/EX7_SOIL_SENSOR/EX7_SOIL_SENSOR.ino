void setup() {
  Serial.begin(115200);            //ตั้งค่าพอร์ตคอนโซล
}

void loop() {
  
  int value = analogRead(15);     //อ่านค่าความชื้นดิน 
  Serial.println(value);          //แสดงผลค่าความชื้นดินบนคอนโซล
  delay(100);                    //รอ 1 วินาที
  
}

