

#include "ADS1115_lite.h"
#include "defines.h"
ADS1115_lite
    adc(ADS1115_DEFAULT_ADDRESS); // Use this for the 16-bit version ADS1115
SemaphoreHandle_t i2cMutex = xSemaphoreCreateMutex();

[[noreturn]] void adc_test_task(void *parameter) {
  LOGSerial.println("ADC test task started");
  I2C_MUTEX_LOCK(); // Lock I2C bus
  Wire.end();
  Wire.setPins(SDA_PIN, SCL_PIN);
  Wire.begin();

  if (adc.isConnected()) {
    LOGSerial.println("ADC Connecton OK");

  } else {
    LOGSerial.println("ADC Connecton FAILED!");
  }
  adc.setSampleRate(ADS1115_DR_128SPS); // Set sample rate to 128SPS
  adc.setGain(ADS1115_GAIN_6_144V);    // Set gain to 6.144V
  I2C_MUTEX_UNLOCK(); // Unlock I2C bus

  while (true) {
    I2C_MUTEX_LOCK(); // Lock I2C bus
    adc.setMux(ADS1115_MUX_SINGLE_0); // Single-Ended AIN0
    LOGSerial.print("ADC: ");
    LOGSerial.println(adc.readConversion());
    adc.startConversion(); // ADS1115 Single Mode
    I2C_MUTEX_UNLOCK();      // Unlock I2C bus

    delay(1000);
  }
}

void init_adc_test() {
  xTaskCreatePinnedToCore(adc_test_task,   /* Function to implement the task */
                          "adc_test_task", /* Name of the task */
                          10000,           /* Stack size in words */
                          nullptr,         /* Task input parameter */
                          1,               /* Priority of the task */
                          nullptr,         /* Task handle. */
                          1);              /* Core where the task should run */
}