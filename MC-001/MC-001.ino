#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <SPIFFS.h>//เพิ่ม
#include <M5StickC.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiClient.h>

uint8_t led_count = 15;
long brightnessTime, tiltTime = 0, tiltTime2 = 0;
float b, c = 0;
int battery = 0;

float accX = 0;
float accY = 0;
float accZ = 0;


#define TFT_GREY 0x5AEB

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg = 0, mdeg = 0, hdeg = 0;
uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120; // Saved H, M, S x & y coords
uint16_t x0 = 0, x1 = 0, yy0 = 0, yy1 = 0;
uint32_t targetTime = 0;                    // for next 1 second timeout

static uint8_t conv2d(const char* p); // Forward declaration needed for IDE 1.6.x

uint8_t hh, mm, ss, YY, MM, DD, dd;

boolean initial = 1;

//needed for library
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>       //Ver 5.13.4   //https://github.com/bblanchon/ArduinoJson
//------------------------------------------------------------------------------------------------------------------------//
#include <BlynkSimpleEsp32.h>          //  Blynk_Release_v0.6.1 
//------------------------------------------------------------------------------------------------------------------------//


//------------------------------        การกำหนดค่าในส่วนของ Blynk         ------------------------------------------------//
#define BLYNK_DEBUG
#define BLYNK_PRINT Serial
int blynkIsDownCount = 0;

char blynk_token[34] = "VgOdIbkJtA3UdRfpnmsvsve2qIWUUk3R";
uint16_t result;
float temperature;
String temp;
// Replace with your network credentials

const char* serverName = "https://www.ie-advisor.net/humantemperature/";
BlynkTimer timer;  //เรียกใช้การตั้งเวลาของ Blynk

//----------------------------------  กำหนดหมายเลขของขาของ Node MCU ESP32  --------------------------------------------//

const int Ledblynk = 2;                     
const int AP_Config = 23;                
//------------------------------------------------------------------------------------------------------------------------//



bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
//------------------------------------------------------------------------------------------------------------------------//



char server[] = "oasiskit.com";
int port = 8080;
//------------------------------------------------------------------------------------------------------------------------//





//------------------------------------------------------------------------------------------------------------------------//
//*********************************************       void setup        **************************************************//
//------------------------------------------------------------------------------------------------------------------------//

