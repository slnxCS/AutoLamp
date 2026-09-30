#include "Input.hpp"
#include "main.hpp"

Button ControlButton { BUTTON_PIN };

void OnControlButtonAction(uint16_t action) {
    switch (action)
    {
        case EB_PRESS:
        Serial.println("button has pressed");
            waiting_sun = !waiting_sun;
            set_rele(waiting_sun ? LOW : HIGH);
        break;
    }
}