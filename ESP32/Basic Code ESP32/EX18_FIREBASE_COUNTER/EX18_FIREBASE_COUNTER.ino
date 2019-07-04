#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>

/* ################################################################ */

//Firebase config here
#define FIREBASE_HOST "projectx-23875.firebaseio.com"
#define FIREBASE_AUTH "wZz7qarwgld9YY4i1NUa8Hpbaor3wah9pIeIWu2V"
#define WIFI_SSID "Kerdsawas 2.4G"
#define WIFI_PASSWORD "aoyaomaum"

/* ################################################################ */

int i = 0;
void setup() {

  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  // set string value
  Firebase.setString("Message", "Welcome to IoT training.");
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /message failed:");
      Serial.println(Firebase.error());  
      return;
  }
  
}

void loop() {
  
  Firebase.setInt("CounterNumber", i);
  if (Firebase.failed()) {
    
    Serial.println("set /CounterNumber failed:");
    Serial.println(Firebase.error());  
    return;
  }

  Serial.print("set /CounterNumber to ");
  Serial.println(Firebase.getInt("CounterNumber"));
  
  i++;
  
  delay(500);
}
