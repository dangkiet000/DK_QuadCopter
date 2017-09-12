/*
  Blink
  Turns on an LED on for two second, then off for two second, repeatedly.
 
  This example code is in the public domain.
 */
int pin=13;
void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(pin, OUTPUT);     
}

void loop() {
  digitalWrite(pin, HIGH);   // set the LED on
  delay(2000);              // wait for a second
  digitalWrite(pin, LOW);    // set the LED off
  delay(2000);              // wait for a second
}