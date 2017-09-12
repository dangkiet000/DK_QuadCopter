/* 
 Keyboard Controller Example

 Shows the output of a USB Keyboard connected to
 the Native USB port on an Arduino Due Board.

 created 8 Oct 2012
 by Cristian Maglie

 http://arduino.cc/en/Tutorial/KeyboardController

 This sample code is part of the public domain.
 */

#include "Arduino.h"

// Require keyboard control library
#include <nvtKeyboardController.h>

// Initialize USB Controller
USBHost * usb;

// Attach keyboard controller to USB
KeyboardController * keyboard;

// This function intercepts key press
void keyPressed() {
  Serial.print("Pressed:  ");
  printKey();
}

// This function intercepts key release
void keyReleased() {
  Serial.print("Released: ");
  printKey();
}

void printKey() {
  // getOemKey() returns the OEM-code associated with the key
  Serial.print(" key:");
  Serial.print(keyboard->getOemKey());

  // getModifiers() returns a bits field with the modifiers-keys
  int mod = keyboard->getModifiers();
  Serial.print(" mod:");
  Serial.print(mod);

  Serial.print(" => ");

  if (mod & LeftCtrl)
    Serial.print("L-Ctrl ");
  if (mod & LeftShift)
    Serial.print("L-Shift ");
  if (mod & Alt)
    Serial.print("Alt ");
  if (mod & LeftCmd)
    Serial.print("L-Cmd ");
  if (mod & RightCtrl)
    Serial.print("R-Ctrl ");
  if (mod & RightShift)
    Serial.print("R-Shift ");
  if (mod & AltGr)
    Serial.print("AltGr ");
  if (mod & RightCmd)
    Serial.print("R-Cmd ");

  // getKey() returns the ASCII translation of OEM key
  // combined with modifiers.
  Serial.write(keyboard->getKey());
  Serial.println();
}


void setup()
{
  Serial.begin(115200);
  Serial.println("Program started");
  delay(200);

  usb = new USBHost;
  keyboard = new KeyboardController;
}

void loop()
{
  // Process USB tasks
  usb->Task();
  keyboard->poll();
}
