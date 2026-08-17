#include <Arduino.h>
#include <MD_MAX72xx.h>

#include "Digits.h"


// --------------------------------------------------
// MAX7219
// --------------------------------------------------

const uint8_t DIN_PIN = 23;
const uint8_t CLK_PIN = 18;
const uint8_t CS_PIN  = 17;

const uint8_t NUM_DEVICES = 4;


MD_MAX72XX mx = MD_MAX72XX(
    MD_MAX72XX::FC16_HW,
    DIN_PIN,
    CLK_PIN,
    CS_PIN,
    NUM_DEVICES
);


// --------------------------------------------------
// Ziffern
// --------------------------------------------------

Digits digits(&mx);


// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("================================");
    Serial.println(" PongScore - Zahlen 0 bis 19");
    Serial.println("================================");
    Serial.println();

    mx.begin();

    mx.control(
        MD_MAX72XX::INTENSITY,
        5
    );

    mx.clear();


    // Vier verschiedene Testzahlen
    digits.drawNumber(0, 0);
    digits.drawNumber(1, 1);
    digits.drawNumber(2, 17);
    digits.drawNumber(3, 18);


    Serial.println(
        "Angezeigt: 0 | 1 | 17 | 18"
    );
}


// --------------------------------------------------
// Loop
// --------------------------------------------------

void loop()
{
}