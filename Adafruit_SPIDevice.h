#include <SPI.h>

#ifndef Adafruit_SPIDevice_h
#define Adafruit_SPIDevice_h

#if !defined(BitOrder)       // modern SPI definitions have a BitOrder enum
typedef uint8_t BitOrder;
#endif

class Adafruit_SPIDevice {
 public:
  Adafruit_SPIDevice(int8_t cspin,
		     uint32_t freq=1000000, 
		     BitOrder dataOrder=MSBFIRST, 
		     uint8_t dataMode=SPI_MODE0,
		     SPIClass *theSPI=&SPI);
  bool begin(void);
  bool read(uint8_t *buffer, size_t len, uint8_t sendvalue=0xFF);
  bool write(uint8_t *buffer, size_t len, uint8_t *prefix_buffer=NULL, size_t prefix_len=0);
  bool write_then_read(uint8_t *write_buffer, size_t write_len, uint8_t *read_buffer, size_t read_len, uint8_t sendvalue=0xFF);

 private:
  SPIClass *_spi;
  SPISettings *_spiSetting;
  int8_t _cs;
  bool _begun;
};

#endif // Adafruit_SPIDevice_h
