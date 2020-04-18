#include <SoftwareSerial.h> 
int val;
SoftwareSerial ArduinoSerial(3, 2); // RX, TX
void setup() {
Serial.begin(115200);
ArduinoSerial.begin(4800);
}
void loop() {
while (ArduinoSerial.available() > 0) {
 val = ArduinoSerial.parseInt();
if (ArduinoSerial.read() == '\n') {
Serial.println(val);
}
}
delay(100);
}
