#include <Wire.h>

class Adafruit_I2CDevice {

private:
  uint8_t _addr;
  TwoWire *_wire;
  bool _begun;

public:
  Adafruit_I2CDevice(uint8_t addr, TwoWire *theWire=&Wire);
  uint8_t address(void);
  bool begin(void);
  bool detected(void);
};
