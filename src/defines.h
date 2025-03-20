#ifndef DEFINES_H
#define DEFINES_H
#include "Arduino.h"

#define LOGSerial USBSerial

#define GPS_RX_PIN 1
#define GPS_TX_PIN 2

#define GPS_UART_BAUD_RATE 38400
#define GPSSerial Serial2

#define STEER_BTN_PIN 12
#define WORK_BTN_PIN 13

#define SDA_PIN 5
#define SCL_PIN 4

#define ADC_DREADY_PIN 14

//create I2C lock
#define I2C_MUTEX_LOCK() (xSemaphoreTake(i2cMutex, portMAX_DELAY) != pdTRUE)
#define I2C_MUTEX_UNLOCK() xSemaphoreGive(i2cMutex)
extern SemaphoreHandle_t i2cMutex;

#define MOTOR_ENABLE_PIN 8
#define MOTOR_PWM_PIN 9
#define MOTOR_DIR_PIN 10
#define MOTOR_CURRENT_PIN 11

#define MOTOR_PWM_CHANNEL 0
#define MOTOR_PWM_FREQ 5000
#define MOTOR_PWM_RESOLUTION 8

#define MAC_ADDR {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}
#define IP_ADDR {192, 168, 178, 22}
#define GW_ADDR {192, 168, 178, 1}
#define SN_ADDR {255, 255, 255, 0}
#define DNS_ADDR {8, 8, 8, 8}

#define W6100_CS_GPIO 18
#define W6100_RESET_GPIO 7
#define W6100_SCK_GPIO 15
#define W6100_MISO_GPIO 16
#define W6100_MOSI_GPIO 17
#define W6100_INT_GPIO 6

#define LED1_GPIO 41
#define LED2_GPIO 42



#endif // DEFINES_H
