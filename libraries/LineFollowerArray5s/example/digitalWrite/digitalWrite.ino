/*************************************************************
digitalWrite.ino


Hardware Hookup:
	SX1509 Breakout ------ Arduino -------- Breadboard
	    GND -------------- GND
	    5V -------------- 5V
		  SDA ------------ SDA (A4)
		  SCL ------------ SCL (A5)
		  S6 -------------------------------- LED+
		                                 LED- -/\/\/\- GND
                                                330


*************************************************************/


#include <Wire.h> // Include the I2C library (required)
#include "QGPMaker_IICSensorbar.h" 

// IICSensorBar I2C address :
const byte IIC_ADDRESS = 0x3E;  
SensorBar io; 

//pin definitions:
const byte LED_PIN_S6 = 6; // LED connected to pin 15

void setup() 
{
  if (!io.begin(SX1509_ADDRESS))
  {
    while (1) ; // If we fail to communicate, loop forever.
  }
  
  // Call io.pinMode(<pin>, <mode>)
  io.pinMode(SX1509_LED_PIN, OUTPUT);
}

void loop() 
{
  // It's blinken time!
  // Call io.digitalWrite(<pin>, <HIGH | LOW>) to set 
  // output pin as either 3.3V or 0V.
  io.digitalWrite(SX1509_LED_PIN, HIGH);
  delay(500); // Delay half-a-second
  io.digitalWrite(SX1509_LED_PIN, LOW); // Set the I/O low
  delay(500); // Delay half-a-second
}
