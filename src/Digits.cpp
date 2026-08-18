#include "Digits.h"


// ==================================================
// ZIFFERN
// ==================================================
//
// Alle normalen Ziffern:
//
// Breite: 5 Pixel
// Höhe:   7 Pixel
//
// Bit 4 = links
// Bit 0 = rechts
//
// Diese Ziffern werden sowohl für 0-9 als auch
// als Einerstelle von 10-19 verwendet.
// ==================================================

const uint8_t DIGITS[10][7] =
{
    // 0
    {
        0b11111,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b11111
    },

    // 1
    {
        0b00100,
        0b01100,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00100
    },

    // 2
    {
        0b11111,
        0b00001,
        0b00001,
        0b11111,
        0b10000,
        0b10000,
        0b11111
    },

    // 3
    {
        0b11111,
        0b00001,
        0b00001,
        0b11111,
        0b00001,
        0b00001,
        0b11111
    },

    // 4
    {
        0b10001,
        0b10001,
        0b10001,
        0b11111,
        0b00001,
        0b00001,
        0b00001
    },

    // 5
    {
        0b11111,
        0b10000,
        0b10000,
        0b11111,
        0b00001,
        0b00001,
        0b11111
    },

    // 6
    {
        0b11111,
        0b10000,
        0b10000,
        0b11111,
        0b10001,
        0b10001,
        0b11111
    },

    // 7
    {
        0b11111,
        0b00001,
        0b00010,
        0b00100,
        0b01000,
        0b01000,
        0b01000
    },

    // 8
    {
        0b11111,
        0b10001,
        0b10001,
        0b11111,
        0b10001,
        0b10001,
        0b11111
    },

    // 9
    {
        0b11111,
        0b10001,
        0b10001,
        0b11111,
        0b00001,
        0b00001,
        0b11111
    }
};


// ==================================================
// Zehner-1
// ==================================================
//
// Breite: 2 Pixel
// Höhe:   7 Pixel
//
// Diese Ziffer wird VOR der Rotation in das
// virtuelle 8x8-Bild eingesetzt.
//
// Wichtig:
// Die unterste Zeile ist absichtlich nur 1 Pixel
// breit. Dadurch entsteht nach der Rotation eine
// korrekte schmale "1".
// ==================================================

const uint8_t TENS_ONE[7] =
{
    0b01,
    0b11,
    0b01,
    0b01,
    0b01,
    0b01,
    0b01
};


// ==================================================
// Konstruktor
// ==================================================

Digits::Digits(MD_MAX72XX* display)
{
    this->display = display;
}


// ==================================================
// Pixel setzen
// ==================================================
//
// Logische Koordinaten:
//
// x = horizontal
// y = vertikal
//
// Die physische Matrix ist links/rechts gespiegelt.
//
// Die eigentliche Rotation wird bei der Darstellung
// des kompletten 8x8-Canvas durchgeführt.
// ==================================================

void Digits::setPixel(
    uint8_t device,
    uint8_t x,
    uint8_t y,
    bool state
)
{
    // Hardware-Spiegelung
    uint8_t physicalX = 7 - x;

    uint16_t globalX =
        device * 8 + physicalX;

    display->setPoint(
        y,
        globalX,
        state
    );
}


// ==================================================
// Normale 5x7-Ziffer in ein 8x8-Canvas schreiben
// ==================================================

void drawDigitToCanvas(
    uint8_t canvas[8][8],
    const uint8_t bitmap[7],
    uint8_t offsetX,
    uint8_t offsetY
)
{
    for (uint8_t y = 0; y < 7; y++)
    {
        for (uint8_t x = 0; x < 5; x++)
        {
            bool pixel =
                bitmap[y] &
                (1 << (4 - x));

            if (pixel)
            {
                canvas[offsetY + y][offsetX + x] = 1;
            }
        }
    }
}


// ==================================================
// Zehner-1 in das 8x8-Canvas schreiben
// ==================================================

void drawTensOneToCanvas(
    uint8_t canvas[8][8],
    uint8_t offsetX,
    uint8_t offsetY
)
{
    for (uint8_t y = 0; y < 7; y++)
    {
        for (uint8_t x = 0; x < 2; x++)
        {
            bool pixel =
                TENS_ONE[y] &
                (1 << (1 - x));

            if (pixel)
            {
                canvas[offsetY + y][offsetX + x] = 1;
            }
        }
    }
}


