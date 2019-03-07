#include <Adafruit_I2CDevice.h>
#include <Arduino.h>

//#define DEBUG_SERIAL Serial

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

bool Adafruit_I2CDevice::write(uint8_t *buffer, size_t len, bool stop) {
  if (len > 32) {
    // currently not guaranteed to work if more than 32 bytes!
    // we will need to find out if some platforms have larger
    // I2C buffer sizes :/
#ifdef DEBUG_SERIAL
    DEBUG_SERIAL.println(F("\tI2CDevice could not write such a large buffer"));
#endif
    return false;
  }

  _wire->beginTransmission(_addr);

  if (_wire->write(buffer, len) != len) {
#ifdef DEBUG_SERIAL
    DEBUG_SERIAL.println(F("\tI2CDevice failed to write"));
#endif
    return false;
  }

#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.print(F("\tI2CDevice Wrote: "));
  for (int i=0; i<len; i++) {
    DEBUG_SERIAL.print(F("0x")); 
    DEBUG_SERIAL.print(buffer[i], HEX); 
    DEBUG_SERIAL.print(F(", "));
    if (len % 32 == 31) {
      DEBUG_SERIAL.println();
    }
  }
  DEBUG_SERIAL.println();
#endif


  return (_wire -> endTransmission(stop) == 0);
}

bool Adafruit_I2CDevice::read(uint8_t *buffer, size_t len, bool stop) {
  if (len > 32) {
    // currently not guaranteed to work if more than 32 bytes!
    // we will need to find out if some platforms have larger
    // I2C buffer sizes :/
#ifdef DEBUG_SERIAL
    DEBUG_SERIAL.println(F("\tI2CDevice could not read such a large buffer"));
#endif
    return false;
  }

  if (_wire->requestFrom(_addr, (uint8_t)len, stop) != len) {
    // Not enough data available to fulfill our obligation!
#ifdef DEBUG_SERIAL
    DEBUG_SERIAL.println(F("\tI2CDevice did not receive enough data"));
#endif
    return false;
  }

  for (uint16_t i=0; i<len; i++) {
    buffer[i] = _wire->read();
  }

#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.print(F("\tI2CDevice Read: "));
  for (int i=0; i<len; i++) {
    DEBUG_SERIAL.print(F("0x")); 
    DEBUG_SERIAL.print(buffer[i], HEX); 
    DEBUG_SERIAL.print(F(", "));
    if (len % 32 == 31) {
      DEBUG_SERIAL.println();
    }
  }
  DEBUG_SERIAL.println();
#endif

  return true;
}


bool Adafruit_I2CDevice::write_then_read(uint8_t *write_buffer, size_t write_len, uint8_t *read_buffer, size_t read_len, bool stop) {
  if (! write(write_buffer, write_len, stop)) {
    return false;
  }
  
  return read(read_buffer, read_len);
}



uint8_t Adafruit_I2CDevice::address(void) {
  return _addr;
}
