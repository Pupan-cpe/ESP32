/*
  EX2_LED_BLINK
*/
int LEDPIN = 4;                 // กำหนดขาที่เชื่อมต่อหลอด LED

void setup() {
  pinMode(LEDPIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LEDPIN, HIGH);   // เปิดไฟหลอด LED
  delay(1000);                  // รอ 1 วินาที
  digitalWrite(LEDPIN, LOW);    // ปิดไฟหลอด LED
  delay(1000);                  // รอ 1 วินาที
}
