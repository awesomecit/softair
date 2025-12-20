#include <Arduino.h>
#include <SystemInitializer.h>
#include <BuzzerController.h>
#include <KeypadController.h>
#include <DisplayController.h>

// ═══════════════════════════════════════════════════════════════════════════
// INCREMENTAL TEST - BOOT + KEYPAD + DISPLAY
// ═══════════════════════════════════════════════════════════════════════════
// Test: SystemInitializer + BuzzerController + KeypadController + DisplayController
// Valida: boot LEDs + buzzer + keypad + display 7-segment con 74HC595
// Pin assignment:
//   - Boot LEDs: 10,11,12 (disabilitati dopo init, RIUSATI per display digit select)
//   - Buzzer: 9
//   - Keypad rows: 4, 5, 6, 7
//   - Keypad cols: A1, A2, A3, A4
//   - Display 74HC595: 2 (data), 3 (clock), A0 (latch)
//   - Display digit select: 10, 11, 12, A5 (COM cathode multiplexing)
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

// Display configuration (74HC595 + 4× 7-segment common cathode)
const uint8_t DISPLAY_DATA_PIN = 2;   // DS (serial data)
const uint8_t DISPLAY_CLOCK_PIN = 3;  // SHCP (shift clock)
const uint8_t DISPLAY_LATCH_PIN = A0; // STCP (latch)
const uint8_t DISPLAY_DIGIT_PINS[4] = {10, 11, 12, A5}; // D1-D4 COM (riuso pin boot LED!)

// Global objects
SystemInitializer* systemInit = nullptr;
BuzzerController* buzzer = nullptr;
KeypadController* keypad = nullptr;
DisplayController* display = nullptr;

// Keypad timing debug (global counter per DEBUG_KEYPAD_TIMING in KeypadController)
volatile uint16_t keypadSlowScans = 0;

// Colon blinking state (500ms on/off for time separator)
unsigned long lastColonBlinkTime = 0;
bool colonBlinkState = true;

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
            
            // Disable boot LEDs after successful init (release pins for display)
            systemInit->disableBootLeds();
            Serial.println(F("✓ Boot LEDs disabled (pins 10,11,12 released for display reuse)\n"));
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
    } else {
        Serial.println(F("✗ Failed to create KeypadController"));
    }
    
    // Initialize display after boot LEDs disabled (riuso pin 10,11,12)
    Serial.println(F("Initializing display (74HC595 + 4× 7-segment)..."));
    Serial.print(F("  74HC595: data=")); Serial.print(DISPLAY_DATA_PIN);
    Serial.print(F(" clock=")); Serial.print(DISPLAY_CLOCK_PIN);
    Serial.print(F(" latch=")); Serial.println(DISPLAY_LATCH_PIN);
    Serial.print(F("  Digit select (COM): "));
    for (uint8_t i = 0; i < 4; i++) {
        Serial.print(DISPLAY_DIGIT_PINS[i]); Serial.print(F(" "));
    }
    Serial.println();
    
    display = new DisplayController(DISPLAY_DATA_PIN, DISPLAY_CLOCK_PIN, DISPLAY_LATCH_PIN,
                                    DISPLAY_DIGIT_PINS[0], DISPLAY_DIGIT_PINS[1],
                                    DISPLAY_DIGIT_PINS[2], DISPLAY_DIGIT_PINS[3]);
    if (display) {
        display->begin();
        Serial.println(F("✓ DisplayController initialized\n"));
        
        // Display test pattern: 12:34
        display->displayTime(12, 34);
        display->setColonBlink(true); // Start with colon visible
        Serial.println(F("✓ Display showing test pattern: 12:34 (colon blinks 500ms)\n"));
        
        Serial.println(F("╔══════════════════════════════════════════════════════╗"));
        Serial.println(F("║         FULL SYSTEM TEST - READY                     ║"));
        Serial.println(F("╠══════════════════════════════════════════════════════╣"));
        Serial.println(F("║  Display: \"12:34\" with blinking colon (500ms)       ║"));
        Serial.println(F("║  Keypad: Press keys (1-9,0,A-D,*,#) for test        ║"));
        Serial.println(F("║  Serial: Each keypress logged as [KEY] X            ║"));
        Serial.println(F("╚══════════════════════════════════════════════════════╝\n"));
    } else {
        Serial.println(F("✗ Failed to create DisplayController"));
    }
}

void loop() {
    // Refresh display (time-division multiplexing - MUST call frequently!)
    if (display) {
        display->refresh();
    }
    
    // Blink colon every 500ms (time separator animation)
    unsigned long currentMillis = millis();
    if (currentMillis - lastColonBlinkTime >= 500) {
        colonBlinkState = !colonBlinkState;
        if (display) {
            display->setColonBlink(colonBlinkState);
        }
        lastColonBlinkTime = currentMillis;
    }
    
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
