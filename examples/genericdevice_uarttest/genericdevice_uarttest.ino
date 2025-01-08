/* 
   Abstracted transport for reading and writing data from a UART-based 
   device such as a TMC2209

   Written with help by Claude! https://claude.ai/chat/335f50b1-3dd8-435e-9139-57ec7ca26a3c
  (at this time chats are not shareable :(
*/

#include "Adafruit_GenericDevice.h"

Stream *uart_stream; // Will hold the pointer to our Stream object

Adafruit_GenericDevice *create_uart_device(Stream *serial_port) {
  uart_stream = serial_port; // Store the Stream pointer

  auto uart_write = [](const uint8_t *buffer, size_t len) -> bool {
    uart_stream->write(buffer, len);
    return true;
  };

  auto uart_read = [](uint8_t *buffer, size_t len) -> bool {
    uint16_t timeout = 100;
    while (uart_stream->available() < len && timeout--) {
      delay(1);
    }
    if (timeout == 0) {
      return false;
    }
    for (size_t i = 0; i < len; i++) {
      buffer[i] = uart_stream->read();
    }
    return true;
  };

  return new Adafruit_GenericDevice(uart_read, uart_write);
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;
  delay(100);

  Serial.println("Generic Device test!");

  Serial1.begin(115200);

  Adafruit_GenericDevice *device = create_uart_device(&Serial1);
  device->begin();

  uint8_t write_buf[4] = {0x5, 0x0, 0x0, 0x48};
  uint8_t read_buf[8];

  Serial.println("Writing data...");
  if (!device->write(write_buf, 4)) {
    Serial.println("Write failed!");
    return;
  }

  Serial.println("Reading response...");
  if (!device->read(read_buf, 8)) {
    Serial.println("Read failed!");
    return;
  }

  Serial.print("Got response: ");
  for (int i = 0; i < 8; i++) {
    Serial.print("0x");
    Serial.print(read_buf[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void loop() { delay(1000); }
