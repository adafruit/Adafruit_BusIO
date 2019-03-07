#include <Adafruit_I2CDevice.h>
#include <Arduino.h>

Adafruit_I2CDevice::Adafruit_I2CDevice(uint8_t addr, TwoWire *theWire) {
  _addr = addr;
  _wire = theWire;
  _begun = false;
}

bool Adafruit_I2CDevice::begin(void) {
  _wire->begin();
  _begun = true;

  return detected();
}

bool Adafruit_I2CDevice::detected(void) {
  // Init I2C if not done yet
  if (!_begun && !begin()) {
    return false;
  }

  // A basic scanner, see if it ACK's
  _wire->beginTransmission(_addr);
  if (_wire->endTransmission () == 0) {
    return true;
  }
  return false;
}

uint8_t Adafruit_I2CDevice::address(void) {
  return _addr;
}
