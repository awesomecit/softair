#include <Arduino.h>
#include <SystemInitializer.h>
#include <BuzzerController.h>
#include <KeypadController.h>

// ═══════════════════════════════════════════════════════════════════════════
// MINIMAL TEST - BOOT SEQUENCE + KEYPAD HELLO WORLD
// ═══════════════════════════════════════════════════════════════════════════
// Test: SystemInitializer + BuzzerController + KeypadController
// Valida: boot LEDs + buzzer + keypad scan con debug seriale
// Pin assignment:
//   - Boot LEDs: 10 (red), 11 (orange), 12 (green)
//   - Buzzer: 9
//   - Keypad rows: 4, 5, 6, 7 (NO conflict with boot/buzzer)
//   - Keypad cols: A1, A2, A3, A4 (NO conflict with boot/buzzer)
// ═══════════════════════════════════════════════════════════════════════════

// System configuration
SystemConfig systemConfig = {
    .version = "1.0.0-minimal",
    .deviceName = "Arduino-Boot-Test",
    .networkSSID = "N/A",
    .baudRate = 115200,
    .redLedPin = 10,
    .orangeLedPin = 11,
    .greenLedPin = 12,
    .buzzerPin = 9,
    .blinkIntervalMs = 200
};

// Keypad configuration (4×4 membrane keypad)
const uint8_t KEYPAD_ROWS = 4;
const uint8_t KEYPAD_COLS = 4;
uint8_t rowPins[KEYPAD_ROWS] = {4, 5, 6, 7};    // Rows: R1-R4
uint8_t colPins[KEYPAD_COLS] = {A1, A2, A3, A4}; // Cols: C1-C4

// Global objects
SystemInitializer* systemInit = nullptr;
BuzzerController* buzzer = nullptr;
KeypadController* keypad = nullptr;

// Keypad timing debug (global counter per DEBUG_KEYPAD_TIMING in KeypadController)
volatile uint16_t keypadSlowScans = 0;

void setup() {
    Serial.begin(systemConfig.baudRate);
    while (!Serial) { delay(10); }
    
    Serial.println(F("\n\n╔══════════════════════════════════════════════════════╗"));
    Serial.println(F("║     MINIMAL TEST - Boot + Keypad Hello World         ║"));
    Serial.println(F("╚══════════════════════════════════════════════════════╝\n"));
    
    // Initialize buzzer
    buzzer = new BuzzerController(systemConfig.buzzerPin);
    if (buzzer) {
        buzzer->begin();
        Serial.println(F("✓ BuzzerController initialized"));
    }
    
    // Initialize system with boot sequence
    systemInit = new SystemInitializer(systemConfig, buzzer);
    if (systemInit) {
        Serial.println(F("✓ SystemInitializer created\n"));
        Serial.println(F("Starting boot sequence...\n"));
        
        bool success = systemInit->initialize();
        
        if (success) {
            Serial.println(F("\n╔══════════════════════════════════════════════════════╗"));
            Serial.println(F("║              ✓ BOOT TEST PASSED                      ║"));
            Serial.println(F("╚══════════════════════════════════════════════════════╝\n"));
            
            // Disable boot LEDs after successful init (release pins)
            systemInit->disableBootLeds();
            Serial.println(F("✓ Boot LEDs disabled (pins released)\n"));
        } else {
            Serial.println(F("\n✗ BOOT TEST FAILED - Check hardware connections"));
            return;
        }
    }
    
    // Initialize keypad after boot sequence
    Serial.println(F("Initializing keypad..."));
    Serial.print(F("  Rows: "));
    for (uint8_t i = 0; i < KEYPAD_ROWS; i++) {
        Serial.print(rowPins[i]); Serial.print(F(" "));
    }
    Serial.println();
    Serial.print(F("  Cols: A"));
    for (uint8_t i = 0; i < KEYPAD_COLS; i++) {
        Serial.print(colPins[i] - A0); Serial.print(F(" "));
    }
    Serial.println();
    
    keypad = new KeypadController(rowPins, colPins);
    if (keypad) {
        keypad->begin();
        Serial.println(F("✓ KeypadController initialized\n"));
        
        Serial.println(F("╔══════════════════════════════════════════════════════╗"));
        Serial.println(F("║           KEYPAD HELLO WORLD - READY                 ║"));
        Serial.println(F("╠══════════════════════════════════════════════════════╣"));
        Serial.println(F("║  Press any key on the 4×4 membrane keypad:          ║"));
        Serial.println(F("║    Layout: 1 2 3 A                                   ║"));
        Serial.println(F("║            4 5 6 B                                   ║"));
        Serial.println(F("║            7 8 9 C                                   ║"));
        Serial.println(F("║            * 0 # D                                   ║"));
        Serial.println(F("║                                                      ║"));
        Serial.println(F("║  Each keypress will be logged to Serial with:       ║"));
        Serial.println(F("║    [KEY] X - detected key                            ║"));
        Serial.println(F("║    [SCAN] XX µs - scan time (timing debug)          ║"));
        Serial.println(F("╚══════════════════════════════════════════════════════╝\n"));
    } else {
        Serial.println(F("✗ Failed to create KeypadController"));
    }
}

void loop() {
    if (!keypad) {
        delay(1000);
        return;
    }
    
    // Scan keypad for presses
    KeypadKey key = keypad->scan();
    
    if (key != KeypadKey::NONE) {
        // Print key press with visual formatting
        Serial.print(F("\n[KEY] "));
        
        // Map key enum to character
        switch (key) {
            case KeypadKey::KEY_0: Serial.print(F("0")); break;
            case KeypadKey::KEY_1: Serial.print(F("1")); break;
            case KeypadKey::KEY_2: Serial.print(F("2")); break;
            case KeypadKey::KEY_3: Serial.print(F("3")); break;
            case KeypadKey::KEY_4: Serial.print(F("4")); break;
            case KeypadKey::KEY_5: Serial.print(F("5")); break;
            case KeypadKey::KEY_6: Serial.print(F("6")); break;
            case KeypadKey::KEY_7: Serial.print(F("7")); break;
            case KeypadKey::KEY_8: Serial.print(F("8")); break;
            case KeypadKey::KEY_9: Serial.print(F("9")); break;
            case KeypadKey::KEY_A: Serial.print(F("A")); break;
            case KeypadKey::KEY_B: Serial.print(F("B")); break;
            case KeypadKey::KEY_C: Serial.print(F("C")); break;
            case KeypadKey::KEY_D: Serial.print(F("D")); break;
            case KeypadKey::KEY_STAR: Serial.print(F("*")); break;
            case KeypadKey::KEY_HASH: Serial.print(F("#")); break;
            default: Serial.print(F("UNKNOWN")); break;
        }
        
        Serial.println(F(" - detected ✓"));
        
        // Optional: beep on keypress for audio feedback
        if (buzzer) {
            buzzer->playSuccess(); // Short beep
        }
    }
    
    // Small delay to avoid overwhelming Serial output
    delay(10);
}
