int LDRSENSOR = 15;                     //GPIO15 (ADC2_CH3)    //LDR sensor

void setup() {
  Serial.begin(115200);
}

void loop() {
  int value = analogRead(LDRSENSOR);    //อ่านค่าความเข้มแสง
  Serial.println(value);
  delay(100);                           //รอ 1 วินาที
}

