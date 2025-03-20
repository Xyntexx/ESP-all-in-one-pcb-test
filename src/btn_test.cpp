
#include "btn_test.h"
#include "Arduino.h"
#include "defines.h"

bool work_btn_state = false;
bool steer_btn_state = false;

[[noreturn]] void button_test_task(void *pvParameters) {
  LOGSerial.println("Button test task started");
  pinMode(WORK_BTN_PIN, INPUT_PULLUP);
  pinMode(STEER_BTN_PIN, INPUT_PULLUP);
  while (true) {
    bool work_btn_state_new = digitalRead(WORK_BTN_PIN);
    bool steer_btn_state_new = digitalRead(STEER_BTN_PIN);
    if (work_btn_state_new != work_btn_state ||
        steer_btn_state_new != steer_btn_state) {
      work_btn_state = work_btn_state_new;
      steer_btn_state = steer_btn_state_new;
      LOGSerial.printf("Work button state: %d, Steer button state: %d\n",
                       work_btn_state, steer_btn_state);
    }
    delay(100);
  }
}

void init_btn_test() {
  xTaskCreatePinnedToCore(button_test_task, "button_test_task", 2048, NULL, 1,
                          NULL, 1);
}