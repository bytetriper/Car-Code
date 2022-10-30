/*************************************************************
digitalRead.ino
Example: digital in (digitalRead)


This example demonstrates the IICSensorBar digitalRead
functionality. A pin can either be set as an INPUT or
INPUT_PULLUP. We'll attach an active-low button to an 
INPUT_PULLUP input, then whenever the button read's LOW, we'll
read the state of another INPUT pin.

After uploading the sketch, open your serial monitor and set 
it to 9600 baud.

Hardware Hookup:
	IICSensorBar Breakout ------ Arduino
	    GND -------------- GND
	    5V -------------- 5V
		  SDA ------------ SDA (A4)
		  SCL ------------ SCL (A5)
		  S0 


*************************************************************/

#include <Wire.h> // Include the I2C library (required)
#include "QGPMaker_IICSensorbar.h" 

// IICSensorBar I2C address :
const byte IIC_ADDRESS = 0x3F;  
SensorBar io; 

// Pin definition:
const byte INPUT_PIN_S0 = 0; // S0
const byte INPUT_PIN_S8 = 4; // S8

void setup() 
{

  Serial.begin(9600);
  // Call io.begin(<address>) to initialize. If it
  // successfully communicates, it'll return 1.
  if (!io.begin(IIC_ADDRESS))
  {
    Serial.println("Failed to communicate.");
    while (1) ;
  }

  // use io.pinMode(<pin>, <mode>) to set input pins as either 
  // INPUT or INPUT_PULLUP
  io.pinMode(INPUT_PIN_S0, INPUT);
  io.pinMode(INPUT_PIN_S8, INPUT);
}

void loop() 
{

    Serial.print("SO,S8 status: ");
	// Read the pin to print either 0 or 1
    Serial.print(io.digitalRead(INPUT_PIN_S0));
    Serial.print(io.digitalRead(INPUT_PIN_S8));
}
