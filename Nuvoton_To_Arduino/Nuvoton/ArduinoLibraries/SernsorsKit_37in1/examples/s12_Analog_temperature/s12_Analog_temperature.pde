int sensorPin = A0;    // select the input pin for the potentiometer

#include <math.h>
double Thermister(int RawADC) {
double Temp;
Temp = log(((10240000/RawADC) - 10000));
Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
Temp = Temp - 273.15; // Convert Kelvin to Celcius
return Temp;
}

void setup() {
 pinMode(sensorPin, OUTPUT); 
  Serial.begin(9600); 
}

void loop() {
Serial.print(Thermister(analogRead(sensorPin))); // display Fahrenheit 
Serial.println("c"); 
delay(500);
}