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
  _spi->beginTransaction(*_spiSetting);
  digitalWrite(_cs, LOW);
  // do the writing
  for (size_t i=0; i<prefix_len; i++) {
    _spi->transfer(prefix_buffer[i]);
  }
  for (size_t i=0; i<len; i++) {
    _spi->transfer(buffer[i]);
  }
  digitalWrite(_cs, HIGH);
  _spi->endTransaction();


#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.print(F("\tSPIDevice Wrote: "));
  if ((prefix_len != 0) && (prefix_buffer != NULL)) {
    for (uint16_t i=0; i<prefix_len; i++) {
      DEBUG_SERIAL.print(F("0x")); 
      DEBUG_SERIAL.print(prefix_buffer[i], HEX); 
      DEBUG_SERIAL.print(F(", "));
    }
  }
  for (uint16_t i=0; i<len; i++) {
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

bool Adafruit_SPIDevice::read(uint8_t *buffer, size_t len, uint8_t sendvalue) {
  memset(buffer, sendvalue, len);  // clear out existing buffer
  _spi->beginTransaction(*_spiSetting);
  digitalWrite(_cs, LOW);
  _spi->transfer(buffer, len); 
  digitalWrite(_cs, HIGH);
  _spi->endTransaction();

#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.print(F("\tSPIDevice Read: "));
  for (uint16_t i=0; i<len; i++) {
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



bool Adafruit_SPIDevice::write_then_read(uint8_t *write_buffer, size_t write_len, uint8_t *read_buffer, size_t read_len, uint8_t sendvalue) {
  _spi->beginTransaction(*_spiSetting);
  digitalWrite(_cs, LOW);
  // do the writing
  for (size_t i=0; i<write_len; i++) {
    _spi->transfer(write_buffer[i]);
  }

#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.print(F("\tSPIDevice Wrote: "));
  for (uint16_t i=0; i<write_len; i++) {
    DEBUG_SERIAL.print(F("0x")); 
    DEBUG_SERIAL.print(write_buffer[i], HEX); 
    DEBUG_SERIAL.print(F(", "));
    if (write_len % 32 == 31) {
      DEBUG_SERIAL.println();
    }
  }
  DEBUG_SERIAL.println();
#endif

  // do the reading
  for (size_t i=0; i<read_len; i++) {
    read_buffer[i] = _spi->transfer(sendvalue);
  }

#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.print(F("\tSPIDevice Read: "));
  for (uint16_t i=0; i<read_len; i++) {
    DEBUG_SERIAL.print(F("0x")); 
    DEBUG_SERIAL.print(read_buffer[i], HEX); 
    DEBUG_SERIAL.print(F(", "));
    if (read_len % 32 == 31) {
      DEBUG_SERIAL.println();
    }
  }
  DEBUG_SERIAL.println();
#endif

  digitalWrite(_cs, HIGH);
  _spi->endTransaction();

  return true;
}
