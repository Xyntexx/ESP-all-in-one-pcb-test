

#include "motor_test.h"
#include "Arduino.h"
#include "defines.h"


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
  pinMode(MOTOR_CURRENT_PIN, INPUT);
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
    LOGSerial.println("Motor braking");
    digitalWrite(MOTOR_ENABLE_PIN, HIGH);
    for (int i = 0; i < 5; i++) {
      print_motor_current();
      delay(100);
    }
    LOGSerial.println("Motor forward");
    digitalWrite(MOTOR_PWM_PIN, HIGH);
    digitalWrite(MOTOR_DIR_PIN, HIGH);
    for (int i = 0; i < 5; i++) {
      print_motor_current();
      delay(100);
    }
    LOGSerial.println("Motor braking");
    digitalWrite(MOTOR_PWM_PIN, LOW);
    digitalWrite(MOTOR_DIR_PIN, LOW);
    for (int i = 0; i < 5; i++) {
      print_motor_current();
      delay(100);
    }
    LOGSerial.println("Motor reverse");
    digitalWrite(MOTOR_PWM_PIN, HIGH);
    digitalWrite(MOTOR_DIR_PIN, LOW);
    for (int i = 0; i < 5; i++) {
      print_motor_current();
      delay(100);
    }
    LOGSerial.println("Motor braking");
    digitalWrite(MOTOR_PWM_PIN, LOW);
    digitalWrite(MOTOR_DIR_PIN, LOW);
    for (int i = 0; i < 5; i++) {
      print_motor_current();
      delay(100);
    }
  }
}

void init_motor_test() {
  xTaskCreate(motor_test_task, "motor_test_task", 4096, NULL, 5, NULL);
}