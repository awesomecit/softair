#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <SystemInitializer.h>
#include <BuzzerController.h>
#include <KeypadController.h>
#include <DisplayController.h>

// ═══════════════════════════════════════════════════════════════════════════
// TACBOMB COUNTDOWN TIMER - FULL STATE MACHINE
// ═══════════════════════════════════════════════════════════════════════════
// States: SETUP_INIT → SETUP_EDIT → ARMED → COUNTDOWN_RUNNING → DISARM_MODE → EXPLODED
// Controls:
//   - KEY_A: Enter EDIT mode
//   - KEY_2/8: increment/decrement selected digit (in EDIT)
//   - KEY_4/6: move cursor left/right (wrap around)
//   - KEY_C: Confirm timer → ARMED state (in EDIT), Confirm disarm code (in DISARM)
//   - KEY_D: Cancel (return to INIT, reset timer in EDIT), Clear disarm code (in DISARM)
//   - KEY_STAR (*): START countdown from ARMED, Cancel disarm (in DISARM)
//   - KEY_HASH (#): Toggle secret code visibility (SETUP_INIT only), Enter DISARM mode (COUNTDOWN)
//   - KEY_0-9: Enter disarm code digits (in DISARM mode)
//   - KEY_B: Battery check (future - not implemented)
// Visual feedback:
//   - Colon: SOLID in SETUP_EDIT/ARMED/DISARM, BLINKING during COUNTDOWN_RUNNING
//   - Status LED: OFF (INIT), ORANGE blink (EDIT/COUNTDOWN), GREEN (ARMED), YELLOW (DISARM), RED blink (EXPLODED)
// ═══════════════════════════════════════════════════════════════════════════

// System configuration
SystemConfig systemConfig = {
    .version = "1.0.0-minimal",
    .deviceName = "Arduino-Boot-Test",
    .networkSSID = "N/A",
    .baudRate = 115200,
    .redLedPin = 10,      // Not used anymore (display D1)
    .orangeLedPin = 11,   // Not used anymore (display D2)
    .greenLedPin = 12,    // Not used anymore (display D3)
    .buzzerPin = 9,
    .blinkIntervalMs = 200
};

// Status LED (NeoPixel WS2812B on pin 8)
const uint8_t STATUS_LED_PIN = 8;
const uint8_t STATUS_LED_COUNT = 1;
Adafruit_NeoPixel statusLed(STATUS_LED_COUNT, STATUS_LED_PIN, NEO_GRB + NEO_KHZ800);

// Status LED colors
const uint32_t COLOR_OFF = statusLed.Color(0, 0, 0);
const uint32_t COLOR_RED = statusLed.Color(255, 0, 0);
const uint32_t COLOR_ORANGE = statusLed.Color(255, 165, 0);
const uint32_t COLOR_YELLOW = statusLed.Color(255, 255, 0);
const uint32_t COLOR_GREEN = statusLed.Color(0, 255, 0);

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

// ═══════════════════════════════════════════════════════════════════════════
// STATE MACHINE & TIMER VARIABLES
// ═══════════════════════════════════════════════════════════════════════════

enum class BombState {
    SETUP_INIT,        // Initial state: display 10:00, wait for KEY_A
    SETUP_EDIT,        // Editing timer: orange blink, D1 blinks (cursor), colon SOLID
    ARMED,             // Timer confirmed: green solid, colon SOLID, wait for KEY_STAR
    COUNTDOWN_RUNNING, // Countdown active: orange blink, colon BLINKS, decrement every 1s
    DISARM_MODE,       // Disarm mode: enter 4-digit code, # to confirm
    EXPLODED           // Timer reached 00:00: red blink, alarm sound
};

BombState currentState = BombState::SETUP_INIT;

// Timer value (minutes:seconds)
uint8_t timerMinutes = 10;
uint8_t timerSeconds = 0;

// Cursor position for editing (0=M1, 1=M0, 2=S1, 3=S0)
uint8_t cursorPosition = 0;

