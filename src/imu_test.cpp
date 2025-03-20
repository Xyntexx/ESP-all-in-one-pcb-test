// imu_test.cpp
#include "imu_test.h"
#include "defines.h"
#include <Adafruit_BNO08x.h>

#define CONST_180_DIVIDED_BY_PI 57.2957795130823

extern Adafruit_BNO08x bno08x;
extern sh2_SensorValue_t sensorValue;
float getYaw(sh2_RotationVectorWAcc_t *rv);
float getRoll(sh2_RotationVectorWAcc_t *rv);
float getPitch(sh2_RotationVectorWAcc_t *rv);
void setReports();

[[noreturn]] void imu_task(void *parameter) {
  LOGSerial.println("IMU task started");
  I2C_MUTEX_LOCK();
  Wire.end();
  Wire.setPins(SDA_PIN, SCL_PIN);
  Wire.begin();
  LOGSerial.println("Adafruit BNO08x test!");
  if (!bno08x.begin_I2C(BNO08x_I2CADDR_DEFAULT, &Wire)) {
    // if (!bno08x.begin_UART(&Serial1)) {  // Requires a device with > 300 byte
    // UART buffer! if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
    LOGSerial.println("Failed to find BNO08x chip");
    I2C_MUTEX_UNLOCK();

    while (1) {
      delay(10);
    }
  }
  LOGSerial.println("BNO08x Found!");

  for (int n = 0; n < bno08x.prodIds.numEntries; n++) {
    LOGSerial.print("Part ");
    LOGSerial.print(bno08x.prodIds.entry[n].swPartNumber);
    LOGSerial.print(": Version :");
    LOGSerial.print(bno08x.prodIds.entry[n].swVersionMajor);
    LOGSerial.print(".");
    LOGSerial.print(bno08x.prodIds.entry[n].swVersionMinor);
    LOGSerial.print(".");
    LOGSerial.print(bno08x.prodIds.entry[n].swVersionPatch);
    LOGSerial.print(" Build ");
    LOGSerial.println(bno08x.prodIds.entry[n].swBuildNumber);
  }
  int i = 0;
  setReports();
  I2C_MUTEX_UNLOCK();

  while (true) {
    I2C_MUTEX_LOCK();
    bno08x.getSensorEvent(&sensorValue);
    if (bno08x.wasReset()) {
      LOGSerial.print("IMU sensor was reset ");
      setReports();
    }
    if (i++ == 10) {
      i = 0;

      if (sensorValue.sensorId == SH2_ROTATION_VECTOR) {
        float yaw =
            getYaw(&sensorValue.un.rotationVector) * CONST_180_DIVIDED_BY_PI;
        float pitch =
            getPitch(&sensorValue.un.rotationVector) * CONST_180_DIVIDED_BY_PI;
        float roll =
            getRoll(&sensorValue.un.rotationVector) * CONST_180_DIVIDED_BY_PI;

        LOGSerial.printf("Yaw: %f, Pitch: %f, Roll: %f\n", yaw, pitch, roll);
      }
    }
    I2C_MUTEX_UNLOCK();
    delay(100);
  }
}

  void init_imu_test() {
    xTaskCreatePinnedToCore(imu_task,   /* Function to implement the task */
                            "imu_task", /* Name of the task */
                            10000,      /* Stack size in words */
                            nullptr,    /* Task input parameter */
                            1,          /* Priority of the task */
                            nullptr,    /* Task handle. */
                            1);         /* Core where the task should run */
  }
  float getYaw(sh2_RotationVectorWAcc_t * rv) {
    float dqw = rv->real;
    float dqx = rv->i;
    float dqy = rv->j;
    float dqz = rv->k;

    float norm = sqrt(dqw * dqw + dqx * dqx + dqy * dqy + dqz * dqz);
    dqw = dqw / norm;
    dqx = dqx / norm;
    dqy = dqy / norm;
    dqz = dqz / norm;

    float ysqr = dqy * dqy;

    // yaw (z-axis rotation)
    float t3 = +2.0 * (dqw * dqz + dqx * dqy);
    float t4 = +1.0 - 2.0 * (ysqr + dqz * dqz);
    float yaw = atan2(t3, t4);

    return (yaw);
  }

  float getRoll(sh2_RotationVectorWAcc_t * rv) {
    float dqw = rv->real;
    float dqx = rv->i;
    float dqy = rv->j;
    float dqz = rv->k;

    float norm = sqrt(dqw * dqw + dqx * dqx + dqy * dqy + dqz * dqz);
    dqw = dqw / norm;
    dqx = dqx / norm;
    dqy = dqy / norm;
    dqz = dqz / norm;

    float ysqr = dqy * dqy;

    // roll (x-axis rotation)
    float t0 = +2.0 * (dqw * dqx + dqy * dqz);
    float t1 = +1.0 - 2.0 * (dqx * dqx + ysqr);
    float roll = atan2(t0, t1);

    return (roll);
  }

  float getPitch(sh2_RotationVectorWAcc_t * rv) {
    float dqw = rv->real;
    float dqx = rv->i;
    float dqy = rv->j;
    float dqz = rv->k;

    float norm = sqrt(dqw * dqw + dqx * dqx + dqy * dqy + dqz * dqz);
    dqw = dqw / norm;
    dqx = dqx / norm;
    dqy = dqy / norm;
    dqz = dqz / norm;

    float ysqr = dqy * dqy;

    // pitch (y-axis rotation)
    float t2 = +2.0 * (dqw * dqy - dqz * dqx);
    t2 = t2 > 1.0 ? 1.0 : t2;
    t2 = t2 < -1.0 ? -1.0 : t2;
    float pitch = asin(t2);

    return (pitch);
  }

  void setReports() {
    if (!bno08x.enableReport(SH2_ROTATION_VECTOR)) {
      LOGSerial.println("Could not enable rotation vector");
    } // This is the one we want
    /*
    //if (!bno08x.enableReport(SH2_ACCELEROMETER)) { LOGSerial.println("Could
    not enable accelerometer"); }
    //if (!bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED)) {
    LOGSerial.println("Could not enable gyroscope"); }
    //if (!bno08x.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED)) {
    LOGSerial.println("Could not enable magnetic field calibrated");    }
    //if (!bno08x.enableReport(SH2_LINEAR_ACCELERATION)) {
    LOGSerial.println("Could not enable linear acceleration");    }
    //if (!bno08x.enableReport(SH2_GRAVITY)) {        LOGSerial.println("Could
    not enable gravity vector");    }
    //if (!bno08x.enableReport(SH2_GEOMAGNETIC_ROTATION_VECTOR)) {
    LOGSerial.println("Could not enable geomagnetic rotation vector");    } //
    Uses only magnetometer and accelerometer
    //if (!bno08x.enableReport(SH2_GAME_ROTATION_VECTOR)) {
    LOGSerial.println("Could not enable game rotation vector");    }
    //if (!bno08x.enableReport(SH2_STEP_COUNTER)) { LOGSerial.println("Could not
    enable step counter");    }
    //if (!bno08x.enableReport(SH2_STABILITY_CLASSIFIER)) {
    LOGSerial.println("Could not enable stability classifier");}
    // if (!bno08x.enableReport(SH2_RAW_ACCELEROMETER)) {
    LOGSerial.println("Could not enable raw accelerometer");    }
    //if (!bno08x.enableReport(SH2_RAW_GYROSCOPE)) {   LOGSerial.println("Could
    not enable raw gyroscope");    }
    //if (!bno08x.enableReport(SH2_RAW_MAGNETOMETER)) { LOGSerial.println("Could
    not enable raw magnetometer"); }
    //if (!bno08x.enableReport(SH2_SHAKE_DETECTOR)) { LOGSerial.println("Could
    not enable shake detector");    }
    //if (!bno08x.enableReport(SH2_PERSONAL_ACTIVITY_CLASSIFIER)) {
    LOGSerial.println("Could not enable personal activity classifier");    }
  */
  }