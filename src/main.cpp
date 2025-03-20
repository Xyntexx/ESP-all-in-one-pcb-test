#include "defines.h"
#include "imu_test.h"
#include "btn_test.h"
#include "gps_test.h"
#include "adc_test.h"
#include "motor_test.h"
#include "ethernet_test.h"

#include <Arduino.h>
void setup() {
  // wait 1 seconds for USB to connect
  delay(1000);
  LOGSerial.begin(115200);
  LOGSerial.println("Hello, World!");
  //init_imu_test();
  //init_btn_test();
  //init_gps_test();
  //init_adc_test();
  //init_motor_test();
  init_ethernet_test();
}

void loop() {
  // write your code here
  delay(1000);
}