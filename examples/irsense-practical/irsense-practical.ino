/*************************************************************************
Title:    Arduino I2C-IRSENSE Practical Example
Authors:  Nathan D. Holmes <maverick@drgw.net>
File:     $Id: $
License:  GNU General Public License v3

This is a more practical example of how to use the Iowa Scaled Engineering
I2C-IRSENSE ( https://www.iascaled.com/store/I2C-IRSENSE ), with some filtering
and error handling code.  If you're a beginner, I advise starting with the irsense-simple
example first.  It doesn't have any of the application logic here to confuse you,
and just shows how to use the sensor.

This sketch is very, very similar to how the Iowa Scaled Engineering CKT-IRSENSE works.
With some filtering, it will turn on and off the Arduino LED (digital I/O 13) in response
to detecting something in close proximity to the sensor.

LICENSE:
    Copyright (C) 2020 Nathan D. Holmes & Michael D. Petersen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*************************************************************************/

#include <Wire.h>
#include <IRSense.h>

IRSense sensor1;

void setup() 
{
  Wire.begin();
  sensor1.begin(Wire);
  pinMode(13, OUTPUT);
}

#define PROXIMITY_THRESHOLD      0x300
#define SENSOR_ERROR_THRESHOLD   2
#define ON_DEBOUNCE_COUNT        1
#define OFF_DEBOUNCE_COUNT       4

bool detect = false;
uint8_t count = 0;
uint8_t sensorErrors = 0;

void setOutput(bool detect)
{
  // This function takes the detection status and performs some action based on it
  // By default, we're just going to turn the Arduino's built in LED on and off
  // On means detecting, off means not detecting.
  
  if (true == detect)
    digitalWrite(13, HIGH);
  else
    digitalWrite(13, LOW);
}


void loop() 
{
  int proximity = sensor1.readProximity();

  if (-1 == proximity)  // -1 from readProximity() is an error state
  {
    // Sensor's gone wonky, reset it and try again
    sensor1.reinitialize();
    
    if (sensorErrors < 255)
      sensorErrors++;

    if (sensorErrors > SENSOR_ERROR_THRESHOLD)
    {
       detect = false;
       count = 0;
       setOutput(detect);
    }

    // Skip the rest of processing, we'll go around the loop again
    // and try to get a good read next time
    return; 
  }

  // Good sensor reading
  sensorErrors = 0;

  if ((false == detect) & (proximity >= PROXIMITY_THRESHOLD))
  {
    // If we're not detecting and the proximity is closer (higher) than the threshold value,
    //  wait until we have ON_DEBOUNCE_COUNT number of closer readings and set detection
    count++;
    if(count > ON_DEBOUNCE_COUNT)
    {
      detect = true;
      count = 0;
    }

  } else if (false == detect & (proximity < PROXIMITY_THRESHOLD)) {
    // If we're not detecting and the proximity is further (lower) than the threshold value,
    // reset the debounce counter to assure that anomalous detections don't accumulate
    count = 0;

  } else if ((true == detect) & (proximity < PROXIMITY_THRESHOLD)) {
    // If we're detecting and the proximity is further (lower) than the threshold value,
    //  wait until we have OFF_DEBOUNCE_COUNT number of further readings and then drop detection
    count++;
    if(count > OFF_DEBOUNCE_COUNT)
    {
      detect = false;
      count = 0;
    }
  } else if (true == detect & (proximity >= PROXIMITY_THRESHOLD)) {
    // If we're detecting and the proximity is closer (higher) than the threshold value,
    // reset the debounce counter to assure that anomalous detection dropouts don't accumulate

    count = 0;
  }

  setOutput(detect);

  delay(100);
}
