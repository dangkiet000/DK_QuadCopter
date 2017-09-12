int LedPinA = 5;
int LedPinB = 4;
int ButtonPinA = 12;
int ButtonPinB = 11;
int buttonStateA = 0; 
int buttonStateB = 0;
int brightness   = 0;

void setup() 
{
    Serial.begin(115200);
    pinMode(LedPinA, OUTPUT); 
    pinMode(LedPinB, OUTPUT);  
    pinMode(ButtonPinA, INPUT); 
    pinMode(ButtonPinB, INPUT);     
}

void loop() 
{
  Serial.print("ButtonPinA =");
    Serial.print(buttonStateA);
  Serial.print(" ButtonPinB =");  
   Serial.println(buttonStateB);
  buttonStateA = digitalRead(ButtonPinA);
  if (buttonStateA == LOW && brightness != 255)
  { 
   brightness ++;
  } 

   buttonStateB = digitalRead(ButtonPinB);
   if (buttonStateB == LOW && brightness != 0)
  { 
   brightness --;
  } 
      analogWrite(LedPinA, brightness);  //  A few dark
      analogWrite(LedPinB, 255 - brightness);  // B few light
      delay(25);
}
