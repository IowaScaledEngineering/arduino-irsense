/*************************************************************************
Title:    Arduino I2C-IRSENSE Library
Authors:  Nathan D. Holmes <maverick@drgw.net>
File:     $Id: $
License:  GNU General Public License v3

This is an Arduino-compatible TMD26721 proximity sensor library for the 
Iowa Scaled Engineering I2C-IRSENSE sensor head.   For more information about the
I2C-IRSENSE, see here:  https://www.iascaled.com/store/I2C-IRSENSE

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

#include <IRSense.h>

IRSense::IRSense()
{
  this->initialized = false;
  this->tmd26721Address = 0x39;
  this->i2cInterface = NULL;

  /* Initialize sensor parameters to defaults */
  this->param_pulseCount = 8;
  this->param_ledStrength = LED_100_PERCENT;
  this->param_ledChannel = LED_CHANNEL_1;
  this->param_proximityGain = PROX_GAIN_4X;
}

bool IRSense::begin(uint8_t tmd26721Address)
{
  return this->begin(Wire, tmd26721Address);
}


bool IRSense::writeByte(uint8_t cmd, uint8_t val)
{
  this->i2cInterface->beginTransmission(this->tmd26721Address);
  this->i2cInterface->write(cmd);
  this->i2cInterface->write(val);
  if(0 != this->i2cInterface->endTransmission(true))
    return false;

  return true;
}

bool IRSense::proximityReady()
{
  
  this->i2cInterface->beginTransmission(this->tmd26721Address);
  this->i2cInterface->write(0x80 | 0x13);
  if (0 != this->i2cInterface->endTransmission(false))
    return false;
  
  if (1 != this->i2cInterface->requestFrom(this->tmd26721Address, (uint8_t)1))
  {
    // Read error, got less than the expected byte
    return false;
  }

  return (bool)(0x02 & this->i2cInterface->read());
}

bool IRSense::readWord(uint8_t cmd, uint16_t* val)
{
  *val = 0;
  
  this->i2cInterface->beginTransmission(this->tmd26721Address);
  this->i2cInterface->write(cmd);
  if (0 != this->i2cInterface->endTransmission(false))
    return false;
  
  if (2 != this->i2cInterface->requestFrom(this->tmd26721Address, (uint8_t)2))
  {
    // Read error, got less than the expected 2 bytes
    return false;
  }

  *val = this->i2cInterface->read();
  *val |= ((uint16_t)this->i2cInterface->read() << 8);
 
  return true;
}

bool IRSense::reinitialize()
{
  this->initialized = true;
  
  // Initialize TMD26711 (bit 0x80 set to indicate command)
  this->initialized &= this->writeByte(0x80|0x00, 0x00);   // Start with everything disabled
  this->initialized &= this->writeByte(0x80|0x01, 0xFF);   // Minimum ATIME
  this->initialized &= this->writeByte(0x80|0x02, 0xFF);   // Maximum integration time
  this->initialized &= this->writeByte(0x80|0x03, 0xFF);   // Minimum wait time
  
  // Note: IRQ not currently used
  this->initialized &= this->writeByte(0x80|0x08, 0x00);   // Set interrupt low threshold to 0x0000
  this->initialized &= this->writeByte(0x80|0x09, 0x00);
  this->initialized &= this->writeByte(0x80|0x0A, 0x00);   // Set interrupt low threshold to 0x0300
  this->initialized &= this->writeByte(0x80|0x0B, 0x03);
  this->initialized &= this->writeByte(0x80|0x0C, 0x10);   // Single out-of-range cycle triggers interrupt

  this->initialized &= this->writeByte(0x80|0x0D, 0x00);   // Long wait disabled
  this->initialized &= this->writeByte(0x80|0x0E, this->param_pulseCount); // Pulse count
  this->initialized &= this->writeByte(0x80|0x0F, (uint8_t)this->param_ledStrength | (uint8_t)param_ledChannel | (uint8_t)param_proximityGain);   // 100% LED drive strength, 4x gain, Use channel 1 diode (ch 1 seems less sensitive to fluorescent) light)

  this->initialized &= this->writeByte(0x80|0x00, 0x27);   // Power ON, Enable proximity, Enable proximity interrupt (not used currently)

  return(this->initialized);
}

bool IRSense::setPulseCount(uint8_t pulseCount)
{
  this->param_pulseCount = pulseCount;
  return this->reinitialize();
}

bool IRSense::setControlValues(PLEDChannel ledChannel, PDriveStrength ledStrength, PGain sensorGain)
{
  this->param_ledStrength = ledStrength;
  this->param_ledChannel = ledChannel;
  this->param_proximityGain = sensorGain;
  return this->reinitialize();
}

bool IRSense::setControlRegister(uint8_t cmd, uint8_t val)
{
  return this->writeByte(0x80|cmd, val);
}

bool IRSense::begin(TwoWire& i2cInterface, uint8_t tmd26721Address)
{
  this->i2cInterface = &i2cInterface;
  this->initialized = this->reinitialize();
  return(this->initialized);
}

int IRSense::readProximity()
{
  uint16_t proximity = 0;
  uint8_t retries = 0;
  uint8_t maxRetries = 5;
  bool readSuccess;
  
  if (false == this->initialized)
  {
    this->reinitialize();
    if (false == this->initialized)
      return -1;
  }

  // Wait for the sensor to indicate it's ready if we've reinitialized
  while(retries++ < maxRetries && !this->proximityReady())
  {
    delay(3);
  }

  // Sensor timed out or failed
  if (retries == maxRetries)
    return -1;
  
  readSuccess = this->readWord(0x80|0x20|0x18, &proximity);  // Read data register (0x80 = command, 0x20 = auto-increment)

  if (false == readSuccess)
  {
    this->initialized = false;
    return -1;
  }
    
  return proximity;
}

