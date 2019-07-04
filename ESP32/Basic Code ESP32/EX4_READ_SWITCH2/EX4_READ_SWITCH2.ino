int LEDPIN = 4;
int SWPIN = 18;

void setup() {

  Serial.begin(115200);
  pinMode(LEDPIN, OUTPUT);          //กำหนดขาดิจิตอล LEDPIN เป็นขาเอาท์พุท (LED)
  pinMode(SWPIN, INPUT_PULLUP);    //กำหนดขาดิจิตอล SWPIN เป็นขาอ่านข้อมูลการกดสวิตซ์
  
}

void loop() {
  
  boolean data = digitalRead(SWPIN);   //อ่านค่าข้อมูลขาดิจิตอล SWPIN ในรูปแบบตัวเลข
  
  if (data == 0) {
    digitalWrite(LEDPIN, HIGH);
    Serial.println("LED status = ON");
  } else {
    digitalWrite(LEDPIN, LOW);
    Serial.println("LED status = OFF");
  }

  delay(100);                      //รอ 100 มิลลิวินาที
  
}


