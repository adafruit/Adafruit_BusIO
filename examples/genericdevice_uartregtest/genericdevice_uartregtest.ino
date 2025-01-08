/*
   Advanced example of using bstracted transport for reading and writing 
   register data from a UART-based device such as a TMC2209

   Written with help by Claude! https://claude.ai/chat/335f50b1-3dd8-435e-9139-57ec7ca26a3c
  (at this time chats are not shareable :(
*/


#include "Adafruit_BusIO_Register.h"
#include "Adafruit_GenericDevice.h"

// Debugging macros
//#define DEBUG_SERIAL Serial

#ifdef DEBUG_SERIAL
#define DEBUG_PRINT(x) DEBUG_SERIAL.print(x)
#define DEBUG_PRINTLN(x) DEBUG_SERIAL.println(x)
#define DEBUG_PRINT_HEX(x)                                                     \
  do {                                                                         \
    if (x < 0x10)                                                              \
      DEBUG_SERIAL.print('0');                                                 \
    DEBUG_SERIAL.print(x, HEX);                                                \
    DEBUG_SERIAL.print(' ');                                                   \
  } while (0)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT_HEX(x)
#endif

// Add IOIN register definition
#define TMC2209_IOIN 0x06

class TMC2209_UART {
private:
  static TMC2209_UART *_instance;
  Stream *_uart_stream;
  uint8_t _addr;

  static bool uart_read_impl(uint8_t *buffer, size_t len) {
    return _instance->uart_read_fn(buffer, len);
  }

  static bool uart_write_impl(const uint8_t *buffer, size_t len) {
    return _instance->uart_write_fn(buffer, len);
  }

  static bool uart_readreg_impl(uint8_t *addr_buf, uint8_t addrsiz,
                                uint8_t *data, uint16_t datalen) {
    return _instance->uart_readreg_fn(addr_buf, addrsiz, data, datalen);
  }

  static bool uart_writereg_impl(uint8_t *addr_buf, uint8_t addrsiz,
                                 const uint8_t *data, uint16_t datalen) {
    return _instance->uart_writereg_fn(addr_buf, addrsiz, data, datalen);
  }

  bool uart_read_fn(uint8_t *buffer, size_t len) {
    uint16_t timeout = 100;
    while (_uart_stream->available() < len && timeout--) {
      delay(1);
    }
    if (timeout == 0) {
      DEBUG_PRINTLN("Read timeout!");
      return false;
    }

    DEBUG_PRINT("Reading: ");
    for (size_t i = 0; i < len; i++) {
      buffer[i] = _uart_stream->read();
      DEBUG_PRINT_HEX(buffer[i]);
    }
    DEBUG_PRINTLN("");

    return true;
  }

  bool uart_write_fn(const uint8_t *buffer, size_t len) {
    DEBUG_PRINT("Writing: ");
    for (size_t i = 0; i < len; i++) {
      DEBUG_PRINT_HEX(buffer[i]);
    }
    DEBUG_PRINTLN("");

    _uart_stream->write(buffer, len);
    return true;
  }

  bool uart_readreg_fn(uint8_t *addr_buf, uint8_t addrsiz, uint8_t *data,
                       uint16_t datalen) {
    while (_uart_stream->available())
      _uart_stream->read();

    uint8_t packet[4] = {0x05, uint8_t(_addr << 1), addr_buf[0], 0x00};

    packet[3] = calcCRC(packet, 3);
    if (!uart_write_impl(packet, 4))
      return false;

    // Read back echo
    uint8_t echo[4];
    if (!uart_read_impl(echo, 4))
      return false;

    // Verify echo
    for (uint8_t i = 0; i < 4; i++) {
      if (echo[i] != packet[i]) {
        DEBUG_PRINTLN("Echo mismatch");
        return false;
      }
    }

    uint8_t response[8]; // sync + 0xFF + reg + 4 data bytes + CRC
    if (!uart_read_impl(response, 8))
      return false;

    // Verify response
    if (response[0] != 0x05) {
      DEBUG_PRINTLN("Invalid sync byte");
      return false;
    }

    // Verify 0xFF address byte
    if (response[1] != 0xFF) {
      DEBUG_PRINTLN("Invalid reply address");
      return false;
    }

    // Verify register address matches our request
    if (response[2] != addr_buf[0]) {
      DEBUG_PRINTLN("Register mismatch");
      return false;
    }

    // Verify CRC
    uint8_t crc = calcCRC(response, 7); // Calculate CRC of all but last byte
    if (crc != response[7]) {
      DEBUG_PRINTLN("CRC mismatch");
      return false;
    }

    // Copy the data bytes
    memcpy(data, &response[3], 4);

    return true;
  }

  bool uart_writereg_fn(uint8_t *addr_buf, uint8_t addrsiz, const uint8_t *data,
                        uint16_t datalen) {
    while (_uart_stream->available())
      _uart_stream->read();

    uint8_t packet[8] = {0x05,
                         uint8_t(_addr << 1),
                         uint8_t(addr_buf[0] | 0x80),
                         data[0],
                         data[1],
                         data[2],
                         data[3],
                         0x00};

    packet[7] = calcCRC(packet, 7);
    if (!uart_write_impl(packet, 8))
      return false;

    // Read and verify echo
    uint8_t echo[8];
    if (!uart_read_impl(echo, 8))
      return false;

    // Verify echo matches what we sent
    for (uint8_t i = 0; i < 8; i++) {
      if (echo[i] != packet[i]) {
        DEBUG_PRINTLN("Write echo mismatch");
        return false;
      }
    }

    return true;
  }

  static uint8_t calcCRC(uint8_t *data, uint8_t length) {
    uint8_t crc = 0;
    for (uint8_t i = 0; i < length; i++) {
      uint8_t currentByte = data[i];
      for (uint8_t j = 0; j < 8; j++) {
        if ((crc >> 7) ^ (currentByte & 0x01)) {
          crc = (crc << 1) ^ 0x07;
        } else {
          crc = crc << 1;
        }
        currentByte = currentByte >> 1;
      }
    }
    return crc;
  }

public:
  TMC2209_UART(Stream *serial, uint8_t addr)
      : _uart_stream(serial), _addr(addr) {
    _instance = this;
  }

  Adafruit_GenericDevice *createDevice() {
    return new Adafruit_GenericDevice(uart_read_impl, uart_write_impl,
                                      uart_readreg_impl, uart_writereg_impl);
  }
};

TMC2209_UART *TMC2209_UART::_instance = nullptr;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;
  delay(100);
  Serial.println("TMC2209 Generic Device register read/write test!");

  Serial1.begin(115200);

  TMC2209_UART uart(&Serial1, 0);
  Adafruit_GenericDevice *device = uart.createDevice();
  device->begin();

  // Create register object for IOIN
  Adafruit_BusIO_Register ioin_reg(device,
                                   TMC2209_IOIN, // device and register address
                                   4,            // width = 4 bytes
                                   MSBFIRST,     // byte order
                                   1);           // address width = 1 byte
  Serial.print("IOIN = 0x");
  Serial.println(ioin_reg.read(), HEX);

  // Create RegisterBits for VERSION field (bits 28:24)
  Adafruit_BusIO_RegisterBits version_bits(
      &ioin_reg, 8, 24); // 8 bits wide, starting at bit 24

  Serial.println("Reading VERSION...");
  uint8_t version = version_bits.read();

  Serial.print("VERSION = 0x");
  Serial.println(version, HEX);
}

void loop() { delay(1000); }
