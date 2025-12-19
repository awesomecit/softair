#include "KeypadController.h"
#include <Arduino.h>  // For Serial, micros()

// Definizione della matrice statica (required per constexpr array)
constexpr KeypadKey KeypadController::KEY_MAP[4][4];

// Debug profiling (Task 001)
#define DEBUG_KEYPAD_TIMING  // Enable scan timing for validation

// Extern counter for slow scan tracking (declared in main.cpp)
extern uint16_t keypadSlowScans;

KeypadController::KeypadController(const uint8_t rowPins[4], const uint8_t colPins[4])
    : lastKey_(KeypadKey::NONE)
    , debounceTimestamp_(0)
{
    // Copia pin arrays
    for (uint8_t i = 0; i < 4; i++) {
        rowPins_[i] = rowPins[i];
        colPins_[i] = colPins[i];
    }
}

void KeypadController::begin() {
    // Configura row pins: OUTPUT, HIGH (idle = non attivo)
    for (uint8_t i = 0; i < 4; i++) {
        pinMode(rowPins_[i], OUTPUT);
        digitalWrite(rowPins_[i], HIGH);
    }
    
    // Configura col pins: INPUT_PULLUP (legge LOW quando row attiva)
    for (uint8_t i = 0; i < 4; i++) {
        pinMode(colPins_[i], INPUT_PULLUP);
    }
}

KeypadKey KeypadController::scan() {
    #ifdef DEBUG_KEYPAD_TIMING
    unsigned long scanStart = micros();
    #endif
    
    // Check debounce window (converti ms → µs per confronto con micros())
    unsigned long now = micros();
    if ((now - debounceTimestamp_) < (DEBOUNCE_DELAY_MS * 1000UL)) {
        return KeypadKey::NONE;  // Ancora in debounce
    }
    
    // Scansiona matrice 4×4
    KeypadKey currentKey = KeypadKey::NONE;
    
    for (uint8_t row = 0; row < 4; row++) {
        // Attiva riga corrente (LOW)
        digitalWrite(rowPins_[row], LOW);
        
        // Piccolo delay per stabilizzare (1µs sufficiente per ATmega328P)
        delayMicroseconds(1);
        
        // Leggi tutte le colonne
        for (uint8_t col = 0; col < 4; col++) {
            if (digitalRead(colPins_[col]) == LOW) {
                // Chiave premuta a [row, col]
                currentKey = KEY_MAP[row][col];
                break;  // Esci dal loop colonne (una chiave alla volta)
            }
        }
        
        // Disattiva riga (HIGH)
        digitalWrite(rowPins_[row], HIGH);
        
        // Se trovata chiave, esci dal loop righe
        if (currentKey != KeypadKey::NONE) {
            break;
        }
    }
    
    // Edge detection: ritorna chiave solo se cambiata (pressione nuova)
    KeypadKey result = KeypadKey::NONE;
    if (currentKey != lastKey_) {
        if (currentKey != KeypadKey::NONE) {
            // Nuova pressione (LOW→HIGH transition logico)
            result = currentKey;
            debounceTimestamp_ = now;  // Resetta debounce timer
        } else {
            // Rilascio chiave (HIGH→LOW transition)
            // Non ritorniamo niente, ma aggiorniamo timestamp per debounce
            debounceTimestamp_ = now;
        }
        lastKey_ = currentKey;
    }
    
    #ifdef DEBUG_KEYPAD_TIMING
    unsigned long scanDuration = micros() - scanStart;
    if (scanDuration > 500) {  // Report solo se >500µs (slow scan)
        keypadSlowScans++;  // Increment counter for summary
        Serial.print(F("Keypad scan: "));
        Serial.print(scanDuration);
        Serial.println(F(" µs"));
    }
    #endif
    
    return result;
}

char KeypadController::keyToChar(KeypadKey key) {
    switch (key) {
        case KeypadKey::KEY_0: return '0';
        case KeypadKey::KEY_1: return '1';
        case KeypadKey::KEY_2: return '2';
        case KeypadKey::KEY_3: return '3';
        case KeypadKey::KEY_4: return '4';
        case KeypadKey::KEY_5: return '5';
        case KeypadKey::KEY_6: return '6';
        case KeypadKey::KEY_7: return '7';
        case KeypadKey::KEY_8: return '8';
        case KeypadKey::KEY_9: return '9';
        case KeypadKey::KEY_A: return 'A';
        case KeypadKey::KEY_B: return 'B';
        case KeypadKey::KEY_C: return 'C';
        case KeypadKey::KEY_D: return 'D';
        case KeypadKey::KEY_STAR: return '*';
        case KeypadKey::KEY_HASH: return '#';
        case KeypadKey::NONE:
        default: return '?';
    }
}
