#include <WiFi.h>
#include <Servo.h> //ประกาศตัวแปร Servo

Servo myservo;  //สร้าง object เพื่อนควบคุม servo

// ประกาศตัวแปร Servo ให้อยู่ที่ ขา 13 
static const int servoPin = 13;

// เชื่อมต่ออินเทอร์เน็ตของคุณ
const char* ssid     = "IPhone";
const char* password = "0844095831";

// ตั้งค่าพอร์ตของเว็บเซิร์ฟเวอร์เป็น 80
WiFiServer server(80);

// ตัวแปรในการจัดเก็บคำขอ HTTP
String header;

// ถอดรหัสค่า HTTP GET
String valueString = String(5);
int pos1 = 0;
int pos2 = 0;

void setup() {
  Serial.begin(115200);

  myservo.attach(servoPin);  //ประกาศตัวแปรให้Servoรู้ว่า servo คือ ขา13
  //เชื่อมต่อกับเครือข่าย Wi-Fi ด้วย SSID และรหัสผ่าน
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print IP ท้องถิ่นและ localIP
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // อ่านสัญญาณที่เข้ามา

  if (client) {                             // เชื่อมต่อเครือข่าย
    Serial.println("New Client.");          // print ข้อความออกมาโชว์บน Serail port
    String currentLine = "";                // สร้าง String เพื่อเก็บข้อมูลที่เข้ามาจากเครือข่าย
    while (client.connected()) {            // วนลูปในจนกว่าจะเชื่อมต่อ
      if (client.available()) {             // ถ้ามีจำนวนไบต์จากการเชื่อมต่อ
        char c = client.read();             // อ่านไบต์แล้ว
        Serial.write(c);                    // โชว์ข้อความออก Serial port
        header += c;
        if (c == '\n') {                    //ถ้าไบต์เป็นอักขระขึ้นบรรทัดใหม่
          // หากบรรทัดปัจจุบันว่างเปล่าคุณจะมีอักขระขึ้นบรรทัดใหม่สองตัวในหนึ่งแถว
           // นั่นคือจุดสิ้นสุดของคำขอ HTTP ของClientดังนั้นให้ส่งคำตอบ
          if (currentLine.length() == 0) {
            //ส่วนหัว HTTP เริ่มต้นด้วยรหัสตอบกลับเสมอ (เช่น HTTP / 1.1 200 OK)
             // และประเภทเนื้อหาเพื่อให้ลูกค้ารู้ว่ามีอะไรเกิดขึ้นจากนั้นบรรทัดว่าง:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            //แสดงหน้าเว็บ HTML
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}");
            client.println(".slider { width: 300px; }</style>");
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
                     
            // หน้าเว็บ
           client.println("<img src=\"https://sv1.picz.in.th/images/2019/07/04/1L8kcQ.jpg\" alt=\"1L8kcQ.jpg\" border=\"0\" />");
            client.println("</head><body><h1>ESP32 with Servo</h1>");
            client.println("<p>Position: <span id=\"servoPos\"></span></p>");          
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider\" onchange=\"servo(this.value)\" value=\""+valueString+"\"/>");
            
            client.println("<script>var slider = document.getElementById(\"servoSlider\");");
            client.println("var servoP = document.getElementById(\"servoPos\"); servoP.innerHTML = slider.value;");
            client.println("slider.oninput = function() { slider.value = this.value; servoP.innerHTML = this.value; }");
            client.println("$.ajaxSetup({timeout:1000}); function servo(pos) { ");
            client.println("$.get(\"/?value=\" + pos + \"&\"); {Connection: close};}</script>");
           
            client.println("</body></html>");     
            
            //GET /?value=180& HTTP/1.1
            if(header.indexOf("GET /?value=")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1+1, pos2);
              
              //หมุน servo
              myservo.write(valueString.toInt());
              Serial.println(valueString); 
            }         
            // การตอบกลับ HTTP ลงท้ายด้วยบรรทัดว่างอื่น
            client.println();
            // Break 
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // ล้างตัวแปรส่วนหัว
    header = "";
    //ปิดการเชื่อมต่อ
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
