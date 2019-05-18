#include <Adafruit_SPIDevice.h>
#include <Arduino.h>

//#define DEBUG_SERIAL Serial

Adafruit_SPIDevice::Adafruit_SPIDevice(int8_t cspin, uint32_t freq, BitOrder dataOrder, uint8_t dataMode, SPIClass *theSPI) {
  _cs = cspin;
  _spi = theSPI;
  _begun = false;
  _spiSetting = new SPISettings(freq, dataOrder, dataMode);
}

bool Adafruit_SPIDevice::begin(void) {
  _spi->begin();
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);
  _begun = true;
  return true;
}

bool Adafruit_SPIDevice::write(uint8_t *buffer, size_t len, uint8_t *prefix_buffer, size_t prefix_len) {



  return true;
}

bool Adafruit_SPIDevice::read(uint8_t *buffer, size_t len, uint8_t sendvalue) {
  memset(buffer, sendvalue, len);  // clear out existing buffer
  _spi->beginTransaction(*_spiSetting);
  digitalWrite(_cs, LOW);
  _spi->transfer(buffer, len); 
  digitalWrite(_cs, HIGH);
  _spi->endTransaction();

  return true;
}



bool Adafruit_SPIDevice::write_then_read(uint8_t *write_buffer, size_t write_len, uint8_t *read_buffer, size_t read_len, uint8_t sendvalue) {
  _spi->beginTransaction(*_spiSetting);
  digitalWrite(_cs, LOW);
  // do the writing
  for (int i=0; i<write_len; i++) {
    _spi->transfer(write_buffer[i]);
  }
  // do the reading
  for (int i=0; i<read_len; i++) {
    read_buffer[i] = _spi->transfer(sendvalue);
  }
  digitalWrite(_cs, HIGH);
  _spi->endTransaction();

  return true;
}