// Disarm mode variables
char disarmCode[5] = "----";  // 4-digit code buffer + null terminator
uint8_t disarmCodePos = 0;    // Current input position (0-3)
char SECRET_CODE[5] = "0000";  // Secret disarm code (4 digits) - generated at boot
bool secretVisible = false;    // Toggle for showing secret in SETUP_INIT

// Test variable: simulate correct/wrong disarm code
// true = correct code (DISARMED_SUCCESS), false = wrong code (DISARMED_FAIL)
bool testDisarmCodeCorrect = true; // Change to false to test fail scenario

// Colon blinking state (500ms on/off for time separator)
unsigned long lastColonBlinkTime = 0;
bool colonBlinkState = true;

// Cursor blinking state (500ms on/off during SETUP_EDIT)
unsigned long lastCursorBlinkTime = 0;
bool cursorBlinkState = true;

// LED blinking state (500ms on/off for orange LED)
unsigned long lastLedBlinkTime = 0;
bool ledBlinkState = false;

// Countdown timer
unsigned long lastCountdownTick = 0;

// Explosion alarm (60 seconds continuous)
unsigned long explosionStartTime = 0;
const unsigned long EXPLOSION_ALARM_DURATION = 60000; // 60 seconds
bool explosionAlarmActive = false;

// Function declarations
void setStatusLedColor(uint32_t color);
void displayCurrentTime();
bool checkDisarmCode();
void handleKeyPress(KeypadKey key);
void incrementDigitAtCursor();
void decrementDigitAtCursor();

// Set Neopixel status LED color
void setStatusLedColor(uint32_t color) {
    statusLed.setPixelColor(0, color);
    statusLed.show();
}

// Generate random 4-digit secret code
void generateSecretCode() {
    randomSeed(analogRead(A5) ^ millis());  // Seed with floating analog pin + time
    
    for (uint8_t i = 0; i < 4; i++) {
        SECRET_CODE[i] = '0' + random(0, 10);  // Random digit 0-9
    }
    SECRET_CODE[4] = '\0';  // Null terminator
    
    Serial.println(F("\n╔══════════════════════════════════════════════════════╗"));
    Serial.print(F("║  SECRET DISARM CODE: "));
    Serial.print(SECRET_CODE[0]);
    Serial.print(SECRET_CODE[1]);
    Serial.print(SECRET_CODE[2]);
    Serial.print(SECRET_CODE[3]);
    Serial.println(F("                            ║"));
    Serial.println(F("║  Press # in SETUP_INIT to toggle visibility         ║"));
    Serial.println(F("╚══════════════════════════════════════════════════════╝\n"));
}

void displayCurrentTime() {
    if (display) {
        display->displayTime(timerMinutes, timerSeconds);
    }
}

// Display custom pattern (e.g., dashes for disarm mode)
void displayCustom(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4) {
    if (display) {
        // Use displayTime with special values for custom patterns
        // 99 will show as dash '-' in digit
        display->displayTime(d1 * 10 + d2, d3 * 10 + d4);
    }
}

// Check disarm code (placeholder - uses test boolean for now)
// In real implementation: compare entered code with secret stored in EEPROM
bool checkDisarmCode() {
    // TODO: Replace with actual secret key verification
    // For now, use test variable to simulate success/fail scenarios
    return testDisarmCodeCorrect;
}

