#include <WiFi.h>
#include <WiFiClientSecure.h>

//########## CONFIGURATION : MODIFY HERE ##########

char ssid[] = "plutter";  //ขื่อวายฟาย SSID
char pass[] = "visit012"; //รหัสผ่านวายฟาย

String txt = "Switch pressed";  //ข้อความที่จะแสดงใน Line
#define TokenLine "0kWBLPCHq5MYSuhdF8WMyVoRGnkrSfH9YQdhr4s9pHP" //รหัสเชื่อมต่อ Line
#define SWPIN 5  //กำหนดขาสวิตซ์ที่เชื่อมต่อกับสวิตซ์

//#################################################

boolean oldState; 
boolean data;

void setup() {
  
  pinMode(SWPIN, INPUT_PULLUP);               //กำหนดให้ขาดิจิตอล SWPIN รับข้อมูลจากสวิตซ์
  Serial.begin(115200);                       //ตั้งค่าคอนโซล
  WiFi.begin(ssid, pass);                     //เริ่มการเชื่อมต่อกับวายฟายแม่ข่าย
  Serial.print("WiFi Connecting");            //แสดง "WiFi Connecting" ในคอนโซล

//ตรวจเช็คสถานะการเขื่อมต่อวายฟาย ถ้าไม่สำเร็จให้แสดง "." ในคอนโซลจนกว่าจะเขื่อมต่อได้
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  //แสดงสถานะการวายฟายเชื่อต่อแล้ว และแสดงหมายเลขไอพีที่ได้รับในคอนโซล
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
}

void loop() {
  
  data = digitalRead(SWPIN);
  if (data == 0 and oldState == 1) {
    delay (10); 
      if (digitalRead(SWPIN) == 0) {
        Serial.println(txt);
        NotifyLine(txt);
      }
  }
  oldState = data;
  
}

void NotifyLine(String t) {
  
  WiFiClientSecure client;

  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("Connection failed");
    return;   
  }

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(TokenLine) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + t).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + t;
  
  Serial.println(req);
  client.print(req);
  delay(20);
  Serial.println("-------------");
  while(client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
}
