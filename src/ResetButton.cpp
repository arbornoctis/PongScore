#include "ResetButton.h"


// ==================================================
// Konstruktor
// ==================================================

ResetButton::ResetButton(uint8_t pin)
{
    this->pin = pin;
}


// ==================================================
// Initialisierung
// ==================================================

void ResetButton::begin()
{
    pinMode(pin, INPUT_PULLUP);

    currentState = digitalRead(pin);
    lastReading = currentState;

    lastDebounceTime = millis();
}


// ==================================================
// Taster aktualisieren
// ==================================================

void ResetButton::update()
{
    bool reading = digitalRead(pin);


    // ----------------------------------------------
    // Entprellung
    // ----------------------------------------------

    if (reading != lastReading)
    {
        lastDebounceTime = millis();
        lastReading = reading;
    }


    if ((millis() - lastDebounceTime) <= debounceTime)
    {
        return;
    }


    // ----------------------------------------------
    // Stabiler Zustand hat sich geändert
    // ----------------------------------------------

    if (reading != currentState)
    {
        currentState = reading;


        // ------------------------------------------
        // Taster wurde gedrückt
        // ------------------------------------------

        if (currentState == LOW)
        {
            pressStartTime = millis();
            longPressTriggered = false;
        }


        // ------------------------------------------
        // Taster wurde losgelassen
        // ------------------------------------------

        else
        {
            unsigned long pressDuration =
                millis() - pressStartTime;


            // Nur wenn noch kein langer Druck
            // erkannt wurde, kann es ein kurzer
            // Druck gewesen sein.

            if (!longPressTriggered &&
                pressDuration < longPressTime)
            {
                shortPressEvent = true;
            }
        }
    }


    // ----------------------------------------------
    // Langer Druck
    // ----------------------------------------------

    if (currentState == LOW &&
        !longPressTriggered &&
        (millis() - pressStartTime) >= longPressTime)
    {
        longPressTriggered = true;
        longPressEvent = true;
    }
}


// ==================================================
// Kurzer Druck erkannt?
// ==================================================

bool ResetButton::shortPressed()
{
    if (shortPressEvent)
    {
        shortPressEvent = false;
        return true;
    }

    return false;
}


// ==================================================
// Langer Druck erkannt?
// ==================================================

bool ResetButton::longPressed()
{
    if (longPressEvent)
    {
        longPressEvent = false;
        return true;
    }

    return false;
}