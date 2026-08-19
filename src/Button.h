#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>


class Button
{
public:

    Button(uint8_t pin);

    void begin();

    // Gibt genau einmal true zurück,
    // wenn der Taster gedrückt wurde.
    bool pressed();


private:

    uint8_t pin;

    bool currentState = HIGH;
    bool lastReading = HIGH;

    unsigned long lastDebounceTime = 0;

    static const unsigned long debounceTime = 30;
};

#endif