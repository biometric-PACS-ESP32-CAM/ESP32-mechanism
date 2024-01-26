#include <Arduino.h>
#include <ESP32Servo.h>

#define BTN_PERIOD 2000

Servo servo;

// Пины обвеса
const int btn_in_pin = 12;
const int btn_out_pin = 13;
const int servo_pin = 14;

// Таймер для сенсоров
uint32_t btn_timer = 0;

void setup() {
  pinMode(btn_in_pin, INPUT);
  pinMode(btn_out_pin, INPUT);
  pinMode(servo_pin, OUTPUT);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servo.setPeriodHertz(50);             //  standard 50 hz servo
  servo.attach(servo_pin, 500, 2400);   //  pin, min, max impulse
  servo.write(0);                       //  set angle
}

void loop() {
  if (digitalRead(btn_in_pin) && millis() - btn_timer >= BTN_PERIOD) {
    btn_timer = millis();
    for (int pos = 0; pos <= 180; pos++) {
      servo.write(pos);
      delay(5);
    }
  }
  if (digitalRead(btn_out_pin) && millis() - btn_timer >= BTN_PERIOD) {
    btn_timer = millis();
    for (int pos = 180; pos >= 0; pos--) {
      servo.write(pos);
      delay(5);
    }
  }
}
