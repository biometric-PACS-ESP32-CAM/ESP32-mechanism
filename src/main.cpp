#include <Arduino.h>
#include <ESP32Servo.h>
#include <WebServer.h>
#include <WiFi.h>
#include "wifi_config.h"

#define BTN_PERIOD 2000

WebServer server(80);

Servo servo;

// Пины обвеса
const int btn_in_pin = 12;
const int btn_out_pin = 14;
const int rele_pin = 27;
const int servo_pin = 13;

// Таймер для двери
uint32_t rele_timer = 0;
bool rele_state = 0;

// Таймер для сенсоров
uint32_t btn_timer = 0;

// user_id=333666&name=Ivan&surname=Mischenko&status_enter=out
void handleData() {
  String user_id = server.arg("user_id");
  String name = server.arg("name"); 
  String surname = server.arg("surname");
  String status_enter = server.arg("status_enter");
  if (status_enter == "out") {
    status_enter = "in";
    server.send(200, "text/plain", "in");
  } else {
    status_enter = "out";
    server.send(200, "text/plain", "out");
  }
  Serial.println("Door opened for " + name + " " + surname + ", status=" + status_enter);
  // Открываем дверь
  rele_state = 1;
  rele_timer = millis();
  digitalWrite(rele_pin, HIGH);
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Настраиваем пины обвеса
  pinMode(rele_pin, OUTPUT);
  digitalWrite(rele_pin, LOW);
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

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.print("http://");
  Serial.println(WiFi.localIP());

  server.on("/data", HTTP_POST, handleData);
  server.begin();
}

void loop() {
  server.handleClient();

  if (millis() - rele_timer >= 5000 && rele_state) {
    rele_state = 0;
    digitalWrite(rele_pin, LOW);
  }
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
