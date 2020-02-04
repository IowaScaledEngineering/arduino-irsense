/*************************************************************************
Title:    Arduino I2C-IRSENSE Library Header
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

#ifndef _IRSENSE_H_
#define _IRSENSE_H_

#include <Wire.h>
#include <Arduino.h>


class IRSense
{
  public:
    IRSense();
    bool begin(uint8_t tmd26721Address = 0x39);
    bool begin(TwoWire& i2cInterface, uint8_t tmd26721Address = 0x39);
    bool setControlRegister(uint8_t cmd, uint8_t val);
    bool reinitialize();
    int readProximity();
    bool proximityReady();

    enum PDriveStrength { LED_100_PERCENT = 0x00, LED_50_PERCENT = 0x40, LED_25_PERCENT = 0x80, LED_12_PERCENT = 0xC0 };
    enum PLEDChannel { LED_NONE = 0x00, LED_CHANNEL_0 = 0x10, LED_CHANNEL_1 = 0x20 };
    enum PGain { PROX_GAIN_1X = 0x00, PROX_GAIN_2X = 0x04, PROX_GAIN_4X = 0x08, PROX_GAIN_8X = 0x0C };

    bool setPulseCount(uint8_t pulseCount);
    bool setControlValues(PLEDChannel ledChannel, PDriveStrength ledStrength, PGain sensorGain);
 
  private:
    TwoWire* i2cInterface;
    bool initialized;
    uint8_t tmd26721Address;
    bool writeByte(uint8_t cmd, uint8_t val);
    bool readWord(uint8_t cmd, uint16_t* val);
    
    PDriveStrength param_ledStrength;
    PLEDChannel param_ledChannel;
    PGain param_proximityGain;
    uint8_t param_pulseCount;    
};

#endif

