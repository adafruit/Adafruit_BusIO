#include "Adafruit_GenericDevice.h"

Adafruit_GenericDevice::Adafruit_GenericDevice(
    busio_genericdevice_read_t read_func,
    busio_genericdevice_write_t write_func,
    busio_genericdevice_readreg_t readreg_func,
    busio_genericdevice_writereg_t writereg_func) {
  _read_func = read_func;
  _write_func = write_func;
  _readreg_func = readreg_func;
  _writereg_func = writereg_func;
  _begun = false;
}

bool Adafruit_GenericDevice::begin(void) {
  _begun = true;
  return true;
}

bool Adafruit_GenericDevice::write(const uint8_t *buffer, size_t len) {
  if (!_begun)
    return false;

  return _write_func(buffer, len);
}

bool Adafruit_GenericDevice::read(uint8_t *buffer, size_t len) {
  if (!_begun)
    return false;

  return _read_func(buffer, len);
}

bool Adafruit_GenericDevice::readRegister(uint8_t *addr_buf, uint8_t addrsiz,
                                          uint8_t *buf, uint16_t bufsiz) {
  if (!_begun || !_readreg_func)
    return false;

  return _readreg_func(addr_buf, addrsiz, buf, bufsiz);
}

bool Adafruit_GenericDevice::writeRegister(uint8_t *addr_buf, uint8_t addrsiz,
                                           const uint8_t *buf,
                                           uint16_t bufsiz) {
  if (!_begun || !_writereg_func)
    return false;

  return _writereg_func(addr_buf, addrsiz, buf, bufsiz);
}
