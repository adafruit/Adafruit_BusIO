/*!
 * @file Adafruit_I2CDevice.h
 */
#include <Wire.h>

#ifndef Adafruit_I2CDevice_h
#define Adafruit_I2CDevice_h

///< The class which defines how we will talk to this device over I2C
class Adafruit_I2CDevice {
public:
  Adafruit_I2CDevice(uint8_t addr, TwoWire *theWire = &Wire);
  uint8_t address(void);
  bool begin(bool addr_detect = true);
  bool detected(void);

  bool read(uint8_t *buffer, size_t len, bool stop = true);
  /**************************************************************************/
  /*!
   * @brief  Writes to a I2C device register.
   *
   * @param  buffer
   *         A pointer to a buffer containing data bytes.
   *         
   * @param  len
   *         Number of bytes to write from the previous buffer.
   *
   * @param  stop
   *         If a stop bit needs to be sent at the end of the transmission.
   *
   * @param  prefix_buffer
   *         A pointer to a buffer containing bytes that needs to be send
   *         before data bytes (e.g. register address).
   *
   * @param  prefix_len
   *         Number of bytes to write from prefix_buffer.
   *
   * @return True if the event read was successful, otherwise false.
   */
  /**************************************************************************/
  bool write(const uint8_t *buffer, size_t len, bool stop = true,
             const uint8_t *prefix_buffer = NULL, size_t prefix_len = 0);
  /**************************************************************************/
  /*!
   * @brief  Writes to and then read from a I2C device register.
   *
   * @param  write_buffer
   *         A pointer to a buffer containing write bytes.
   *         
   * @param  write_len
   *         Number of bytes to write from the previous buffer.
   *
   * @param  read_buffer
   *         A pointer to a buffer to store bytes from the read operation.
   *
   * @param  read_len
   *         Number of bytes to read.
   *
   * @param  stop
   *         If a stop bit needs to be sent at the end of the transmission.
   *
   * @return True if the event read was successful, otherwise false.
   */
  /**************************************************************************/
  bool write_then_read(const uint8_t *write_buffer, size_t write_len,
                       uint8_t *read_buffer, size_t read_len,
                       bool stop = false);
  bool setSpeed(uint32_t desiredclk);

  /*!   @brief  How many bytes we can read in a transaction
   *    @return The size of the Wire receive/transmit buffer */
  size_t maxBufferSize() { return _maxBufferSize; }

private:
  uint8_t _addr;
  TwoWire *_wire;
  bool _begun;
  size_t _maxBufferSize;
};

#endif // Adafruit_I2CDevice_h
