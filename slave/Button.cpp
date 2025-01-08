#include "Button.h"

Button::Button(int pin, bool activeLow) : pin(pin), activeLow(activeLow), lastState(!activeLow) {}

void Button::begin() {
    pinMode(pin, INPUT);
}

bool Button::isPressed() {
    bool currentState = digitalRead(pin);
    return activeLow ? (currentState == LOW) : (currentState == HIGH);
}

bool Button::stateChanged() {
    bool currentState = isPressed();
    if (currentState != lastState) {
        lastState = currentState;
        return true;
    }
    return false;
}

bool Button::getState() {
    return isPressed();
}