void setup() {

  //-------IO NODE MCU Esp32-------//
  pinMode(Ledblynk, OUTPUT);      //กำหนดโหมดใช้งานให้กับขา Ledblynk เป็นขา สัญญาณไฟ ในสภาวะต่างๆ
  pinMode(AP_Config, INPUT_PULLUP);//กำหนดโหมดใช้งานให้กับขา AP_Config เป็นขา กดปุ่ม ค้าง เพื่อตั้งค่า AP config


  // ให้ LED ทั้งหมดดับก่อน
  digitalWrite(Ledblynk, LOW);//ให้หลอด LED สีฟ้าดับก่อน

  //-------------------------------//


  Serial.begin(115200);
  //-------------------------------//


  pinMode(M5_BUTTON_HOME, INPUT);

  pinMode(M5_BUTTON_HOME, INPUT);

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_37, 1); //1 = High, 0 = Low
  M5.begin();
  Wire.begin(0, 26);
  Serial.begin(115200);

  M5.Lcd.setRotation(3);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(3);
  //M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(0, 40);





  //*************************    การ อ่าน  เขียนค่า WiFi + Password ]ลงใน Node MCU ESP32   ************//

  //read configuration from FS json
  Serial.println("mounting FS...");//แสดงข้อความใน Serial Monitor

  if (SPIFFS.begin(true)) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          strcpy(blynk_token, json["blynk_token"]);

        } else {
          Serial.println("failed to load json config");//แสดงข้อความใน Serial Monitor
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");//แสดงข้อความใน Serial Monitor
  }
  //end read
  Serial.println(blynk_token);

  //*************************   จบการ อ่าน  เขียนค่า WiFi + Password ]ลงใน Node MCU ESP32   **********//









  //**************************        AP AUTO CONNECT   ********************************************//

  WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token, 34);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //  wifiManager.addParameter(&custom_blynk_token);


  for (int i = 5; i > -1; i--) {  // นับเวลาถอยหลัง 5 วินาทีก่อนกดปุ่ม AP Config
    digitalWrite(Ledblynk, HIGH);
    delay(500);
    digitalWrite(Ledblynk, LOW);
    delay(500);
    Serial.print (String(i) + " ");//แสดงข้อความใน Serial Monitor
  }


  if (digitalRead(AP_Config) == LOW) {
    digitalWrite(Ledblynk, HIGH);
    Serial.println("Button Pressed");//แสดงข้อความใน Serial Monitor



    // wifiManager.resetSettings();//ให้ล้างค่า SSID และ Password ที่เคยบันทึกไว้
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); //load the flash-saved configs
    esp_wifi_init(&cfg); //initiate and allocate wifi resources (does not matter if connection fails)
    delay(2000); //wait a bit
    if (esp_wifi_restore() != ESP_OK)

    {
      Serial.println("WiFi is not initialized by esp_wifi_init ");
    } else {
      Serial.println("WiFi Configurations Cleared!");
    }
    //continue
    //delay(1000);
    //esp_restart(); //just my reset configs routine...


  }


  wifiManager.setTimeout(60);



  if (!wifiManager.autoConnect("MC-01")) {
    Serial.println("failed to connect and hit timeout");//แสดงข้อความใน Serial Monitor
    M5.Lcd.fillRect(0, 40, 120, 100, BLACK);
    M5.Lcd.setCursor(20, 40);

    M5.Lcd.print("internet");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();//แก้ เดิม ESP.reset(); ใน Esp8266
    delay(5000);

  }

  Serial.println("Connected.......OK!)");//แสดงข้อความใน Serial Monitor
  strcpy(blynk_token, custom_blynk_token.getValue());


  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["blynk_token"] = blynk_token;
    File configFile = SPIFFS.open("/config.json", "w");

    if (!configFile) {
      Serial.println("failed to open config file for writing");//แสดงข้อความใน Serial Monitor

    }
    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

  //**************************    จบ    AP AUTO CONNECT   *****************************************//




  Serial.println("local ip"); //แสดงข้อความใน Serial Monitor
  delay(100);
  Serial.println(WiFi.localIP());//แสดงข้อความใน Serial Monitor
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.subnetMask());


  // Blynk.config(blynk_token);////เริ่มการเชื่อมต่อ Blynk Server แบบปกติ
  Blynk.config(blynk_token, server, port);
  ////เริ่มการเชื่อมต่อ Blynk Server*********สำหรับ Server local ที่แจกให้ เพิ่ม, server, port  แค่นี้จบ



  timer.setInterval(30000L, reconnecting);  //Function reconnect
  timer.setInterval(60000L, sendpost);

  //ตั้งเวลาส่งข้อมูลให้ Blynk Server ทุกๆ 30 วินาที


}


//------------------------------------------------------------------------------------------------------------------------//
//*********************************************   จบ  void setup        **************************************************//
//------------------------------------------------------------------------------------------------------------------------//


//------------------------------------------------------------------------------------------------------------------------//
//*********************************************       void Loop        ***************************************************//
//------------------------------------------------------------------------------------------------------------------------//


