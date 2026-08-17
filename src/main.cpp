#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <Adafruit_MCP23X17.h>
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

// ==================================================
// MCP23017
// ==================================================

Adafruit_MCP23X17 mcp;


// MCP23017 Eingänge
const uint8_t PLAYER1_PLUS  = 0;  // GPA0
const uint8_t PLAYER1_MINUS = 1;  // GPA1
const uint8_t PLAYER2_PLUS  = 2;  // GPA2
const uint8_t PLAYER2_MINUS = 3;  // GPA3


// ==================================================
// Button-Klasse
// ==================================================

class Button
{
public:

    Button(uint8_t pin)
    {
        this->pin = pin;
    }


    void begin()
    {
        mcp.pinMode(pin, INPUT_PULLUP);

        currentState = mcp.digitalRead(pin);
        lastReading = currentState;
        lastDebounceTime = millis();
    }


    // Gibt genau einmal true zurück,
    // wenn der Taster gedrückt wurde.
    bool pressed()
    {
        bool reading = mcp.digitalRead(pin);


        // Hat sich der physikalische Zustand verändert?
        if (reading != lastReading)
        {
            lastDebounceTime = millis();
            lastReading = reading;
        }


        // Zustand muss für 30 ms stabil sein
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


private:

    uint8_t pin;

    bool currentState = HIGH;
    bool lastReading = HIGH;

    unsigned long lastDebounceTime = 0;

    static const unsigned long debounceTime = 30;
};


// ==================================================
// Unsere vier Taster
// ==================================================

Button player1Plus(PLAYER1_PLUS);
Button player1Minus(PLAYER1_MINUS);

Button player2Plus(PLAYER2_PLUS);
Button player2Minus(PLAYER2_MINUS);


// ==================================================
// Spielstand
// ==================================================

int player1Score = 0;
int player2Score = 0;


// ==================================================
// Spielstand anzeigen
// ==================================================

void updateDisplays()
{
    digits.drawNumber(0, player1Score);
    digits.drawNumber(1, player2Score);
}


// ==================================================
// Spielstand ausgeben
// ==================================================

void printScore()
{
    Serial.print("Spielstand: ");
    Serial.print(player1Score);
    Serial.print(" : ");
    Serial.println(player2Score);
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
    Serial.println(" PongScore - Punktzahl");
    Serial.println("================================");
    Serial.println();

    // ----------------------------------------------
    // MAX7219 initialisieren
    // ----------------------------------------------

    mx.begin();

    mx.control(
        MD_MAX72XX::INTENSITY,
        5
    );

    mx.clear();

    // ----------------------------------------------
    // MCP23017 initialisieren
    // ----------------------------------------------

    if (!mcp.begin_I2C(0x20))
    {
        Serial.println(
            "FEHLER: MCP23017 nicht gefunden!"
        );

        while (true)
        {
            delay(1000);
        }
    }

    Serial.println(
        "MCP23017 erfolgreich gefunden!"
    );

    // ----------------------------------------------
    // Taster initialisieren
    // ----------------------------------------------

    player1Plus.begin();
    player1Minus.begin();

    player2Plus.begin();
    player2Minus.begin();


    Serial.println(
        "Alle vier Taster initialisiert."
    );


    // ----------------------------------------------
    // Start-Spielstand anzeigen
    // ----------------------------------------------

    updateDisplays();

    printScore();

/*   // Vier verschiedene Testzahlen
    digits.drawNumber(0, 0);
    digits.drawNumber(1, 1);
    digits.drawNumber(2, 17);
    digits.drawNumber(3, 18);
*/


}


// --------------------------------------------------
// Loop
// --------------------------------------------------

void loop()
{
    // ----------------------------------------------
    // Spieler 1
    // ----------------------------------------------

    if (player1Plus.pressed())
    {
        if (player1Score < 19)
        {
            player1Score++;

            updateDisplays();

            Serial.println("Spieler 1: +1");
            printScore();
        }
    }


    if (player1Minus.pressed())
    {
        if (player1Score > 0)
        {
            player1Score--;

            updateDisplays();

            Serial.println("Spieler 1: -1");
            printScore();
        }
    }

    // ----------------------------------------------
    // Spieler 2
    // ----------------------------------------------

    if (player2Plus.pressed())
    {
        if (player2Score < 19)
        {
            player2Score++;

            updateDisplays();

            Serial.println("Spieler 2: +1");
            printScore();
        }
    }


    if (player2Minus.pressed())
    {
        if (player2Score > 0)
        {
            player2Score--;

            updateDisplays();

            Serial.println("Spieler 2: -1");
            printScore();
        }
    }    
}