void setup() {
    Serial.begin(systemConfig.baudRate);
    while (!Serial) { delay(10); }
    
    Serial.println(F("\n\n╔══════════════════════════════════════════════════════╗"));
    Serial.println(F("║         TACBOMB - Countdown Timer System             ║"));
    Serial.println(F("╚══════════════════════════════════════════════════════╝\n"));
    
    // Initialize Neopixel status LED
    statusLed.begin();
    Serial.println(F("✓ NeoPixel status LED initialized (pin 8)\n"));
    
    // Boot sequence visual feedback via Neopixel
    Serial.println(F("[NEOPIXEL] Boot sequence: RED → ORANGE → GREEN → OFF"));
    
    setStatusLedColor(COLOR_RED);
    Serial.println(F("  RED (system starting)"));
    delay(500);
    
    setStatusLedColor(COLOR_ORANGE);
    Serial.println(F("  ORANGE (reading config)"));
    delay(800);
    
    setStatusLedColor(COLOR_GREEN);
    Serial.println(F("  GREEN (ready)"));
    delay(500);
    
    setStatusLedColor(COLOR_OFF);
    Serial.println(F("  OFF (boot complete)\n"));
    
    // Generate random secret code after boot sequence
    generateSecretCode();
    
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
            
            // Disable boot LEDs after successful init (release pins 10,11,12 for display)
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
        
        // Display default time: 10:00
        displayCurrentTime();
        display->setColonBlink(true); // Start with colon visible
        Serial.println(F("✓ Display showing default: 10:00\n"));
        
        Serial.println(F("╔══════════════════════════════════════════════════════╗"));
        Serial.println(F("║           TACBOMB COUNTDOWN TIMER                    ║"));
        Serial.println(F("╠══════════════════════════════════════════════════════╣"));
        Serial.println(F("║  STATE: SETUP_INIT - Press A to enter EDIT mode     ║"));
        Serial.println(F("║                                                      ║"));
        Serial.println(F("║  Controls:                                           ║"));
        Serial.println(F("║    A = Next state (INIT→EDIT→CONFIRM→START)         ║"));
        Serial.println(F("║    2/8 = Inc/Dec digit (in EDIT mode)                ║"));
        Serial.println(F("║    4/6 = Move cursor left/right                      ║"));
        Serial.println(F("║    B = Disarm attempt (check code: success/fail)     ║"));
        Serial.println(F("║    C = Force explode (test timer expiry)             ║"));
        Serial.println(F("║                                                      ║"));
        Serial.println(F("║  Test variable: testDisarmCodeCorrect (true/false)   ║"));
        Serial.println(F("╚══════════════════════════════════════════════════════╝\n"));
    } else {
        Serial.println(F("✗ Failed to create DisplayController"));
    }
}

