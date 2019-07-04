#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = "0840d2bbea6a4585b1000c27439f4f0f";
char ssid[] = "plutter";
char pass[] = "visit012";

void setup()
{
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, IPAddress(128,199,204,127), 8080);
}

void loop()
{
  Blynk.run();
}

