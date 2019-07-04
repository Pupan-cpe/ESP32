
void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 Touch Test");
}

void loop()
{
  Serial.print("Touch sensor = ");
  Serial.println(touchRead(T3));  // get value using T3
  delay(100);
}