void loop() {
  brightnessT();
//  batterySaver();
//  batteryLevel();

  //-----------------------------------------------------------------------------------------------------------//
  if (Blynk.connected())
  {
    Blynk.run();
    digitalWrite(Ledblynk, HIGH);

  } else {

    digitalWrite(Ledblynk, LOW);
    delay(200);
    digitalWrite(Ledblynk, HIGH);
    delay(100);

  }
  //-----------------------------------------------------------------------------------------------------------//
  Wire.beginTransmission(0x5A);          // Send Initial Signal and I2C Bus Address
  Wire.write(0x07);                      // Send data only once and add one address automatically.
  Wire.endTransmission(false);           // Stop signal
  Wire.requestFrom(0x5A, 2);             // Get 2 consecutive data from 0x5A, and the data is stored only.
  result = Wire.read();                  // Receive DATA
  result |= Wire.read() << 8;            // Receive DATA

  temperature = result * 0.02 - 273.15;

  M5.Lcd.fillRect(0, 40, 120, 100, BLACK);
  M5.Lcd.setCursor(20, 40);

  M5.Lcd.print(temperature);
  //  Serial.println(temperature);

  Blynk.virtualWrite(V6, temperature);
  M5.update();
  timer.run();//ให้เวลาของ Blynk ทำงาน

  delay(500);


}

//------------------------------------------------------------------------------------------------------------------------//
//*********************************************      จบ void Loop       **************************************************//
//------------------------------------------------------------------------------------------------------------------------//




//------------------------------------------------------------------------------------------------------------------------//
void reconnecting()
{
  if (!Blynk.connected())
  {
    blynkIsDownCount++;
    BLYNK_LOG("blynk server is down! %d  times", blynkIsDownCount);
    Blynk.connect(5000);
  }
}

void sendpost() {
  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Prepare your HTTP POST request data
    String httpRequestData = "&id_mc=" + String("1") + "&temp=" + temperature;
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    int httpResponseCode = http.POST(httpRequestData);



    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  //Send an HTTP POST request every 30 seconds

}


void Sensor()
{

  float t = temperature;
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //  Serial.print("Temperature is: "); Serial.println(t, 1);


  Blynk.virtualWrite(V6, t);
}

void batterySaver() {
  M5.MPU6886.getAccelData(&accX, &accY, &accZ);
  accX *= 1000;
  accY *= 1000;

  if (!((accX < (0 - 200) && (accX > -900)) && ((accY > 0 - 300) && (accY < 300))))
  {
    if (millis() > (tiltTime2 + 10000)) {
      M5.Axp.DeepSleep();
      //    while (accX < (0 - 500) && ((accY > 0) && (accY < 100)));
    }
  }
  else
    tiltTime2 = millis();
}
void brightnessT() {
  M5.MPU6886.getAccelData(&accX, &accY, &accZ);
  accX *= 1000;
  accY *= 1000;

  if ((accX < (0 - 200) && (accX > -900)) && ((accY > 0 - 300) && (accY < 300)))
  {
    if (millis() > (tiltTime + 500)) {
      M5.Axp.ScreenBreath(12);
      brightnessTime = millis();
      //    while (accX < (0 - 500) && ((accY > 0) && (accY < 100)));
    }
  }
  else
    tiltTime = millis();

  if (brightnessTime < millis() - 2000)
  {
    M5.Axp.ScreenBreath(7);
    brightnessTime = 0;
  }
}
void batteryLevel() {
  M5.Lcd.setCursor(110, 3, 1);
  c = M5.Axp.GetVapsData() * 1.4 / 1000;
  b = M5.Axp.GetVbatData() * 1.1 / 1000;
  //  M5.Lcd.print(b);
  battery = ((b - 3.0) / 1.2) * 100;

  if (c >= 4.5) {
    M5.Lcd.setTextColor(TFT_YELLOW, TFT_GREY);
    M5.Lcd.print("CHG:");
  }
  else {

  }

  if (battery > 100)
    battery = 100;
  else if (battery < 100 && battery > 9)
    M5.Lcd.print(" ");
  else if (battery < 9)
    M5.Lcd.print("  ");
  if (battery < 10)
    M5.Axp.DeepSleep();

  //  if (digitalRead(M5_BUTTON_HOME) == LOW) {
  //    while (digitalRead(M5_BUTTON_HOME) == LOW);
  //    M5.Axp.DeepSleep(SLEEP_SEC(1));
  //  }
  M5.Lcd.print(battery);
  M5.Lcd.print("%");
}
