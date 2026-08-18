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
int player1Sets = 0;
int player2Sets = 0;

bool setFinished = false;
bool matchFinished = false;
bool scoreVisible = true;

unsigned long lastBlinkTime = 0;
const unsigned long BLINK_INTERVAL = 750;


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

void startNextSet();
void updateSetFinishedBlink();
void finishSet();
bool isSetFinished();
void updateDisplays();
void printScore();
void updateServeLEDs();

// ==================================================
// Nächsten Satz starten und Satzscore vertauschen
// ==================================================

void startNextSet()
{
    // Seitenwechsel
    bool tempServe = player1Serves;
    player1Serves = !tempServe;

    // Satzanzeigen auf die neue Tischseite beziehen
    int tempSets = player1Sets;
    player1Sets = player2Sets;
    player2Sets = tempSets;

    // Neuer Satz beginnt bei 0:0
    player1Score = 0;
    player2Score = 0;

    setFinished = false;
    scoreVisible = true;

    updateDisplays();
    updateServeLEDs();

    printScore();
}

// ==================================================
// Blinken einschalten
// ==================================================

void updateSetFinishedBlink()
{
    if (!setFinished)
        return;

    unsigned long currentTime = millis();

    if (currentTime - lastBlinkTime >= BLINK_INTERVAL)
    {
        lastBlinkTime = currentTime;

        scoreVisible = !scoreVisible;

        if (scoreVisible)
        {
            // Anzeigen wieder herstellen
            if (matchFinished)
            {
                // Alle vier Displays wieder anzeigen
                updateDisplays();
            }
            else
            {
                // Nur Punktestand wieder anzeigen
                digits.drawNumber(0, player1Score);
                digits.drawNumber(1, player2Score);
            }
        }
        else
        {
            if (matchFinished)
            {
                // Alle vier Displays löschen
                mx.clear();
            }
            else
            {
                // Nur Punktanzeigen löschen
                mx.clear(0);
                mx.clear(1);
            }
        }
    }
}

// ==================================================
// Gewonnenen Satz verarbeiten und Ergebnis blinken lassen
// ==================================================

void finishSet()
{
    if (player1Score >= 11 &&
        player1Score - player2Score >= 2)
    {
        player1Sets++;
    }
    else if (player2Score >= 11 &&
             player2Score - player1Score >= 2)
    {
        player2Sets++;
    }

    setFinished = true;
    scoreVisible = true;
    lastBlinkTime = millis();

    if (player1Sets >= 3 || player2Sets >= 3)
    {
        matchFinished = true;
    }

    updateDisplays();
}

// ==================================================
// Satzende überprüfen
// ==================================================

bool isSetFinished()
{
    if (player1Score >= 11 &&
        player1Score - player2Score >= 2)
    {
        return true;
    }

    if (player2Score >= 11 &&
        player2Score - player1Score >= 2)
    {
        return true;
    }

    return false;
}

// ==================================================
// Spielstand anzeigen
// ==================================================

void updateDisplays()
{
    // Punktestand
    digits.drawNumber(0, player1Score);
    digits.drawNumber(1, player2Score);
    // Satzstand
    digits.drawNumber(2, player1Sets);
    digits.drawNumber(3, player2Sets);
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
// Aufschlag-LED steuern
// --------------------------------------------------

void updateServeLEDs()
{
    int totalScore = player1Score + player2Score;

    if (player1Score > 9 && player2Score > 9)
    {
        // Ab 10:10 wechselt der Aufschlag nach jedem Punkt.
        // player1Serves bestimmt, wer den ersten Aufschlag des Satzes hatte.

        if (player1Serves && totalScore % 2 == 0)
        {
            mcp.digitalWrite(PLAYER1_LED, HIGH);
            mcp.digitalWrite(PLAYER2_LED, LOW);
        }
        else if (!player1Serves && totalScore % 2 == 1)
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
    else
    {
        // Normaler Satz:
        // Aufschlag wechselt alle zwei Punkte.

        bool player1HasServe;

        if (player1Serves)
            player1HasServe = (totalScore % 4 < 2);
        else
            player1HasServe = (totalScore % 4 >= 2);

        if (player1HasServe)
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

    updateSetFinishedBlink();

    // ----------------------------------------------
    // Spieler 1
    // ----------------------------------------------

    if (!setFinished && player1Plus.pressed())
    {
        if (player1Score < 19)
        {
            player1Score++;

            updateDisplays();
            
            if (isSetFinished())
            {
                finishSet();
            }
            updateServeLEDs();

            Serial.println("Spieler 1: +1");
            printScore();
        }
        updateServeLEDs();
    }


    if (!setFinished && player1Minus.pressed())
    {
        if (player1Score > 0)
        {
            player1Score--;

            updateDisplays();

            if (isSetFinished())
            {
                finishSet();
            }
            updateServeLEDs();

            Serial.println("Spieler 1: -1");
            printScore();
        }
        updateServeLEDs();
    }


    // ----------------------------------------------
    // Spieler 2
    // ----------------------------------------------

    if (!setFinished && player2Plus.pressed())
    {
        if (player2Score < 19)
        {
            player2Score++;

            updateDisplays();

            if (isSetFinished())
            {
                finishSet();
            }
            updateServeLEDs();

            Serial.println("Spieler 2: +1");
            printScore();
        }
        updateServeLEDs();
    }


    if (!setFinished && player2Minus.pressed())
    {
        if (player2Score > 0)
        {
            player2Score--;

            updateDisplays();

            if (isSetFinished())
            {
                finishSet();
            }
            updateServeLEDs();

            Serial.println("Spieler 2: -1");
            printScore();
        }
        updateServeLEDs();
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
        player1Sets = 0;
        player2Sets = 0;

        setFinished = false;
        matchFinished = false;
        scoreVisible = true;

        updateDisplays();
        updateServeLEDs();

        Serial.println("RESET - neues Match");
        printScore();
    }


    // ----------------------------------------------
    // Kurzer Druck
    // ----------------------------------------------

    if (resetButton.shortPressed())
    {
        // Wenn der Satz beendet ist:
        // bei Tastendruck neuen Satz beginnen
        if (setFinished && !matchFinished)
        {
            startNextSet();
        }
        // Der anfängliche Aufschlag darf nur
        // bei 0 : 0 gewechselt werden.
        else if (player1Score == 0 &&
            player2Score == 0 &&
            player1Sets == 0 &&
            player2Sets == 0)
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