void loop() {
    unsigned long currentMillis = millis();
    
    // Refresh display (time-division multiplexing - MUST call frequently!)
    if (display) {
        display->refresh();
    }
    
    // Blink colon every 500ms (ONLY during COUNTDOWN_RUNNING)
    if (currentMillis - lastColonBlinkTime >= 500) {
        colonBlinkState = !colonBlinkState;
        if (display) {
            // Colon blinks ONLY during countdown, solid otherwise
            if (currentState == BombState::COUNTDOWN_RUNNING) {
                display->setColonBlink(colonBlinkState);
            }
        }
        lastColonBlinkTime = currentMillis;
    }
    
    // Cursor blinking during SETUP_EDIT (500ms on/off)
    if (currentState == BombState::SETUP_EDIT && currentMillis - lastCursorBlinkTime >= 500) {
        cursorBlinkState = !cursorBlinkState;
        lastCursorBlinkTime = currentMillis;
        
        Serial.print(F("[DEBUG CURSOR] Blink toggle: "));
        Serial.print(cursorBlinkState ? F("ON") : F("OFF"));
        Serial.print(F(" | Position: "));
        Serial.print(cursorPosition);
        Serial.print(F(" | Display digit: D"));
        Serial.println(cursorPosition + 1);
        
        // Blank/unblank the digit at cursor position
        if (display) {
            display->setDigitBlanked(cursorPosition, !cursorBlinkState);
        }
    }
    
    // Explosion alarm management (60 seconds continuous)
    if (explosionAlarmActive) {
        if (currentMillis - explosionStartTime >= EXPLOSION_ALARM_DURATION) {
            // Stop alarm after 60 seconds
            noTone(systemConfig.buzzerPin);
            explosionAlarmActive = false;
            Serial.println(F("[ALARM] Alarm stopped after 60 seconds"));
        }
        // Note: tone() continues running until noTone() is called
    }
    
    // LED blinking for SETUP_EDIT, COUNTDOWN_RUNNING, and EXPLODED states
    if (currentMillis - lastLedBlinkTime >= 500) {
        ledBlinkState = !ledBlinkState;
        lastLedBlinkTime = currentMillis;
        
        if (currentState == BombState::SETUP_EDIT) {
            setStatusLedColor(ledBlinkState ? COLOR_ORANGE : COLOR_OFF);
        } else if (currentState == BombState::COUNTDOWN_RUNNING) {
            setStatusLedColor(ledBlinkState ? COLOR_ORANGE : COLOR_OFF);
        } else if (currentState == BombState::EXPLODED) {
            setStatusLedColor(ledBlinkState ? COLOR_RED : COLOR_OFF);
        }
    }
    
    // Countdown tick (1 second decrement) - runs in COUNTDOWN_RUNNING and DISARM_MODE
    if (currentState == BombState::COUNTDOWN_RUNNING || currentState == BombState::DISARM_MODE) {
        if (currentMillis - lastCountdownTick >= 1000) {
            lastCountdownTick = currentMillis;
            
            // Decrement timer
            if (timerSeconds > 0) {
                timerSeconds--;
            } else if (timerMinutes > 0) {
                timerMinutes--;
                timerSeconds = 59;
            } else {
                // Timer expired → EXPLODED
                currentState = BombState::EXPLODED;
                // LED blinking handled in loop(), don't set color here
                
                // Start continuous alarm for 60 seconds
                explosionStartTime = millis();
                explosionAlarmActive = true;
                tone(systemConfig.buzzerPin, 1000); // Start continuous 1kHz alarm
                
                Serial.println(F("\n[EXPLODED] Timer reached 00:00 - BOMB DETONATED!"));
                Serial.println(F("[FAIL] Mission failed - device exploded"));
                Serial.println(F("[ALARM] Continuous alarm for 60 seconds\n"));
                
                // Show 00:00 on display
                display->displayTime(0, 0);
            }
            
            // Only update display if in COUNTDOWN_RUNNING or DISARM_MODE (show countdown time)
            if (currentState == BombState::COUNTDOWN_RUNNING || currentState == BombState::DISARM_MODE) {
                displayCurrentTime();
            }
            
            // Log time remaining during disarm attempt
            if (currentState == BombState::DISARM_MODE) {
                Serial.print(F("[COUNTDOWN] Time remaining: "));
                Serial.print(timerMinutes);
                Serial.print(F(":"));
                if (timerSeconds < 10) Serial.print(F("0"));
                Serial.println(timerSeconds);
            }
        }
    }
    
    if (!keypad) {
        delay(10);
        return;
    }
    
    // Scan keypad for presses
    KeypadKey key = keypad->scan();
    
    if (key != KeypadKey::NONE) {
        handleKeyPress(key);
    }
    
    delay(10);
}

