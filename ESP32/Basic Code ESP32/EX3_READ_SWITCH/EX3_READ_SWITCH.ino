/*
  EX3_READ_SWITCH
*/
int SWPIN = 18;                  //กำหนดขาดิจิตอล 18
void setup() {
  
  Serial.begin(115200);         //กำหนดค่าพอร์ตคอนโซล
  pinMode(SWPIN, INPUT_PULLUP); //กำหนดขาดิจิตอล 18 เป็นขาอ่านข้อมูลการกดสวิตซ์
  
}

void loop() {
  
  int data = digitalRead(SWPIN);  //อ่านค่าข้อมูลขาดิจิตอล 18 ในรูปแบบตัวเลข
  Serial.println(data);           //แสดงค่าข้อมูลที่อ่านได้ในคอนโซล
  delay(100);                     //รอ 100 มิลลิวินาที
  
}


