#include "SPI.h"

#define PS2_SS  10
#define PS2_READ_DELAY  5
/*******  Connection: ********
PS2            Arduino
Data --------- 12 (MISO)
Cmd  --------- 11 (MOSI)
Attention ---- 10 (SS)
Clock -------- 13 (SCK)
Viration
Acknowledge
Gnd ---------- GND
3.3V --------- 3.3V
*****************************/
unsigned char PS2_byte[21];

unsigned char Poll_Once[5]={0x01,0x42,0x00,0x00,0x00};
unsigned char Read_Data[9]={0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
/* Only works when the controller is already in configuration mode (0xF3) */
unsigned char Enter_Config[5]={0x01,0x43,0x00,0x01,0x00};
unsigned char Set_Mode[9]={0x01,0x44,0x00,0x01,0x03,0x00,0x00,0x00,0x00};
unsigned char Exit_Config[9]={0x01,0x43,0x00,0x00,0x5A,0x5A,0x5A,0x5A,0x5A};

unsigned char set_bytes_large[]={0x01,0x4F,0x00,0xFF,0xFF,0x03,0x00,0x00,0x00};

unsigned char enable_rumble[]={0x01,0x4D,0x00,0x00,0x01};
unsigned char type_read[]={0x01,0x45,0x00,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A};
/* Only ACK'ed when the controller is in escape mode (0xF) */
unsigned char GetButton_polling[]={0x01,0x41,0x00,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A};


boolean PS2_GetValidData(void);
void Transfer_CommandList(unsigned char *CmdList, unsigned char len);
void setup() {
  pinMode(PS2_SS, OUTPUT); // we use this for SS pin
  digitalWrite(PS2_SS, HIGH);
  SPI.begin(); // wake up the SPI bus.
  
  /* PS2 Max speed = 500Kb
  => SPI Clock < 500.000 Hz
  */
  SPI.setClockDivider(SPI_CLOCK_DIV128);

  /* PS2 is LSB */
  SPI.setBitOrder(LSBFIRST);

  /* 1. PS2 Clock Avtive High RISING
  => CPOL = 1
  => SPI_MODE2 or SPI_MODE3
     2.Data are captured on clock's falling edge and data
       is output on a rising edge
  =>  CPHA = 0
  */
  SPI.setDataMode(SPI_MODE2);

  Serial.begin(9600);
  delay(100);
  /*
  Serial.println("Poll once just for fun");
  Transfer_CommandList(Poll_Once, 5);
  delay(100);

  Serial.println("Enter Config Mode");
  Transfer_CommandList(Enter_Config, 5);
  delay(100);

  Serial.println("Set Mode");
  Transfer_CommandList(Set_Mode, 9);
  delay(100);

  Serial.println("Exit Config Mode");
  Transfer_CommandList(Exit_Config, 9);
  delay(100);
  */
  reconfig_gamepad();
  delay(2000);
  PS2_GetValidData();
}
void reconfig_gamepad()
{
  Transfer_CommandList(Enter_Config, 5);
  Transfer_CommandList(Set_Mode, 9);
  Transfer_CommandList(Exit_Config, 9);
}
void loop() {
  Transfer_CommandList(Read_Data, 9);
  delay(1000);
}
void Transfer_CommandList(unsigned char *CmdList, unsigned char len)
{
  unsigned char tempChar, i;
  digitalWrite(PS2_SS, LOW);
  delay(PS2_READ_DELAY);
  for (i=0; i<len; i++)
  {
    tempChar = *CmdList;
    PS2_byte[i] = SPI.transfer(tempChar);
    CmdList++;
  }
  digitalWrite(PS2_SS, HIGH);
  
  for (i=0; i<len; i++)
  {
    Serial.print("B");
    Serial.print(i,DEC);
    Serial.print(" :");
    Serial.print(PS2_byte[i], HEX);
    Serial.print("  ");
  }
  Serial.println();
  
}

boolean PS2_GetValidData(void)
{
  unsigned char Read_Data_Ext[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

  // Try a few times to get valid data...
  for (unsigned long RetryCnt = 0; RetryCnt < 100; RetryCnt++) 
  {
    digitalWrite(PS2_SS, LOW);
    delayMicroseconds(3);
    //Send the command to send button and joystick data;
    for (int i = 0; i<9; i++) 
    {
      PS2_byte[i] = SPI.transfer(Read_Data[i]);
    }
    //if controller is in full data return mode, get the rest of data
    if(PS2_byte[1] == 0x79) 
    { 
      Serial.println("controller is in full data return mode");
      for (int i = 0; i<12; i++) 
      {
        PS2_byte[i+9] = SPI.transfer(Read_Data_Ext[i]);
      }
    }

    digitalWrite(PS2_SS, HIGH);

    // Check to see if we received valid data or not.  
    // We should be in analog mode for our data to be valid (analog == 0x7_)
    if ((PS2_byte[1] & 0xf0) == 0x70) 
    {
      Serial.println("Data is valid ^^!");
      break;
    }
    reconfig_gamepad(); // try to get back into Analog mode.
    delay(30);
    Serial.println(PS2_byte[1], HEX);
  }
  if ((PS2_byte[1] & 0xf0) == 0x70)
  {
    Serial.println("Data is valid ^^!");
    return true;
  }
  Serial.println("Data is INvalid ^^!");
  return false;
}
