#ifndef RESET_BUTTON_H
#define RESET_BUTTON_H

#include <Arduino.h>


class ResetButton
{
public:

    ResetButton(uint8_t pin);

    void begin();

    // Muss regelmäßig aus loop() aufgerufen werden.
    void update();

    // Gibt genau einmal true zurück,
    // wenn ein kurzer Druck erkannt wurde.
    bool shortPressed();

    // Gibt genau einmal true zurück,
    // wenn ein langer Druck erkannt wurde.
    bool longPressed();


private:

    uint8_t pin;

    bool currentState = HIGH;
    bool lastReading = HIGH;

    bool shortPressEvent = false;
    bool longPressEvent = false;

    bool longPressTriggered = false;

    unsigned long lastDebounceTime = 0;
    unsigned long pressStartTime = 0;

    static const unsigned long debounceTime = 30;
    static const unsigned long longPressTime = 1000;
};

#endif