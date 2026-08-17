#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include <Adafruit_MCP23X17.h>


class Button
{
public:

    Button(
        Adafruit_MCP23X17* mcp,
        uint8_t pin
    );

    void begin();

    // Gibt genau einmal true zurück,
    // wenn der Taster gedrückt wurde.
    bool pressed();


private:

    Adafruit_MCP23X17* mcp;
    uint8_t pin;

    bool currentState = HIGH;
    bool lastReading = HIGH;

    unsigned long lastDebounceTime = 0;

    static const unsigned long debounceTime = 30;
};

#endif