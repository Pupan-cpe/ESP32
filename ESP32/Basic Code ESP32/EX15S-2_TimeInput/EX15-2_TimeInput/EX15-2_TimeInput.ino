#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp32.h>
#include <SimpleTimer.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

BlynkTimer timer;

WidgetRTC rtc;
WidgetLED ledStatus(V12);
//BLYNK_ATTACH_WIDGET(rtc, V0);

#define TestLED 4                 // on board LED pin assignment
char Date[16];
char Time[16];
char auth[] = "2fe2bdd3339e40f5a7991b1b2414c167";
char ssid[] = "plutter";
char pass[] = "visit012";
long startsecondswd;            // weekday start time in seconds
long stopsecondswd;             // weekday stop  time in seconds
long nowseconds;                // time now in seconds

void setup() {
  
  pinMode(TestLED, OUTPUT);
  digitalWrite(TestLED, LOW); // set LED OFF 
  Serial.begin(115200);
  Serial.println("\Starting");
  Blynk.begin(auth, ssid, pass, IPAddress(128,199,204,127), 8080);
 
 int mytimeout = millis() / 1000;
  while (Blynk.connect() == false) { // try to connect to server for 10 seconds
    if((millis() / 1000) > mytimeout + 8){ // try local server if not connected within 9 seconds
       break;
    }
  }
  rtc.begin();
  timer.setInterval(60000L, activetoday);  // check every minute if schedule should run today 
  timer.setInterval(30000L, reconnectBlynk);  // check every 30s if still connected to server 
}

void activetoday(){        // check if schedule should run today
  if(year() != 1970){
    Blynk.syncVirtual(V13); // sync timeinput widget    
  }
}

BLYNK_WRITE(V13) {
  sprintf(Date, "%02d/%02d/%04d",  day(), month(), year());
  sprintf(Time, "%02d:%02d:%02d", hour(), minute(), second());
  
  TimeInputParam t(param);
  
  Serial.print("Checked schedule at: ");
  Serial.println(Time);
  int dayadjustment = -1;  
  if(weekday() == 1){
    dayadjustment =  6; // needed for Sunday, Time library is day 1 and Blynk is day 7
  }
  if(t.isWeekdaySelected((weekday() + dayadjustment))){ //Time library starts week on Sunday, Blynk on Monday
    Serial.println("Schedule ACTIVE today");
    if (t.hasStartTime()) // Process start time
    {
      Serial.println(String("Start: ") + t.getStartHour() + ":" + t.getStartMinute());
    }
    if (t.hasStopTime()) // Process stop time
    {
      Serial.println(String("Stop : ") + t.getStopHour() + ":" + t.getStopMinute());
    }
    // Display timezone details, for information purposes only 
    Serial.println(String("Time zone: ") + t.getTZ()); // Timezone is already added to start/stop time 
    Serial.println(String("Time zone offset: ") + t.getTZ_Offset()); // Get timezone offset (in seconds)
  
    for (int i = 1; i <= 7; i++) {  // Process weekdays (1. Mon, 2. Tue, 3. Wed, ...)
      if (t.isWeekdaySelected(i)) {
        Serial.println(String("Day ") + i + " is selected");
      }
    } 
    nowseconds = ((hour() * 3600) + (minute() * 60) + second());
    startsecondswd = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
    //Serial.println(startsecondswd);  // used for debugging
    if(nowseconds >= startsecondswd){    
      Serial.print("LED is ON at");
      Serial.println(String(" ") + t.getStartHour() + ":" + t.getStartMinute());
      if(nowseconds <= startsecondswd + 90){    // 90s on 60s timer ensures 1 trigger command is sent
        digitalWrite(TestLED, HIGH); // set LED ON
        ledStatus.on();
        // code here to switch the relay ON
      }      
    }
    else{
      Serial.println("LED NOT ON today");
      // nothing more to do here, waiting for motor to be turned on later today      
    }
    stopsecondswd = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
    //Serial.println(stopsecondswd);  // used for debugging
    if(nowseconds >= stopsecondswd){
      Serial.print("LED is OFF at");
      Serial.println(String(" ") + t.getStopHour() + ":" + t.getStopMinute());
      if(nowseconds <= stopsecondswd + 90){   // 90s on 60s timer ensures 1 trigger command is sent
        digitalWrite(TestLED, LOW); // set LED OFF
        ledStatus.off();
        // code here to switch the relay OFF
      }              
    }
    else{
      if(nowseconds >= startsecondswd){  // only show if motor has already started today
        Serial.println("LED is still ON");
        // nothing more to do here, waiting for motor to be turned off later today
      }          
    }
  }
  else{
    Serial.println("Schedule INACTIVE today");
    // nothing to do today, check again in 1 minutes time    
  }
  Serial.println();
}

void reconnectBlynk() {
  if (!Blynk.connected()) {
    if(Blynk.connect()) {
      BLYNK_LOG("Reconnected");
    } else {
      BLYNK_LOG("Not reconnected");
    }
  }
}

void loop()
{
  if (Blynk.connected()) {
    Blynk.run();
  }
  timer.run();
}