// Handle keypress based on current state
void handleKeyPress(KeypadKey key) {
    Serial.print(F("[KEY] "));
    
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
        default: Serial.print(F("?")); break;
    }
    
    Serial.print(F(" - State: "));
    
    // State-specific key handling
    switch (currentState) {
        case BombState::SETUP_INIT:
            if (key == KeypadKey::KEY_A) {
                currentState = BombState::SETUP_EDIT;
                setStatusLedColor(COLOR_ORANGE); // Start orange blink
                // Colon stays solid during EDIT (not blinking)
                if (display) display->setColonBlink(true); // Always visible
                // Reset cursor blink timer when entering EDIT
                lastCursorBlinkTime = millis();
                cursorBlinkState = true;
                Serial.println(F("SETUP_EDIT"));
                Serial.println(F("[STATE] Entering EDIT mode - use 2/8 to change digit, 4/6 to move cursor, C to confirm"));
                Serial.print(F("[DEBUG STATE] Cursor blink enabled | Initial position: D"));
                Serial.println(cursorPosition + 1);
            } else if (key == KeypadKey::KEY_HASH) {
                // Toggle secret code visibility on display
                secretVisible = !secretVisible;
                
                if (secretVisible) {
                    // Show secret code on display
                    uint8_t d1 = SECRET_CODE[0] - '0';
                    uint8_t d2 = SECRET_CODE[1] - '0';
                    uint8_t d3 = SECRET_CODE[2] - '0';
                    uint8_t d4 = SECRET_CODE[3] - '0';
                    display->displayTime(d1 * 10 + d2, d3 * 10 + d4);
                    
                    Serial.println(F("SETUP_INIT (secret visible)"));
                    Serial.print(F("[SECRET] Displaying code on screen: "));
                    Serial.println(SECRET_CODE);
                } else {
                    // Hide secret, show default time
                    displayCurrentTime();
                    
                    Serial.println(F("SETUP_INIT (secret hidden)"));
                    Serial.println(F("[SECRET] Code hidden - displaying default time"));
                }
            } else {
                Serial.println(F("SETUP_INIT (ignored - press A to enter EDIT mode)"));
            }
            break;
            
        case BombState::SETUP_EDIT:
            if (key == KeypadKey::KEY_C) {
                // Confirm timer → ARMED state
                Serial.println(F("[DEBUG STATE] Exiting EDIT mode - cursor blink disabled"));
                
                // Clear digit blanking
                if (display) {
                    display->setDigitBlanked(cursorPosition, false);
                }
                
                currentState = BombState::ARMED;
                setStatusLedColor(COLOR_GREEN);
                if (display) display->setColonBlink(true); // Keep solid
                if (buzzer) buzzer->playSuccess();
                Serial.println(F("ARMED"));
                Serial.print(F("[STATE] Timer confirmed: "));
                Serial.print(timerMinutes);
                Serial.print(F(":"));
                if (timerSeconds < 10) Serial.print(F("0"));
                Serial.print(timerSeconds);
                Serial.println(F(" - press * to START countdown"));
            } else if (key == KeypadKey::KEY_D) {
                // Cancel → return to INIT, reset timer
                Serial.println(F("[DEBUG STATE] Exiting EDIT mode (cancelled) - cursor blink disabled"));
                
                // Clear digit blanking
                if (display) {
                    display->setDigitBlanked(cursorPosition, false);
                }
                
                currentState = BombState::SETUP_INIT;
                setStatusLedColor(COLOR_OFF);
                timerMinutes = 10;
                timerSeconds = 0;
                cursorPosition = 0;
                displayCurrentTime();
                Serial.println(F("SETUP_INIT"));
                Serial.println(F("[STATE] Cancelled - timer reset to 10:00"));
            } else if (key == KeypadKey::KEY_2) {
                incrementDigitAtCursor();
                displayCurrentTime();
                Serial.println(F("SETUP_EDIT (incremented)"));
            } else if (key == KeypadKey::KEY_8) {
                decrementDigitAtCursor();
                displayCurrentTime();
                Serial.println(F("SETUP_EDIT (decremented)"));
            } else if (key == KeypadKey::KEY_4) {
                // Move cursor left (wrap)
                uint8_t oldPos = cursorPosition;
                
                // Clear blanking on old position BEFORE moving
                if (display) {
                    display->setDigitBlanked(oldPos, false);
                }
                
                if (cursorPosition == 0) cursorPosition = 3;
                else cursorPosition--;
                
                // Reset blink state to start visible on new position
                cursorBlinkState = true;
                lastCursorBlinkTime = millis();
                
                Serial.print(F("SETUP_EDIT (cursor moved: D"));
                Serial.print(oldPos + 1);
                Serial.print(F(" → D"));
                Serial.print(cursorPosition + 1);
                Serial.println(F(")"));
                Serial.print(F("[DEBUG STATE] Selected display changed: D"));
                Serial.println(cursorPosition + 1);
            } else if (key == KeypadKey::KEY_6) {
                // Move cursor right (wrap)
                uint8_t oldPos = cursorPosition;
                
                // Clear blanking on old position BEFORE moving
                if (display) {
                    display->setDigitBlanked(oldPos, false);
                }
                
                if (cursorPosition == 3) cursorPosition = 0;
                else cursorPosition++;
                
                // Reset blink state to start visible on new position
                cursorBlinkState = true;
                lastCursorBlinkTime = millis();
                
                Serial.print(F("SETUP_EDIT (cursor moved: D"));
                Serial.print(oldPos + 1);
                Serial.print(F(" → D"));
                Serial.print(cursorPosition + 1);
                Serial.println(F(")"));
                Serial.print(F("[DEBUG STATE] Selected display changed: D"));
                Serial.println(cursorPosition + 1);
            } else {
                Serial.println(F("SETUP_EDIT (ignored)"));
            }
            break;
            
        case BombState::ARMED:
            if (key == KeypadKey::KEY_STAR) {
                // START countdown
                currentState = BombState::COUNTDOWN_RUNNING;
                setStatusLedColor(COLOR_ORANGE); // Start orange blink
                // Enable colon blinking during countdown
                if (display) display->setColonBlink(colonBlinkState);
                lastCountdownTick = millis();
                if (buzzer) buzzer->playStartup();
                Serial.println(F("COUNTDOWN_RUNNING"));
                Serial.println(F("[START] Countdown initiated!"));
            } else if (key == KeypadKey::KEY_D) {
                // Cancel → return to EDIT
                currentState = BombState::SETUP_EDIT;
                setStatusLedColor(COLOR_ORANGE);
                Serial.println(F("SETUP_EDIT"));
                Serial.println(F("[STATE] Returned to EDIT mode"));
            } else {
                Serial.println(F("ARMED (ignored - press * to START, D to cancel)"));
            }
            break;
            
        case BombState::COUNTDOWN_RUNNING:
            if (key == KeypadKey::KEY_HASH) {
                // Enter DISARM mode
                currentState = BombState::DISARM_MODE;
                setStatusLedColor(COLOR_YELLOW);
                
                // Reset disarm code buffer
                for (uint8_t i = 0; i < 4; i++) {
                    disarmCode[i] = '-';
                }
                disarmCode[4] = '\0';
                disarmCodePos = 0;
                
                // Stop colon blinking, show solid
                if (display) display->setColonBlink(true);
                
                // Keep displaying countdown time (no change to display)
                
                if (buzzer) buzzer->playSuccess();  // Audio feedback
                
                Serial.println(F("DISARM_MODE"));
                Serial.println(F("[DISARM] Entered disarm mode - enter 4-digit code, press C to confirm"));
                Serial.println(F("[DISARM] Current input: ----"));
                Serial.println(F("[DISARM] Display continues showing countdown time"));
            } else {
                Serial.println(F("COUNTDOWN_RUNNING (press # to enter DISARM mode)"));
            }
            break;
            
        case BombState::DISARM_MODE:
            Serial.print(F("[DEBUG] Key pressed: "));
            Serial.print(static_cast<uint8_t>(key));
            Serial.print(F(" | Checking if numeric... "));
            
            // Accept numeric keys (0-9) - check each individually since KEY_0 is not sequential
            if (key == KeypadKey::KEY_0 || key == KeypadKey::KEY_1 || key == KeypadKey::KEY_2 || 
                key == KeypadKey::KEY_3 || key == KeypadKey::KEY_4 || key == KeypadKey::KEY_5 || 
                key == KeypadKey::KEY_6 || key == KeypadKey::KEY_7 || key == KeypadKey::KEY_8 || 
                key == KeypadKey::KEY_9) {
                
                Serial.println(F("YES - numeric key detected!"));
                
                if (disarmCodePos < 4) {
                    // Convert key enum to digit character
                    char digit;
                    if (key == KeypadKey::KEY_0) digit = '0';
                    else if (key == KeypadKey::KEY_1) digit = '1';
                    else if (key == KeypadKey::KEY_2) digit = '2';
                    else if (key == KeypadKey::KEY_3) digit = '3';
                    else if (key == KeypadKey::KEY_4) digit = '4';
                    else if (key == KeypadKey::KEY_5) digit = '5';
                    else if (key == KeypadKey::KEY_6) digit = '6';
                    else if (key == KeypadKey::KEY_7) digit = '7';
                    else if (key == KeypadKey::KEY_8) digit = '8';
                    else digit = '9';
                    
                    disarmCode[disarmCodePos] = digit;
                    disarmCodePos++;
                    
                    Serial.print(F("DISARM_MODE (digit "));
                    Serial.print(digit);
                    Serial.println(F(" captured)"));
                    
                    // Audio feedback for each digit
                    if (buzzer) buzzer->playStartup();
                    
                    // Display continues showing countdown time (no update needed)
                    
                    Serial.print(F("[DISARM] Digit entered: "));
                    Serial.print(digit);
                    Serial.print(F(" | Current input: "));
                    Serial.print(disarmCode);
                    Serial.print(F(" ("));
                    Serial.print(disarmCodePos);
                    Serial.println(F("/4)"));
                } else {
                    Serial.println(F("DISARM_MODE (4 digits complete)"));
                    Serial.println(F("[DISARM] Code already complete (4 digits) - press C to confirm, D to clear"));
                }
            } else if (key == KeypadKey::KEY_C) {
                Serial.println(F("NO - KEY_C detected"));
                // Confirm code (changed from KEY_HASH)
                if (disarmCodePos == 4) {
                    Serial.println(F("DISARM_MODE (confirming code)"));
                    Serial.print(F("[DISARM] Code submitted: "));
                    Serial.println(disarmCode);
                    Serial.print(F("[DISARM] Secret code: "));
                    Serial.println(SECRET_CODE);
                    
                    // Check if code matches
                    bool codeCorrect = true;
                    for (uint8_t i = 0; i < 4; i++) {
                        if (disarmCode[i] != SECRET_CODE[i]) {
                            codeCorrect = false;
                            break;
                        }
                    }
                    
                    if (codeCorrect) {
                        // SUCCESS - bomb disarmed
                        setStatusLedColor(COLOR_GREEN);
                        if (buzzer) buzzer->playSuccess();
                        
                        Serial.println(F("[SUCCESS] ✓ Correct code - BOMB DISARMED!"));
                        Serial.println(F("[SUCCESS] Mission accomplished - device neutralized\n"));
                        
                        // Show success on display (88:88)
                        display->displayTime(88, 88);
                        
                        // Terminal state - reset device to continue
                        currentState = BombState::SETUP_INIT;
                        delay(3000);
                        
                        // Reset to initial state
                        timerMinutes = 10;
                        timerSeconds = 0;
                        displayCurrentTime();
                        setStatusLedColor(COLOR_OFF);
                    } else {
                        // FAIL - wrong code, bomb explodes
                        currentState = BombState::EXPLODED;
                        
                        // Start continuous alarm for 60 seconds
                        explosionStartTime = millis();
                        explosionAlarmActive = true;
                        tone(systemConfig.buzzerPin, 1000); // Start continuous 1kHz alarm
                        
                        Serial.println(F("[FAIL] ✗ Wrong code - BOMB EXPLODED!"));
                        Serial.println(F("[FAIL] Mission failed - incorrect disarm code"));
                        Serial.println(F("[ALARM] Continuous alarm for 60 seconds\n"));
                        
                        // Show 00:00
                        display->displayTime(0, 0);
                    }
                } else {
                    Serial.println(F("DISARM_MODE (code incomplete)"));
                    Serial.print(F("[DISARM] Code incomplete ("));
                    Serial.print(disarmCodePos);
                    Serial.println(F("/4 digits) - enter remaining digits"));
                }
            } else if (key == KeypadKey::KEY_D) {
                Serial.println(F("NO - KEY_D detected"));
                // Clear code and restart (changed from KEY_STAR)
                Serial.println(F("DISARM_MODE (clearing code)"));
                
                // Reset disarm code buffer
                for (uint8_t i = 0; i < 4; i++) {
                    disarmCode[i] = '-';
                }
                disarmCode[4] = '\0';
                disarmCodePos = 0;
                
                // Display continues showing countdown time (no change needed)
                
                Serial.println(F("[DISARM] Code cleared - enter new 4-digit code"));
                Serial.println(F("[DISARM] Current input: ----"));
            } else if (key == KeypadKey::KEY_STAR) {
                Serial.println(F("NO - KEY_STAR detected"));
                // Cancel disarm, return to countdown
                Serial.println(F("DISARM_MODE (cancelled)"));
                currentState = BombState::COUNTDOWN_RUNNING;
                setStatusLedColor(COLOR_ORANGE);
                if (display) display->setColonBlink(colonBlinkState);
                displayCurrentTime();
                
                Serial.println(F("[DISARM] Cancelled - returned to countdown"));
            } else {
                Serial.println(F("NO - unrecognized key"));
                Serial.println(F("DISARM_MODE (0-9 to enter code, C to confirm, D to clear, * to cancel)"));
            }
            break;
            
        case BombState::EXPLODED:
            Serial.println(F("EXPLODED (terminal state - reset device)"));
            break;
    }
}

