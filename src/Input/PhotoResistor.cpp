#include "PhotoResistor.hpp"
#include "Arduino.h"

static bool dark = false;         // подтверждённое состояние датчика
static bool became_dark = false;  // флаг: только что стемнело (один тик)
static bool pending = false;      // ждём подтверждения нового состояния
static bool pending_dark = false; // какое состояние ждём
static uint32_t pending_since = 0;
static uint32_t lock_until = 0;

void photo_resistor_hold(uint32_t ms) {
    lock_until = millis() + ms;
    pending = false;  // кандидат, накопленный до щелчка, отбрасываем
}

void photo_resistor_tick() {
    became_dark = false;

    // защита от засвета в момент переключения реле:
    // пока не прошло LIGHT_LOCK_MS, показания не читаем
    if ((int32_t)(millis() - lock_until) < 0) {
        pending = false;
        return;
    }

    int level = analogRead(RESISTOR_PIN);

    // исходный порог сохранён: темно при level < 500 (HYST = 0 — без сдвига).
    // Пороги не двигаем, иначе датчик «не видит» темноту/свет при значениях
    // около 500. Если состояние дёргается на границе — поднимите HYST.
    bool next = dark ? (level < PHOTORESISTOR_NIGHT_THRESHOLD + PHOTORESISTOR_HYST)
                     : (level < PHOTORESISTOR_NIGHT_THRESHOLD - PHOTORESISTOR_HYST);

    if (next == dark) {  // порог не пройден — ждём дальше
        pending = false;
        return;
    }

    // новое состояние должно продержаться LIGHT_CONFIRM_MS,
    // иначе это засвет/помеха, а не рассвет или сумерки
    if (!pending || pending_dark != next) {
        pending = true;
        pending_dark = next;
        pending_since = millis();
        return;
    }
    if (millis() - pending_since < LIGHT_CONFIRM_MS) return;

    dark = next;
    became_dark = dark;
    pending = false;
}

bool is_dark() {
    return dark;
}

bool just_became_dark() {
    return became_dark;
}

bool photo_resistor_locked() {
    return (int32_t)(millis() - lock_until) < 0;
}

bool photo_resistor_pending_dark() {
    return pending && pending_dark;
}
