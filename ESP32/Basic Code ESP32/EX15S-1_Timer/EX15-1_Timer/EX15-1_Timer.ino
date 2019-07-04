#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp32.h>

#define TestLED 4                 // on board LED pin assignment

char auth[] = "b1343c84b1204752b8d4681e28156aa1";
char ssid[] = "plutter";
char pass[] = "visit012";

BlynkTimer timer;
WidgetLED ledStatus(V12);

BLYNK_WRITE(V11)
{
  // You'll get HIGH/1 at startTime and LOW/0 at stopTime.
  // this method will be triggered every day
  // until you remove widget or stop project or
  // clean stop/start fields of widget
  Serial.print("Got a value: ");
  Serial.println(param.asInt());
  int data = param.asInt();
  if(data == 1)
  {
    digitalWrite(TestLED, HIGH);
    ledStatus.on();
    Serial.println("LED is ON");
  }else{    
    digitalWrite(TestLED, LOW);
    ledStatus.off();
    Serial.println("LED is OFF");
    }
  
}

void setup() {
  Serial.begin(115200); 
  pinMode(TestLED, OUTPUT); 
  Blynk.begin(auth, ssid, pass, IPAddress(128,199,204,127), 8080);
 // timer.setInterval(2000L, sendSensor);
  
}

void loop() {
  Blynk.run();
  timer.run();
}

