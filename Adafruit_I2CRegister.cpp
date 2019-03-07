#include <Adafruit_I2CRegister.h>

Adafruit_I2CRegister::Adafruit_I2CRegister(Adafruit_I2CDevice *device, uint16_t reg_addr, uint8_t width, uint8_t bitorder, uint8_t address_width) {
  _device = device;
  _addrwidth = address_width;
  _address = reg_addr;
  _bitorder = bitorder;
  _width = width;
}


bool Adafruit_I2CRegister::write(uint8_t *buffer, uint8_t len) {
  uint8_t addrbuffer[2] = {_address & 0xFF, _address >> 8};
  if (! _device->write(buffer, len, true, addrbuffer, _addrwidth)) {
    return false;
  }
  return true;
}

bool Adafruit_I2CRegister::write(uint32_t value, uint8_t numbytes=0) {
  if (numbytes == 0) {
    numbytes = _width;
  }
  if (numbytes > 4) {
    return false;
  }

  for (int i=0; i<numbytes; i++) {
    if (_bitorder == LSBFIRST) {
      _buffer[i] = value & 0xFF;
    } else {
      _buffer[numbytes-i-1] = value & 0xFF;
    }
    value >>= 8;
  }
  return write(_buffer, numbytes);
}

bool Adafruit_I2CRegister::read(uint8_t *buffer, uint8_t len) {
  _buffer[0] = _address;
  if (! _device->write_then_read(_buffer, 1, buffer, len)) {
    return false;
  }
  return true;
}

bool Adafruit_I2CRegister::read(uint16_t *value) {
  if (! read(_buffer, 2)) {
    return false;
  }

  if (_bitorder == LSBFIRST) {
    *value = _buffer[1];
    *value <<= 8;
    *value |= _buffer[0];
  } else {
    *value = _buffer[0];
    *value <<= 8;
    *value |= _buffer[1];
  }
  return true;
}

bool Adafruit_I2CRegister::read(uint8_t *value) {
  if (! read(_buffer, 1)) {
    return false;
  }

  *value = _buffer[0];
  return true;
}