// ==================================================
// 8x8-Canvas transformieren und anzeigen
// ==================================================
//
// Das komplette Bild wird gemeinsam:
//
// 1. um 90° im Uhrzeigersinn gedreht
// 2. anschließend durch setPixel() hardwarebedingt
//    horizontal gespiegelt.
//
// Dadurch werden beide Ziffern exakt gemeinsam
// transformiert.
// ==================================================

void Digits::drawCanvas(
    uint8_t device,
    uint8_t canvas[8][8]
)
{
    for (uint8_t y = 0; y < 8; y++)
    {
        for (uint8_t x = 0; x < 8; x++)
        {
            if (!canvas[y][x])
                continue;


            // 90° im Uhrzeigersinn
            //
            // 8x8:
            // (x,y) -> (7-y,x)

            uint8_t rotatedX =
                7 - y;

            uint8_t rotatedY =
                x;


            setPixel(
                device,
                rotatedX,
                rotatedY,
                true
            );
        }
    }
}


// ==================================================
// Zahl 0-19 anzeigen
// ==================================================

void Digits::drawNumber(
    uint8_t device,
    uint8_t number
)
{
    // Ungültige Zahl
    if (number > 19)
    {
        clear(device);
        return;
    }


    // ----------------------------------------------
    // Modul zunächst löschen
    // ----------------------------------------------

    clear(device);


    // ----------------------------------------------
    // 0-9
    // ----------------------------------------------
    //
    // Dieser Teil bleibt exakt wie bisher.
    // Dadurch verändern wir die funktionierenden
    // einstelligen Zahlen nicht.
    // ----------------------------------------------

    if (number < 10)
    {
        drawDigitBitmap(
            device,
            DIGITS[number],
            0, 3
        );

        return;
    }


    // ----------------------------------------------
    // 10-19
    // ----------------------------------------------

    uint8_t ones =
        number % 10;


    // ----------------------------------------------
    // Virtuelles 8x8-Bild
    // ----------------------------------------------

    uint8_t canvas[8][8] = {};


    // ----------------------------------------------
    // Zehner-1
    //
    // Position:
    // x = 0..1
    // y = 1..7
    //
    // Dadurch bleibt die Darstellung nach der
    // Rotation an derselben Stelle wie unsere
    // bisherigen einstelligen Ziffern.
    // ----------------------------------------------

    drawTensOneToCanvas(
        canvas,
        0,
        1
    );


    // ----------------------------------------------
    // Einer-Ziffer
    //
    // Position:
    // x = 2..6
    // y = 1..7
    //
    // Die beiden Ziffern stehen damit wirklich
    // nebeneinander und können sich nicht mehr
    // überlagern.
    // ----------------------------------------------

    drawDigitToCanvas(
        canvas,
        DIGITS[ones],
        3,
        1
    );


    // ----------------------------------------------
    // Komplettes Bild gemeinsam transformieren
    // ----------------------------------------------

    drawCanvas(
        device,
        canvas
    );
}


// ==================================================
// Normale 5x7-Ziffer zeichnen
// ==================================================
//
// Dieser Code wird ausschließlich für 0-9 benutzt.
// Er bleibt unverändert gegenüber der bisherigen
// funktionierenden Version.
// ==================================================

void Digits::drawDigitBitmap(
    uint8_t device,
    const uint8_t bitmap[7],
    uint8_t offsetX,
    uint8_t offsetY
)
{
    const uint8_t originalWidth  = 5;
    const uint8_t originalHeight = 7;


    for (uint8_t y = 0; y < originalHeight; y++)
    {
        for (uint8_t x = 0; x < originalWidth; x++)
        {
            bool pixel =
                bitmap[y] &
                (1 << (4 - x));

            if (!pixel)
                continue;


            // 90° im Uhrzeigersinn
            uint8_t rotatedX =
                originalHeight - 1 - y;

            uint8_t rotatedY =
                x;


            setPixel(
                device,
                rotatedX + offsetX,
                rotatedY + offsetY,
                true
            );
        }
    }
}


// ==================================================
// Modul löschen
// ==================================================

void Digits::clear(
    uint8_t device
)
{
    for (uint8_t x = 0; x < 8; x++)
    {
        for (uint8_t y = 0; y < 8; y++)
        {
            setPixel(
                device,
                x,
                y,
                false
            );
        }
    }
}