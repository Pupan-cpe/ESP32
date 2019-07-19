
#include "HX711.h"
#define DOUT  4
#define CLK  5
HX711 scale(DOUT, CLK);
float calibration_factor = 179999; 
void setup() {
  Serial.begin(9600);
  Serial.println("Press + or - to calibration factor");
  Serial.println("ArduinoAll Calibration 0 Please Wait ... ");
  scale.set_scale();
  scale.tare(); 
  long zero_factor = scale.read_average();
  Serial.print("Zero factor: "); 
  Serial.println(zero_factor);
}
void loop() {
  scale.set_scale(calibration_factor); 
  Serial.print("Reading: ");
  Serial.print(scale.get_units()*453.59237, 2); 
  Serial.print(" g");
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();
  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+')
      calibration_factor += 100;
    else if(temp == '-')
      calibration_factor -= 100;
  }
}
