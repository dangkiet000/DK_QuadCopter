const int interruptA = 2;       // Interrupt 0 在 pin 2 上
const int interruptB = 3;       // Interrupt 1 在 pin 3 上 沒用

int clk = 2;     // PIN2
int DAT = 3;     // PIN3
int BUTTON = 4;  // PIN4
int LED1 = 5;    // PIN5
int LED2 = 6;    // PIN6
int COUNT = 0;

void setup() 
 {
  attachInterrupt(interruptA, RoteStateChanged, FALLING);
 // attachInterrupt(interruptB, buttonState, FALLING);
  pinMode(clk, INPUT);   
  pinMode(DAT, INPUT);   
  pinMode(BUTTON, INPUT); 
  
  pinMode(LED1, OUTPUT); 
  pinMode(LED2, OUTPUT); 
  Serial.begin(9600);
 }

void loop() 
{
  if  (!(digitalRead(BUTTON))) 
    {
     COUNT = 0;  
     Serial.println("STOP COUNT = 0");
     digitalWrite(LED1, LOW);
     digitalWrite(LED2, LOW);
     delay (2000);
    }
     Serial.println(COUNT);  
}

//-------------------------------------------
void RoteStateChanged() //When CLK  FALLING READ DAT
{
 if  (digitalRead(DAT)) // When DAT = HIGH IS FORWARD
   {
    COUNT++;
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
   }
 else                   // When DAT = LOW IS BackRote
   {
    COUNT--;
    digitalWrite(LED2, HIGH);
    digitalWrite(LED1, LOW);
   }
}
