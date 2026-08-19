#ifndef DIGITS_H
#define DIGITS_H

#include <Arduino.h>
#include <MD_MAX72xx.h>

class Digits
{
public:

    Digits(MD_MAX72XX* display);

    // Zahl von 0 bis 19 anzeigen
    void drawNumber(uint8_t device, uint8_t number);

    // Modul löschen
    void clear(uint8_t device);

private:

    MD_MAX72XX* display;

    // Logischen Pixel setzen
    void setPixel(
        uint8_t device,
        uint8_t x,
        uint8_t y,
        bool state
    );

    // Eine normale 5x7-Ziffer zeichnen
    void drawDigitBitmap(
        uint8_t device,
        const uint8_t bitmap[7],
        uint8_t offsetX,
        uint8_t offsetY
    );

    // Komplettes 8x8-Bild transformieren und anzeigen
    void drawCanvas(
        uint8_t device,
        uint8_t canvas[8][8]
    );
};

#endif