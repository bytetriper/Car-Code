/*************************************************************
analogWrite.ino
I/O Expander Example: pwm output (analogWrite)


This example demonstrates the SX1509's analogWrite function. 
Connect an LED to the SX1509's pin 15 (or any other pin, they 
can all PWM!). The SX1509 can either sink or source current, 
just don't forget your limiting resistor!

Hardware Hookup:
	IICSensorBar Breakout ------ Arduino -------- Breadboard
	    GND -------------- GND
	    5V -------------- 5V
		  SDA ------------ SDA (A4)
		  SCL ------------ SCL (A5)
		  S5 -------------------------------- LED+
		                                 LED- -/\/\/\- GND
                                            330

*************************************************************/

#include <Wire.h> // Include the I2C library (required)
#include "QGPMaker_IICSensorbar.h" 

// IICSensorBar I2C address :
const byte IIC_ADDRESS = 0x3E;  
SensorBar io; 

// Pin definition:
const byte LED_PIN_S5 = 5; 

void setup() 
{
  // Call io.begin(<address>) to initialize. If it 
  // successfully communicates, it'll return 1.
  if (!io.begin(IIC_ADDRESS))
  {
    while (1) ; // If we fail to communicate, loop forever.
  }

  
  // Use the pinMode(<pin>, <mode>) function to set our led 
  // pin as an ANALOG_OUTPUT, which is required for PWM output
  io.pinMode(LED_PIN_S5, ANALOG_OUTPUT);
  io.analogWrite(LED_PIN_S5, 200);
}

void loop()
{

  
  
  // Ramp brightness up, from 0-255, delay 2ms in between 
  // analogWrite's
  for (int brightness=0; brightness<256; brightness++)
  {
    // Call io.analogWrite(<pin>, <0-255>) to configure the 
    // PWM duty cycle
    io.analogWrite(LED_PIN_S5, brightness);
    delay(2); // Delay 2 milliseconds
  }
  delay(500); // Delay half-a-second
  
  // Ramp brightness down, from 255-0, delay 2ms in between 
  // analogWrite's
  for (int brightness=255; brightness>=0; brightness--)
  {
    io.analogWrite(LED_PIN_S5, brightness);
    delay(2); // Delay 2 milliseconds
  }
  delay(500); // Delay half-a-second
  
}
