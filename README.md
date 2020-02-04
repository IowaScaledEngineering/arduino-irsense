# Arduino Library for the Iowa Scaled Engineering I2C-IRSENSE

### Summary

The [Iowa Scaled Engineering I2C-IRSENSE](https://www.iascaled.com/store/I2C-IRSENSE) is a [QWIIC-compatible I2C](https://www.sparkfun.com/qwiic) infrared proximity sensor based
on the [AMS TMD26721](https://ams.com/tmd26721) and packaged in a small, convenient form factor.  While we
largely intend them for model railroading use, the sensor is versatile and can be used anywhere that a simple
reflective proximity sensor is needed.

The board comes equipped with a single [QWIIC-compatible](https://www.sparkfun.com/qwiic) JST connector.  Because
the part only has a single I2C address (0x39), it's only possible to have a single unit on an I2C bus at a time without
using something like an [I2C address changer](https://www.iascaled.com/store/I2C-XLATE).

-----

### Operation

The Golden Rule:  When in doubt, check [the TMD26721 datasheet](https://ams.com/documents/20143/36005/TMD2672_DS000342_1-00.pdf)  This guide is not designed to be a comprehensive guide to the TMD26721.

-----

### Library Functions

This is the Arduino-compatible control library for the I2C-IRSENSE.  

The sensor is represented by an IRSense class.

#### IRSense Class Methods

##### bool begin(uint8_t tmd26721Address = 0x39)
##### bool begin(TwoWire& i2cInterface, uint8_t tmd26721Address = 0x39)

Before using the IRSense object, begin() must be called once. 

__Arguments:__
* If called with no arguments, it will initialize to use the Wire I2C object and the default address of 0x39 for the sensor.
* Optionally, it can be called with a different address for the sensor if you're using an I2C address changer.
* Optionally for boards where the sensor is not attached to the default Wire interface (such as if it's attached to Wire1), an alternate Wire interface object can be passed as well.

__Return values:__
* true - initialization was successful
* false - initialization failed

##### int readProximity()

The core function of the sensor.  This will read the proximity registers and return as an integer if a valid conversion is available.  If not, it waits up to 15ms for PVALID in the status register to be set.  If the sensor has had an error or isn't initialized, it will try to reinitialize the sensor.  

__Return values:__
* -1 = Sensor communications error
* 0-1023 = Value in the proximity register

##### bool reinitialize()

Attempts to reinitialize the sensor in the event of an error.

__Return values:__
* true - initialization was successful
* false - initialization failed

##### bool proximityReady()

Polls the status register (0x13) and gets the value of PVALID, indicating that a valid proximity reading.

__Return values:__
* true - reading is valid and available
* false - reading is not valid, or communications failed

##### bool setControlValues(PLEDChannel ledChannel, PDriveStrength ledStrength, PGain sensorGain)

The key parameters of the proximity sensor are the LED channel to use, how strongly to drive the LED, and how much gain to apply to the sensor reading.  See [the TMD26721 datasheet](https://ams.com/documents/20143/36005/TMD2672_DS000342_1-00.pdf) for full details on how to use these.  Our usual recommended values for model railroad applications are channel 1 (more immune to flourescent lighting), 100% power, and 4x gain, and so these are the library defaults.  However they may not be optimal for all applications, and so a function is provided to allow you to change them.

__Arguments:__

ledChannel must be one of:
* IRSense::LED_NONE - Turn off all LEDs.  Essentially makes the sensor useless, but significantly decreases power use
* IRSense::LED_CHANNEL_0 - Use LED Channel 0.  
* IRSense::LED_CHANNEL_1 - Use LED Channel 1 (default).

ledStrength must be one of:
* IRSense::LED_100_PERCENT - 100% LED power (default)
* IRSense::LED_50_PERCENT - 50% LED power
* IRSense::LED_25_PERCENT - 25% LED power
* IRSense::LED_12_PERCENT - 12.5% LED power

sensorGain must be one of:
* IRSense::PROX_GAIN_1X - 1x gain
* IRSense::PROX_GAIN_2X - 2x gain
* IRSense::PROX_GAIN_4X - 4x gain (default)
* IRSense::PROX_GAIN_8X - 8x gain

__Return values:__
* true - configuration change was successful
* false - configuration change or communications failed

##### bool setPulseCount(uint8_t pulseCount)

Sets the number of IR pulses sent out for each reading.  See the datasheet for more details on how this affects readings.

__Arguments:__

pulseCount must be 1-255, defaults to 8

__Return values:__
* true - configuration change was successful
* false - configuration change or communications failed   

### Hardware

The I2C-IRSENSE hardware design files can be found as part of the [ISE CKT-IRSENSE project](https://github.com/IowaScaledEngineering/ckt-irsense).  Assembled and tested parts can be purchased from the [ISE store](https://www.iascaled.com/store/I2C-IRSENSE).

### License

Like all public Iowa Scaled Engineering designs, the I2C-IRSENSE is both open hardware and open source software.  Please refer to the hardware and software license files in this repository for details.
