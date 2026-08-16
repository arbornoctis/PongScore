#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;

// MCP23017 Eingänge
const uint8_t PLAYER1_PLUS  = 0;  // GPA0
const uint8_t PLAYER1_MINUS = 1;  // GPA1
const uint8_t PLAYER2_PLUS  = 2;  // GPA2
const uint8_t PLAYER2_MINUS = 3;  // GPA3


// --------------------------------------------------
// Button-Klasse
// --------------------------------------------------

class Button {
public:
    Button(uint8_t pin) {
        this->pin = pin;
    }

    void begin() {
        mcp.pinMode(pin, INPUT_PULLUP);

        currentState = mcp.digitalRead(pin);
        lastReading = currentState;
        lastDebounceTime = millis();
    }

    // Gibt genau einmal true zurück,
    // wenn der Taster gedrückt wurde.
    bool pressed() {

        bool reading = mcp.digitalRead(pin);

        // Hat sich der physikalische Zustand verändert?
        if (reading != lastReading) {
            lastDebounceTime = millis();
            lastReading = reading;
        }

        // Zustand muss für 30 ms stabil sein
        if ((millis() - lastDebounceTime) > debounceTime) {

            if (reading != currentState) {
                currentState = reading;

                // LOW bedeutet bei INPUT_PULLUP:
                // Taster wurde gedrückt.
                if (currentState == LOW) {
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


// --------------------------------------------------
// Unsere vier Taster
// --------------------------------------------------

Button player1Plus(PLAYER1_PLUS);
Button player1Minus(PLAYER1_MINUS);
Button player2Plus(PLAYER2_PLUS);
Button player2Minus(PLAYER2_MINUS);


// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup() {

    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("=== PongScore - Button Event Test ===");

    // MCP23017 initialisieren
    if (!mcp.begin_I2C(0x20)) {
        Serial.println("FEHLER: MCP23017 nicht gefunden!");

        while (true) {
            delay(1000);
        }
    }

    Serial.println("MCP23017 erfolgreich gefunden!");

    // Taster initialisieren
    player1Plus.begin();
    player1Minus.begin();
    player2Plus.begin();
    player2Minus.begin();

    Serial.println("Alle vier Taster initialisiert.");
    Serial.println();
}


// --------------------------------------------------
// Loop
// --------------------------------------------------

void loop() {

    if (player1Plus.pressed()) {
        Serial.println("EVENT: Spieler 1 PLUS");
    }

    if (player1Minus.pressed()) {
        Serial.println("EVENT: Spieler 1 MINUS");
    }

    if (player2Plus.pressed()) {
        Serial.println("EVENT: Spieler 2 PLUS");
    }

    if (player2Minus.pressed()) {
        Serial.println("EVENT: Spieler 2 MINUS");
    }
}