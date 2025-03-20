#ifndef IMU_TEST_H
#define IMU_TEST_H

#include <Arduino.h>
#include <Adafruit_BNO08x.h>

#define BNO08X_RESET (-1)

static Adafruit_BNO08x bno08x(BNO08X_RESET);
static sh2_SensorValue_t sensorValue;


// Function to initialize the IMU task
void init_imu_test();

#endif // IMU_TEST_H