#include "Button.h"


// ==================================================
// Konstruktor
// ==================================================

Button::Button(uint8_t pin)
{
    this->pin = pin;
}


// ==================================================
// Initialisierung
// ==================================================

void Button::begin()
{
    pinMode(pin, INPUT_PULLUP);

    currentState = digitalRead(pin);
    lastReading = currentState;

    lastDebounceTime = millis();
}


// ==================================================
// Taster gedrückt?
// ==================================================

bool Button::pressed()
{
    bool reading = digitalRead(pin);


    // ----------------------------------------------
    // Hat sich der physikalische Zustand verändert?
    // ----------------------------------------------

    if (reading != lastReading)
    {
        lastDebounceTime = millis();
        lastReading = reading;
    }


    // ----------------------------------------------
    // Zustand muss für 30 ms stabil sein
    // ----------------------------------------------

    if ((millis() - lastDebounceTime) > debounceTime)
    {
        if (reading != currentState)
        {
            currentState = reading;


            // LOW bedeutet bei INPUT_PULLUP:
            // Taster wurde gedrückt.

            if (currentState == LOW)
            {
                return true;
            }
        }
    }


    return false;
}