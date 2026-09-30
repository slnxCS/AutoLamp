#include "Arduino.h"
#include "Input/Input.hpp"
#include "Input/PhotoResistor.hpp"

#define RELE_PIN 7
#define SUN_CONFIRM_MS 1000  // свет должен продержаться столько, чтобы снять ожидание, мс
//#define DEBUG

bool lamp_on = false;      // текущее состояние лампы
bool waiting_sun = false;  // ручное выключение до рассвета
uint32_t sun_since = 0;    // с какого момента светло (для снятия waiting_sun)

void set_rele(uint8_t signal) {
  static int8_t last = -1;
  if ((int8_t)signal == last) return;  // состояние не изменилось — пин не трогаем
  last = signal;
  digitalWrite(RELE_PIN, signal);
  // пока щелчок реле не улёгся, фоторезистору не верим:
  // свой же свет лампы не должен выглядеть как смена дня и ночи
  photo_resistor_hold(LIGHT_LOCK_MS);
}

void setup() {
  Serial.begin(9600);
  pinMode(RELE_PIN, OUTPUT);
  // pinMode кнопки не трогаем: EncButton сам ставит INPUT_PULLUP в конструкторе
  pinMode(RESISTOR_PIN, INPUT);
  set_rele(LOW);  // лампа выключена на старте
  Serial.println("Boot : OK");
}

void loop() {
  ControlButton.tick();
  photo_resistor_tick();

  if (ControlButton.click()) {  // клик всегда меняет состояние
    lamp_on = !lamp_on;
    waiting_sun = !lamp_on;     // выключили сами — ждём рассвета
  }

  // рассвет: свет должен продержаться SUN_CONFIRM_MS.
  // Считаем только «свежий» свет: не во время заморозки после щелчка реле
  // и пока датчик не подтвердил, что темноты нет. Фары, вспышка, свет самой
  // лампы — засвет, он ожидание не снимает.
  if (waiting_sun && !is_dark() && !photo_resistor_locked() &&
      !photo_resistor_pending_dark()) {
    if (sun_since == 0) sun_since = millis();
    if (millis() - sun_since >= SUN_CONFIRM_MS) {
      waiting_sun = false;
      sun_since = 0;
    }
  } else {
    sun_since = 0;
  }

  if (!waiting_sun && !lamp_on && is_dark() && just_became_dark())
    lamp_on = true;  // сумерки включают лампу

  set_rele(lamp_on ? HIGH : LOW);  // единственная запись на пин реле

#ifdef DEBUG
  Serial.println(analogRead(RESISTOR_PIN));
#endif
}