#include "Button.h"


// ==================================================
// Konstruktor
// ==================================================

Button::Button(
    Adafruit_MCP23X17* mcp,
    uint8_t pin
)
{
    this->mcp = mcp;
    this->pin = pin;
}


// ==================================================
// Initialisierung
// ==================================================

void Button::begin()
{
    mcp->pinMode(pin, INPUT_PULLUP);

    currentState = mcp->digitalRead(pin);
    lastReading = currentState;

    lastDebounceTime = millis();
}


// ==================================================
// Taster gedrückt?
// ==================================================

bool Button::pressed()
{
    bool reading = mcp->digitalRead(pin);


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