#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <Adafruit_MCP23X17.h>

#include "Digits.h"
#include "Button.h"
#include "ResetButton.h"


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

const uint8_t RESET_BUTTON  = 15; // GPB7


// MCP23017 Ausgänge
const uint8_t PLAYER1_LED = 10;   // GPB2
const uint8_t PLAYER2_LED = 11;   // GPB3


// ==================================================
// Player-Score-Taster
// ==================================================

Button player1Plus(&mcp, PLAYER1_PLUS);
Button player1Minus(&mcp, PLAYER1_MINUS);

Button player2Plus(&mcp, PLAYER2_PLUS);
Button player2Minus(&mcp, PLAYER2_MINUS);


// ==================================================
// Reset-Taster
// ==================================================

ResetButton resetButton(&mcp, RESET_BUTTON);


// ==================================================
// Spielstand
// ==================================================

int player1Score = 0;
int player2Score = 0;


// ==================================================
// Aufschlag
// ==================================================
//
// true  = Spieler 1 hat Aufschlag
// false = Spieler 2 hat Aufschlag
//

bool player1Serves = true;


// ==================================================
// Funktionsdeklarationen
// ==================================================

void updateServeLEDs();

void updateDisplays();

void printScore();


// ==================================================
// Aufschlag-LEDs anzeigen
// ==================================================

void updateServeLEDs()
{
    if (player1Serves)
    {
        mcp.digitalWrite(PLAYER1_LED, HIGH);
        mcp.digitalWrite(PLAYER2_LED, LOW);
    }
    else
    {
        mcp.digitalWrite(PLAYER1_LED, LOW);
        mcp.digitalWrite(PLAYER2_LED, HIGH);
    }
}


// ==================================================
// Spielstand anzeigen
// ==================================================

void updateDisplays()
{
    // Display 0 = Spieler 1
    digits.drawNumber(0, player1Score);

    // Display 1 = Spieler 2
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

    resetButton.begin();


    Serial.println(
        "Alle fünf Taster initialisiert."
    );


    // ----------------------------------------------
    // Aufschlag-LEDs initialisieren
    // ----------------------------------------------

    mcp.pinMode(PLAYER1_LED, OUTPUT);
    mcp.pinMode(PLAYER2_LED, OUTPUT);

    updateServeLEDs();


    // ----------------------------------------------
    // Start-Spielstand anzeigen
    // ----------------------------------------------

    updateDisplays();

    printScore();


/*  // Vier verschiedene Testzahlen
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


    // ----------------------------------------------
    // Reset-Taster
    // ----------------------------------------------

    resetButton.update();


    // ----------------------------------------------
    // Langer Druck
    // ----------------------------------------------

    if (resetButton.longPressed())
    {
        player1Score = 0;
        player2Score = 0;

        updateDisplays();

        Serial.println("RESET - neues Match");
        printScore();
    }


    // ----------------------------------------------
    // Kurzer Druck
    // ----------------------------------------------

    if (resetButton.shortPressed())
    {
        // Der anfängliche Aufschlag darf nur
        // bei 0 : 0 gewechselt werden.

        if (player1Score == 0 &&
            player2Score == 0)
        {
            player1Serves = !player1Serves;

            updateServeLEDs();

            Serial.print(
                "Anfänglicher Aufschlag: Spieler "
            );

            if (player1Serves)
            {
                Serial.println("1");
            }
            else
            {
                Serial.println("2");
            }
        }
    }
}