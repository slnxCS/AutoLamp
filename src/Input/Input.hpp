#pragma once
#include "EncButton.h"

#define BUTTON_PIN 5

extern Button ControlButton;

void OnControlButtonAction(uint16_t action);