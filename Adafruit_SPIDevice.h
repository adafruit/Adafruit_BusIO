#ifndef Adafruit_SPIDevice_h
#define Adafruit_SPIDevice_h

#include <Arduino.h>

#if !defined(SPI_INTERFACES_COUNT) ||                                          \
    (defined(SPI_INTERFACES_COUNT) && (SPI_INTERFACES_COUNT > 0))

#include <SPI.h>

// some modern SPI definitions don't have BitOrder enum
#if (defined(__AVR__) && !defined(ARDUINO_ARCH_MEGAAVR)) ||                    \
    defined(ESP8266) || defined(TEENSYDUINO) || defined(SPARK) ||              \
    defined(ARDUINO_ARCH_SPRESENSE) || defined(MEGATINYCORE) ||                \
    defined(DXCORE) || defined(ARDUINO_AVR_ATmega4809) ||                      \
    defined(ARDUINO_AVR_ATmega4808) || defined(ARDUINO_AVR_ATmega3209) ||      \
    defined(ARDUINO_AVR_ATmega3208) || defined(ARDUINO_AVR_ATmega1609) ||      \
    defined(ARDUINO_AVR_ATmega1608) || defined(ARDUINO_AVR_ATmega809) ||       \
    defined(ARDUINO_AVR_ATmega808) || defined(ARDUINO_ARCH_ARC32)

typedef enum _BitOrder {
  SPI_BITORDER_MSBFIRST = MSBFIRST,
  SPI_BITORDER_LSBFIRST = LSBFIRST,
} BusIOBitOrder;

#elif defined(ESP32) || defined(__ASR6501__) || defined(__ASR6502__)

// some modern SPI definitions don't have BitOrder enum and have different SPI
// mode defines
typedef enum _BitOrder {
  SPI_BITORDER_MSBFIRST = SPI_MSBFIRST,
  SPI_BITORDER_LSBFIRST = SPI_LSBFIRST,
} BusIOBitOrder;

#else
// Some platforms have a BitOrder enum but its named MSBFIRST/LSBFIRST
#define SPI_BITORDER_MSBFIRST MSBFIRST
#define SPI_BITORDER_LSBFIRST LSBFIRST
typedef BitOrder BusIOBitOrder;
#endif

#if defined(__IMXRT1062__) // Teensy 4.x
// *Warning* I disabled the usage of FAST_PINIO as the set/clear operations
// used in the cpp file are not atomic and can effect multiple IO pins
// and if an interrupt happens in between the time the code reads the register
//  and writes out the updated value, that changes one or more other IO pins
// on that same IO port, those change will be clobbered when the updated
// values are written back.  A fast version can be implemented that uses the
// ports set and clear registers which are atomic.
// typedef volatile uint32_t BusIO_PortReg;
// typedef uint32_t BusIO_PortMask;
//#define BUSIO_USE_FAST_PINIO

#elif defined(__AVR__) || defined(TEENSYDUINO)
typedef volatile uint8_t BusIO_PortReg;
typedef uint8_t BusIO_PortMask;
#define BUSIO_USE_FAST_PINIO

#elif defined(ESP8266) || defined(ESP32) || defined(__SAM3X8E__) ||            \
    defined(ARDUINO_ARCH_SAMD)
typedef volatile uint32_t BusIO_PortReg;
typedef uint32_t BusIO_PortMask;
#define BUSIO_USE_FAST_PINIO

#elif (defined(__arm__) || defined(ARDUINO_FEATHER52)) &&                      \
    !defined(ARDUINO_ARCH_MBED) && !defined(ARDUINO_ARCH_RP2040)
typedef volatile uint32_t BusIO_PortReg;
typedef uint32_t BusIO_PortMask;
#if !defined(__ASR6501__) && !defined(__ASR6502__)
#define BUSIO_USE_FAST_PINIO
#endif

#else
#undef BUSIO_USE_FAST_PINIO
#endif

/**! The class which defines how we will talk to this device over SPI **/
class Adafruit_SPIDevice {
public:
  Adafruit_SPIDevice(int8_t cspin, uint32_t freq = 1000000,
                     BusIOBitOrder dataOrder = SPI_BITORDER_MSBFIRST,
                     uint8_t dataMode = SPI_MODE0, SPIClass *theSPI = &SPI);

  Adafruit_SPIDevice(int8_t cspin, int8_t sck, int8_t miso, int8_t mosi,
                     uint32_t freq = 1000000,
                     BusIOBitOrder dataOrder = SPI_BITORDER_MSBFIRST,
                     uint8_t dataMode = SPI_MODE0);
  ~Adafruit_SPIDevice();

  bool begin(void);
  bool read(uint8_t *buffer, size_t len, uint8_t sendvalue = 0xFF);
  bool write(const uint8_t *buffer, size_t len,
             const uint8_t *prefix_buffer = nullptr, size_t prefix_len = 0);
  bool write_then_read(const uint8_t *write_buffer, size_t write_len,
                       uint8_t *read_buffer, size_t read_len,
                       uint8_t sendvalue = 0xFF);
  bool write_and_read(uint8_t *buffer, size_t len);

  uint8_t transfer(uint8_t send);
  void transfer(uint8_t *buffer, size_t len);
  void beginTransaction(void);
  void endTransaction(void);
  void beginTransactionWithAssertingCS();
  void endTransactionWithDeassertingCS();

private:
  SPIClass *_spi;
  SPISettings *_spiSetting;
  uint32_t _freq;
  BusIOBitOrder _dataOrder;
  uint8_t _dataMode;

  void setChipSelect(int value);

  int8_t _cs, _sck, _mosi, _miso;
#ifdef BUSIO_USE_FAST_PINIO
  BusIO_PortReg *mosiPort, *clkPort, *misoPort, *csPort;
  BusIO_PortMask mosiPinMask, misoPinMask, clkPinMask, csPinMask;
#endif

  //! constant for the buffer size for the chunked transfer
  static constexpr size_t maxBufferSizeForChunkedTransfer =
#ifdef __AVR__
      32;
#else
      64;
#endif
  bool _begun;

protected:
  /*!
   *    @brief  Template to encypsulate a C-array, provides STL-style accessors
   * analoguous to std::array<>
   *    @param  Type the type of the array
   *    @param  Size the size of the array
   */
  template <typename Type, size_t Size> class Array {
  public:
    /*! @brief returns a pointer the start of the buffer
     * @returns a pointer the start of the buffer
     */
    Type *begin() { return buffer; }

    /*! @brief returns a pointer the one increment beyond the end of the buffer
     * @returns a pointer the one increment beyond the end of the buffer
     */
    Type *end() { return endPointer; }

    /*! @brief returns the size of the buffer
     * @returns the size of the buffer
     */
    constexpr size_t size() { return Size; }

    /*! @brief returns the buffer
     * @returns the buffer
     */
    Type *data() { return buffer; }

    /*! @brief returns a reference to the element at @param i
     * @param i the index
     * @returns a reference to the element at @param i
     */
    Type &operator[](size_t i) { return buffer[i]; }

    /*! @brief returns a reference to the element at @param i
     * @param i the index
     * @returns a reference to the element at @param i
     */
    const Type &operator[](size_t i) const { return buffer[i]; }

  private:
    //! the buffer
    Type buffer[Size];
    //! address buffer one increment after the end
    Type *endPointer = buffer + Size;
  };
};

#endif // has SPI defined
#endif // Adafruit_SPIDevice_h
