#include <Adafruit_SPIDevice.h>
#include <Arduino.h>

//#define DEBUG_SERIAL Serial

/*!
 *    @brief  Create an SPI device with the given CS pin and settins
 *    @param  cspin The arduino pin number to use for chip select
 *    @param  freq The SPI clock frequency to use, defaults to 1MHz
 *    @param  dataOrder The SPI data order to use for bits within each byte, defaults to SPI_MSBFIRST
 *    @param  dataMode The SPI mode to use, defaults to SPI_MODE0
 *    @param  theSPI The SPI bus to use, defaults to &theSPI
 */
Adafruit_SPIDevice::Adafruit_SPIDevice(int8_t cspin, uint32_t freq, BitOrder dataOrder, uint8_t dataMode, SPIClass *theSPI) {
  _cs = cspin;
  _spi = theSPI;
  _begun = false;
  _spiSetting = new SPISettings(freq, dataOrder, dataMode);
}

/*!
 *    @brief  Initializes SPI bus and sets CS pin high
 *    @return Always returns true because there's no way to test success of SPI init
 */
bool Adafruit_SPIDevice::begin(void) {
  _spi->begin();
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);
  _begun = true;
  return true;
}

/*!
 *    @brief  Write a buffer or two to the SPI device.
 *    @param  buffer Pointer to buffer of data to write
 *    @param  len Number of bytes from buffer to write
 *    @param  prefix_buffer Pointer to optional array of data to write before buffer.
 *    @param  prefix_len Number of bytes from prefix buffer to write
 *    @return Always returns true because there's no way to test success of SPI writes
 */
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

/*!
 *    @brief  Read from SPI into a buffer from the SPI device.
 *    @param  buffer Pointer to buffer of data to read into
 *    @param  len Number of bytes from buffer to read.
 *    @param  sendvalue The 8-bits of data to write when doing the data read, defaults to 0xFF
 *    @return Always returns true because there's no way to test success of SPI writes
 */
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


/*!
 *    @brief  Write some data, then read some data from SPI into another buffer. The buffers can point to same/overlapping locations. This does not transmit-receive at the same time!
 *    @param  write_buffer Pointer to buffer of data to write from
 *    @param  write_len Number of bytes from buffer to write.
 *    @param  read_buffer Pointer to buffer of data to read into.
 *    @param  read_len Number of bytes from buffer to read.
 *    @param  sendvalue The 8-bits of data to write when doing the data read, defaults to 0xFF
 *    @return Always returns true because there's no way to test success of SPI writes
 */
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
