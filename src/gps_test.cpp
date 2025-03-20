

#include "gps_test.h"
#include "Arduino.h"
#include "defines.h"

const size_t gpsBufferSize = 256;

[[noreturn]] void gps_test_task(void *parameter) {
  LOGSerial.println("GPS test task started");
  // Open serial port for GPS communication and reference station communication
  // BaseSerial.begin(115200, SERIAL_8N1, 8, 7);
  GPSSerial.begin(GPS_UART_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN, false, 10);
  GPSSerial.setRxTimeout(10);

  while (true) {
    while (GPSSerial.available()) {
      uint8_t gpsBuffer[gpsBufferSize];
      uint8_t len =
          GPSSerial.readBytesUntil('\n', gpsBuffer, sizeof(gpsBuffer));
      if (len > 0) {
        LOGSerial.write(gpsBuffer, len);
      }
      delay(1);
    }
    delay(1);
  }
}

void init_gps_test() {
  xTaskCreatePinnedToCore(gps_test_task,   /* Function to implement the task */
                          "gps_test_task", /* Name of the task */
                          10000,           /* Stack size in words */
                          NULL,            /* Task input parameter */
                          1,               /* Priority of the task */
                          NULL,            /* Task handle. */
                          0);              /* Core where the task should run */
}