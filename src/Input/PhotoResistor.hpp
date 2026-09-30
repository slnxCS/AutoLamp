#pragma once
#include <Arduino.h>

#define RESISTOR_PIN 3

// порог тот же, что и в исходном коде: темно, когда меньше 500
#define PHOTORESISTOR_NIGHT_THRESHOLD 500
// гистерезис, ед. АЦП: 0 = порог один (как раньше),
// >0 = раздвижка (темно ниже 500-HYST, светло выше 500+HYST) — включать, если
// датчик шумит на границе и состояние дёргается
#define PHOTORESISTOR_HYST 0

#define LIGHT_CONFIRM_MS 1000  // новый уровень должен держаться столько, мс
#define LIGHT_LOCK_MS 1500     // не слушаем датчик после щелчка реле, мс

// true — сейчас темно (подтверждённое состояние)
bool is_dark();

// true ровно в тот тик, когда состояние подтвердило переход в темноту
bool just_became_dark();

void photo_resistor_tick();

// заморозка датчика: ms мс после щелчка реле изменения не принимаются
void photo_resistor_hold(uint32_t ms);

// true, пока показания заморожены щелчком реле (данные устарели)
bool photo_resistor_locked();

// true, пока датчик ждёт подтверждения перехода в темноту
bool photo_resistor_pending_dark();
