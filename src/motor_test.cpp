#include "motor_test.h"
#include "Arduino.h"
#include "defines.h"

// PWM speed control constants
const int TEST_SPEEDS[] = {
  static_cast<int>(MAX_PWM * 0.25),  // 25% of max
  static_cast<int>(MAX_PWM * 0.50),  // 50% of max
  static_cast<int>(MAX_PWM * 0.75),  // 75% of max
  static_cast<int>(MAX_PWM)          // 100% of max
};
const int NUM_SPEEDS = sizeof(TEST_SPEEDS) / sizeof(TEST_SPEEDS[0]);

void print_motor_current() {
  int current = analogRead(MOTOR_CURRENT_PIN);
  LOGSerial.print("Motor current: ");
  LOGSerial.println(current);
}

[[noreturn]] void motor_test_task(void *params) {
  LOGSerial.println("Motor test task started");
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  pinMode(MOTOR_CURRENT_PIN, ANALOG);
  digitalWrite(MOTOR_ENABLE_PIN, LOW);
  digitalWrite(MOTOR_PWM_PIN, LOW);
  digitalWrite(MOTOR_DIR_PIN, LOW);

  while (true) {
    LOGSerial.println("Motor coasting");
    digitalWrite(MOTOR_ENABLE_PIN, LOW);
    for (int i = 0; i < 25; i++) {
      print_motor_current();
      delay(200);
    }

    // Test different speeds in forward direction
    for (int speed_idx = 0; speed_idx < NUM_SPEEDS; speed_idx++) {
      int speed = TEST_SPEEDS[speed_idx];
      LOGSerial.print("Motor forward at speed: ");
      LOGSerial.println(speed);
      
      digitalWrite(MOTOR_ENABLE_PIN, HIGH);
      digitalWrite(MOTOR_DIR_PIN, HIGH);
      analogWrite(MOTOR_PWM_PIN, speed);
      
      for (int i = 0; i < 5; i++) {
        print_motor_current();
        delay(100);
      }
      
      // Brake between speed changes
      LOGSerial.println("Motor braking");
      digitalWrite(MOTOR_PWM_PIN, LOW);
      for (int i = 0; i < 5; i++) {
        print_motor_current();
        delay(100);
      }
    }

    // Test different speeds in reverse direction
    for (int speed_idx = 0; speed_idx < NUM_SPEEDS; speed_idx++) {
      int speed = TEST_SPEEDS[speed_idx];
      LOGSerial.print("Motor reverse at speed: ");
      LOGSerial.println(speed);
      
      digitalWrite(MOTOR_ENABLE_PIN, HIGH);
      digitalWrite(MOTOR_DIR_PIN, LOW);
      analogWrite(MOTOR_PWM_PIN, speed);
      
      for (int i = 0; i < 5; i++) {
        print_motor_current();
        delay(100);
      }
      
      // Brake between speed changes
      LOGSerial.println("Motor braking");
      digitalWrite(MOTOR_PWM_PIN, LOW);
      for (int i = 0; i < 5; i++) {
        print_motor_current();
        delay(100);
      }
    }
  }
}

void init_motor_test() {
  xTaskCreate(motor_test_task, "motor_test_task", 4096, NULL, 5, NULL);
}