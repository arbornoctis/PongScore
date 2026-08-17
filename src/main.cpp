#include <Arduino.h>
#include <MD_MAX72xx.h>

// --------------------------------------------------
// MAX7219 Anschlüsse
// --------------------------------------------------

const uint8_t DIN_PIN = 23;
const uint8_t CLK_PIN = 18;
const uint8_t CS_PIN  = 17;

const uint8_t NUM_DEVICES = 4;


// --------------------------------------------------
// MAX7219 initialisieren
// --------------------------------------------------

MD_MAX72XX mx = MD_MAX72XX(
    MD_MAX72XX::FC16_HW,
    DIN_PIN,
    CLK_PIN,
    CS_PIN,
    NUM_DEVICES
);


// --------------------------------------------------
// Pixel setzen
//
// Unsere logischen Koordinaten:
//
// x = 0 ... 7  -> horizontal von links nach rechts
// y = 0 ... 7  -> vertikal von oben nach unten
//
// MD_MAX72XX erwartet bei unseren Modulen:
//
// 1. Parameter = vertikale Position
// 2. Parameter = horizontale Position
// --------------------------------------------------

void setPixel(uint8_t device, uint8_t x, uint8_t y, bool state)
{
    uint16_t globalX = device * 8 + x;

    mx.setPoint(y, globalX, state);
}


// --------------------------------------------------
// Ein einzelnes 8x8-Muster anzeigen
// --------------------------------------------------

void drawTestPattern(uint8_t device)
{
    // Modul löschen
    for (uint8_t x = 0; x < 8; x++)
    {
        for (uint8_t y = 0; y < 8; y++)
        {
            setPixel(device, x, y, false);
        }
    }


    // ------------------------------------------------
    // Asymmetrisches Testmuster
    // ------------------------------------------------

    // Linke obere Ecke
    setPixel(device, 0, 0, true);
    setPixel(device, 1, 0, true);
    setPixel(device, 0, 1, true);

    // Rechte obere Ecke
    setPixel(device, 6, 0, true);
    setPixel(device, 7, 0, true);

    // Diagonale
    for (uint8_t i = 0; i < 8; i++)
    {
        setPixel(device, i, i, true);
    }

    // Zusätzliches Muster unten rechts
    setPixel(device, 5, 6, true);
    setPixel(device, 6, 6, true);
    setPixel(device, 7, 6, true);

    setPixel(device, 6, 7, true);
    setPixel(device, 7, 7, true);
}


// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("================================");
    Serial.println(" PongScore - Matrix Orientierung");
    Serial.println("================================");
    Serial.println();

    mx.begin();

    // Helligkeit 0 ... 15
    mx.control(MD_MAX72XX::INTENSITY, 5);

    mx.clear();

    // Dasselbe Muster auf allen vier Modulen
    for (uint8_t device = 0; device < NUM_DEVICES; device++)
    {
        drawTestPattern(device);
    }

    Serial.println("Testmuster auf allen vier Modulen angezeigt.");
}


// --------------------------------------------------
// Loop
// --------------------------------------------------

void loop()
{
}