void incrementDigitAtCursor() {
    // cursorPosition: 0=M1 (tens min), 1=M0 (units min), 2=S1 (tens sec), 3=S0 (units sec)
    switch (cursorPosition) {
        case 0: // Minutes tens (0-9, but max 99 minutes → max 9 for tens)
            timerMinutes = (timerMinutes + 10) % 100;
            if (timerMinutes > 99) timerMinutes = 90;
            break;
        case 1: // Minutes units (0-9)
            {
                uint8_t tens = (timerMinutes / 10) * 10;
                uint8_t units = (timerMinutes % 10 + 1) % 10;
                timerMinutes = tens + units;
            }
            break;
        case 2: // Seconds tens (0-5)
            {
                uint8_t tens = ((timerSeconds / 10) + 1) % 6;
                uint8_t units = timerSeconds % 10;
                timerSeconds = tens * 10 + units;
            }
            break;
        case 3: // Seconds units (0-9)
            {
                uint8_t tens = (timerSeconds / 10) * 10;
                uint8_t units = (timerSeconds % 10 + 1) % 10;
                timerSeconds = tens + units;
            }
            break;
    }
    displayCurrentTime();
}

void decrementDigitAtCursor() {
    switch (cursorPosition) {
        case 0: // Minutes tens
            if (timerMinutes < 10) {
                timerMinutes = 90;
            } else {
                timerMinutes = ((timerMinutes / 10 - 1) * 10) + (timerMinutes % 10);
            }
            break;
        case 1: // Minutes units
            {
                uint8_t tens = (timerMinutes / 10) * 10;
                uint8_t units = timerMinutes % 10;
                if (units == 0) units = 9; else units--;
                timerMinutes = tens + units;
            }
            break;
        case 2: // Seconds tens (0-5)
            {
                uint8_t tens = timerSeconds / 10;
                uint8_t units = timerSeconds % 10;
                if (tens == 0) tens = 5; else tens--;
                timerSeconds = tens * 10 + units;
            }
            break;
        case 3: // Seconds units
            {
                uint8_t tens = (timerSeconds / 10) * 10;
                uint8_t units = timerSeconds % 10;
                if (units == 0) units = 9; else units--;
                timerSeconds = tens + units;
            }
            break;
    }
    displayCurrentTime();
}
