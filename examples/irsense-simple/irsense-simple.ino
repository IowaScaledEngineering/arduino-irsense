/*************************************************************************
Title:    Arduino I2C-IRSENSE Simple Example
Authors:  Nathan D. Holmes <maverick@drgw.net>
File:     $Id: $
License:  GNU General Public License v3

This is a very simple example of how to use the Iowa Scaled Engineering
I2C-IRSENSE ( https://www.iascaled.com/store/I2C-IRSENSE ) with the
IRSense library.

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
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  Wire.begin();
  boolean sensor1Initialized = sensor1.begin(Wire);

  while(!Serial);

  if (sensor1Initialized)
    Serial.print("Sensor successfully initialized!\n");
  else
    Serial.print("Sensor initialization FAILURE!\n");
}

void loop() 
{
  // If nothing seems to be working, uncomment this.  It will enumerate all devices on the I2C bus and
  //  print out their addresses.  At least then you can see if the sensor is even responding.  If it's not
  //  there's a good chance you've got some sort of hardware problem
  /*
  while(!Serial.available())
  {
    enumerateI2C(false);
    delay(1000);
  }*/

  int proximity = sensor1.readProximity();

  Serial.print("proximity = ");
  Serial.print(proximity);
  Serial.print("\n");
  delay(100);
}

byte enumerateI2C(boolean showErrors)
{
  byte addr, stat, last_ack = 0x00;
 
  Serial.print("Enumerating all I2C devices\n (Press any key to stop scanning I2C bus)\n-------------\n");
 
  for(addr=0x00; addr<0x7F; addr++)
  {
    Wire.beginTransmission(addr);
    stat = Wire.endTransmission();
    if(stat)
    {
      if(showErrors)
      {
        Serial.print("0x");
        Serial.print(addr, HEX);
        Serial.print(": ");
        Serial.print(stat);
        Serial.print("\n");
      }
    }  
    else
    {   
      Serial.print("0x");
      Serial.print(addr, HEX);
      Serial.print(" ");
      last_ack = addr;   
    }
  }  
  Serial.print("\n");
  return last_ack;
}
 
