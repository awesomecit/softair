#include <Arduino.h>
#include <SystemInitializer.h>
#include <ConfigLoader.h>
#include <BuzzerController.h>
#include <DisplayController.h>
#include <KeypadController.h>
#include <CountdownController.h>

// ═══════════════════════════════════════════════════════════════════════════
// CONFIGURATION
// ═══════════════════════════════════════════════════════════════════════════

// JSON configuration embedded in program memory
const char* SYSTEM_CONFIG = R"({
    "version": "1.0.0",
    "deviceName": "Arduino-Softair",
    "networkSSID": "HomeNetwork",
    "baudRate": 115200,
    "leds": {
        "red": 10,
        "orange": 11,
        "green": 12
    },
    "buzzer": 9,
    "blinkIntervalMs": 200
})";

// Hardware pin assignments
const uint8_t BUTTON_PIN = 2;           // Digital pin 2 (with interrupt support)
const uint8_t LED_PIN = 13;             // Built-in LED on Arduino Uno

// Button debouncing configuration
const unsigned long DEBOUNCE_DELAY = 50; // 50ms debounce time

// Battery monitor configuration
const uint8_t BATTERY_PIN = A0;         // Analog pin for battery voltage
const uint8_t LED_FULL = 2;             // Green LED - FULL state
const uint8_t LED_GOOD = 3;             // Yellow LED - GOOD state  
const uint8_t LED_LOW = 4;              // Orange LED - LOW state
const uint8_t LED_CRITICAL = 5;         // Red LED - CRITICAL state

// Voltage thresholds (direct from potentiometer, 0-5V range)
// Potentiometer simulates battery voltage directly (no voltage divider in Wokwi)
// Adjusted thresholds to cover full POT range for easier testing
const float VREF = 5.0f;                // ADC reference voltage
const float V_FULL = 3.75f;             // FULL state threshold (75% of 5V)
const float V_GOOD = 2.50f;             // GOOD state threshold (50% of 5V)
const float V_LOW = 1.25f;              // LOW state threshold (25% of 5V)
// Below V_LOW is CRITICAL (0-25%)

const unsigned long BATTERY_UPDATE_INTERVAL = 2000; // Check every 2 seconds

// Display controller configuration
const uint8_t DISPLAY_DATA_PIN = 6;     // 74HC595 DS (Serial Data)
const uint8_t DISPLAY_CLOCK_PIN = 7;    // 74HC595 SHCP (Shift Register Clock)
const uint8_t DISPLAY_LATCH_PIN = 8;    // 74HC595 STCP (Storage Register Clock / Latch)
const uint8_t DISPLAY_DIGIT1_PIN = A1;  // Digit 1 COM (cathode control)
const uint8_t DISPLAY_DIGIT2_PIN = A2;  // Digit 2 COM (cathode control)
const uint8_t DISPLAY_DIGIT3_PIN = A3;  // Digit 3 COM (cathode control)
const uint8_t DISPLAY_DIGIT4_PIN = A4;  // Digit 4 COM (cathode control)

// Keypad controller configuration (4×4 matrix)
// Rows: R1-R4 connect to digital pins 13, 12, 11, 10
// Cols: C1-C4 connect to A5, 9, 8, 7
const uint8_t KEYPAD_ROW_PINS[4] = {13, 12, 11, 10};  // R1, R2, R3, R4
const uint8_t KEYPAD_COL_PINS[4] = {A5, 9, 8, 7};     // C1, C2, C3, C4

// Display modes
enum class DisplayMode : uint8_t {
    CLOCK,           // Orologio con tempo corrente
    COUNTDOWN_EDIT   // Editing countdown timer
};

// ═══════════════════════════════════════════════════════════════════════════
// GLOBAL STATE
// ═══════════════════════════════════════════════════════════════════════════

SystemInitializer* systemInit = nullptr;
BuzzerController* buzzer = nullptr;
DisplayController* display = nullptr;
KeypadController* keypad = nullptr;
CountdownController* countdown = nullptr;
SystemConfig systemConfig;

// Display mode state
DisplayMode currentMode = DisplayMode::CLOCK;
bool modeSelectionActive = false;  // DISABLED per test - abilita dopo fix multiplexing
unsigned long modeSelectionStart = 0;
const unsigned long MODE_SELECTION_TIMEOUT = 5000;

// Button state management
volatile bool buttonPressed = false;    // Flag set by interrupt
bool ledState = LOW;                    // Current LED state
unsigned long lastDebounceTime = 0;     // Last time button pin changed
bool lastButtonState = HIGH;            // Previous stable button state (HIGH = not pressed with INPUT_PULLUP)
bool buttonReading = HIGH;              // Current button reading

// Statistics
unsigned long buttonPressCount = 0;
unsigned long lastStatsDisplay = 0;
const unsigned long STATS_INTERVAL = 10000; // Show stats every 10 seconds

// Battery monitor state
unsigned long lastBatteryUpdate = 0;
float currentVoltage = 0.0f;

// Display time tracking
unsigned long lastTimeUpdate = 0;
const unsigned long TIME_UPDATE_INTERVAL = 1000;  // Update display every 1 second
uint8_t currentHours = 12;
uint8_t currentMinutes = 34;
bool colonBlinkState = true;  // Track colon blink state
unsigned long lastColonBlink = 0;
const unsigned long COLON_BLINK_INTERVAL = 500;  // Blink every 500ms

// Countdown editing state
bool cursorBlinkState = true;  // Track cursor blink (show/hide digit)
unsigned long lastCursorBlink = 0;
const unsigned long CURSOR_BLINK_INTERVAL = 500;  // Blink every 500ms

// ═══════════════════════════════════════════════════════════════════════════
// FORWARD DECLARATIONS
// ═══════════════════════════════════════════════════════════════════════════

int freeRam();
void toggleLed();
bool handleButton();
void displayStats();
void displayUsage();
void handleSerialCommands();
void buttonISR();

// Battery monitor functions
float readBatteryVoltage();
void updateBatteryLeds(float voltage);

// Keypad & countdown functions
void handleKeyPress(KeypadKey key);
void updateCountdownDisplay();

// ═══════════════════════════════════════════════════════════════════════════
// UTILITY FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief Calculate free RAM (for debugging)
 * @return Free RAM in bytes
 */
int freeRam() {
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

// ═══════════════════════════════════════════════════════════════════════════
// INTERRUPT SERVICE ROUTINE
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief Button interrupt handler
 * Sets a flag when button state changes (to be debounced in main loop)
 */
void buttonISR() {
    buttonPressed = true;
}

// ═══════════════════════════════════════════════════════════════════════════
// BATTERY MONITOR FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief Read battery voltage from ADC (A0)
 * @return Voltage at A0 pin (after voltage divider)
 */
float readBatteryVoltage() {
    const uint16_t adcValue = analogRead(BATTERY_PIN);
    // Convert ADC to voltage: (ADC / 1023) × VREF
    return (adcValue / 1023.0f) * VREF;
}

/**
 * @brief Update battery LEDs based on voltage
 * @param voltage Voltage at A0 (after voltage divider)
 */
void updateBatteryLeds(float voltage) {
    // Turn off all battery LEDs first
    digitalWrite(LED_FULL, LOW);
    digitalWrite(LED_GOOD, LOW);
    digitalWrite(LED_LOW, LOW);
    digitalWrite(LED_CRITICAL, LOW);
    
    // Turn on appropriate LED based on voltage threshold
    if (voltage >= V_FULL) {
        digitalWrite(LED_FULL, HIGH);       // Green - FULL
    } else if (voltage >= V_GOOD) {
        digitalWrite(LED_GOOD, HIGH);       // Yellow - GOOD
    } else if (voltage >= V_LOW) {
        digitalWrite(LED_LOW, HIGH);        // Orange - LOW
    } else {
        digitalWrite(LED_CRITICAL, HIGH);   // Red - CRITICAL
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// KEYPAD & COUNTDOWN HANDLING
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief Handle keypad key press (da EPIC_03)
 * 
 * Mapping funzionale:
 *   KEY_A → MODE (toggle CLOCK ↔ COUNTDOWN_EDIT)
 *   KEY_4 → LEFT (move cursor left con wrap)
 *   KEY_6 → RIGHT (move cursor right con wrap)
 *   KEY_2 → UP (increment digit at cursor)
 *   KEY_8 → DOWN (decrement digit at cursor)
 * 
 * Serial logging format: [MODE], [CURSOR], [VALUE]
 * 
 * @param key Chiave premuta
 */
void handleKeyPress(KeypadKey key) {
    // Debug: print chiave premuta
    Serial.print(F("[KEY] "));
    Serial.println(KeypadController::keyToChar(key));
    
    // ────────────────────────────────────────────────────────────────────────
    // MODE SELECTION (solo al boot)
    // ────────────────────────────────────────────────────────────────────────
    if (modeSelectionActive) {
        if (key == KeypadKey::KEY_A) {
            // Utente ha scelto COUNTDOWN_EDIT
            modeSelectionActive = false;
            currentMode = DisplayMode::COUNTDOWN_EDIT;
            Serial.println(F("[MODE] COUNTDOWN_EDIT selected"));
            
            // Mostra countdown 10:00 con colon fisso ON
            updateCountdownDisplay();
            if (display) {
                display->setColonBlink(false);  // Colon sempre ON
            }
            
            // Beep conferma
            if (buzzer) {
                buzzer->playSuccess();
            }
        }
        // Altri tasti ignorati durante mode selection
        return;
    }
    
    // ────────────────────────────────────────────────────────────────────────
    // RUNTIME MODE TOGGLE (dopo boot)
    // ────────────────────────────────────────────────────────────────────────
    if (key == KeypadKey::KEY_A) {
        // Toggle display mode
        if (currentMode == DisplayMode::CLOCK) {
            currentMode = DisplayMode::COUNTDOWN_EDIT;
            Serial.println(F("[MODE] COUNTDOWN_EDIT"));
            
            // Aggiorna display con valore countdown
            updateCountdownDisplay();
            if (display) {
                display->setColonBlink(false);  // Colon sempre ON
            }
        } else {
            currentMode = DisplayMode::CLOCK;
            Serial.println(F("[MODE] CLOCK"));
            
            // Torna a mostrare orologio
            if (display) {
                display->displayTime(currentHours, currentMinutes);
                display->setColonBlink(true);  // Riabilita blink colon
            }
        }
        return;
    }
    
    // ────────────────────────────────────────────────────────────────────────
    // COUNTDOWN EDITING (solo in COUNTDOWN_EDIT mode)
    // ────────────────────────────────────────────────────────────────────────
    // NOTA: Per ora disabilitato - mostra solo valore statico 10:00
    // TODO (future): Abilitare cursor navigation + increment/decrement
    // quando DisplayController supporterà blanking digit singolo per cursor blink
}

/**
/**
 * @brief Aggiorna display con countdown value (statico)
 * 
 * Mostra countdown time in formato HH:MM.
 * In questa versione: solo display statico, no cursor blink.
 */
void updateCountdownDisplay() {
    if (!display || !countdown) return;
    
    uint8_t h, m;
    countdown->getTime(h, m);
    display->displayTime(h, m);
    // Colon sempre ON in COUNTDOWN mode (no blink)
    display->setColonBlink(false);
}

// ═══════════════════════════════════════════════════════════════════════════
// BUTTON HANDLING WITH DEBOUNCING
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief Handle button press with software debouncing
 * @return true if button was pressed (debounced), false otherwise
 */
bool handleButton() {
    // Read current button state
    buttonReading = digitalRead(BUTTON_PIN);
    
    // Check if state changed (potential press/release)
    if (buttonReading != lastButtonState) {
        lastDebounceTime = millis();
    }
    
    // If enough time has passed, consider it a valid state change
    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
        // If button state actually changed
        if (buttonReading == LOW && lastButtonState == HIGH) {
            // Button was just pressed (LOW = pressed with INPUT_PULLUP)
            lastButtonState = buttonReading;
            return true;
        }
        lastButtonState = buttonReading;
    }
    
    return false;
}

/**
 * @brief Toggle LED state and update output
 */
void toggleLed() {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    
    buttonPressCount++;
    
    Serial.print("Button pressed! LED is now ");
    Serial.println(ledState ? "ON" : "OFF");
    
    #ifdef DEBUG
    Serial.print("  → Total presses: ");
    Serial.println(buttonPressCount);
    Serial.print("  → Free RAM: ");
    Serial.print(freeRam());
    Serial.println(" bytes");
    #endif
}

// ═══════════════════════════════════════════════════════════════════════════
// DISPLAY FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief Display runtime statistics
 */
void displayStats() {
    Serial.println("\n╔════════════════════════════════════════════════════════╗");
    Serial.println("║               SYSTEM STATISTICS                        ║");
    Serial.println("╠════════════════════════════════════════════════════════╣");
    
    Serial.print("║ Uptime:           ");
    unsigned long seconds = millis() / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    Serial.print(hours);
    Serial.print("h ");
    Serial.print(minutes % 60);
    Serial.print("m ");
    Serial.print(seconds % 60);
    Serial.println("s");
    
    Serial.print("║ Button presses:   ");
    Serial.println(buttonPressCount);
    
    Serial.print("║ LED state:        ");
    Serial.println(ledState ? "ON" : "OFF");
    
    Serial.print("║ Free RAM:         ");
    Serial.print(freeRam());
    Serial.println(" bytes");
    
    if (systemInit) {
        Serial.print("║ Init state:       ");
        switch(systemInit->getState()) {
            case InitState::READY:
                Serial.println("READY");
                break;
            case InitState::ERROR:
                Serial.println("ERROR");
                break;
            default:
                Serial.println("UNKNOWN");
        }
    }
    
    Serial.println("╚════════════════════════════════════════════════════════╝\n");
}

/**
 * @brief Display usage instructions
 */
void displayUsage() {
    Serial.println(F("\n=== USAGE ==="));
    Serial.println(F("Button: Toggle LED"));
    Serial.println(F("Serial 's': Stats"));
    Serial.println(F("Serial 'i': Info"));
    Serial.println(F("Serial 'r': Reset"));
    Serial.println(F("Serial 'h': Help\n"));
}

// ═══════════════════════════════════════════════════════════════════════════
// SERIAL COMMAND HANDLING
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief Handle serial commands
 */
void handleSerialCommands() {
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        
        switch(cmd) {
            case 's':
            case 'S':
                displayStats();
                break;
                
            case 'i':
            case 'I':
                if (systemInit) {
                    Serial.println();
                    systemInit->printSystemInfo();
                }
                break;
                
            case 'r':
            case 'R':
                buttonPressCount = 0;
                Serial.println("✓ Statistics reset");
                break;
                
            case 'h':
            case 'H':
            case '?':
                displayUsage();
                break;
                
            case 't':
            case 'T':
                // Manual LED toggle for testing
                toggleLed();
                Serial.println("  (Manual toggle via serial)");
                break;
                
            default:
                // Ignore other characters (newline, etc.)
                break;
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// ARDUINO SETUP
// ═══════════════════════════════════════════════════════════════════════════

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    while (!Serial && millis() < 3000) {
        ; // Wait for serial port to connect (max 3 seconds)
    }
    
    Serial.println(F("\n\n"));
    Serial.println(F("====== ARDUINO BOOT ======"));
    Serial.println(F("LED & BUTTON DEMO"));
    Serial.println(F("=========================="));
    Serial.println();
    
    // ─────────────────────────────────────────────────────────────────────
    // PHASE 1: Load Configuration
    // ─────────────────────────────────────────────────────────────────────
    Serial.println(F("Phase 1: Loading config..."));
    
    // Use default config directly (JSON parsing disabled for RAM optimization)
    systemConfig = ConfigLoader::getDefaultConfig();
    Serial.println(F("OK: Using defaults"));
    
    // To enable JSON parsing (requires more RAM):
    // if (!ConfigLoader::loadFromJson(SYSTEM_CONFIG, systemConfig)) {
    //     Serial.println(F("WARN: Using defaults"));
    //     systemConfig = ConfigLoader::getDefaultConfig();
    // } else {
    //     Serial.println(F("OK: Config loaded"));
    // }
    
    // ─────────────────────────────────────────────────────────────────────
    // PHASE 2: Initialize Components
    // ─────────────────────────────────────────────────────────────────────
    Serial.println(F("\nPhase 2: Init components..."));
    
    // Initialize buzzer
    buzzer = new BuzzerController(systemConfig.buzzerPin);
    buzzer->begin();
    
    // Initialize display controller
    display = new DisplayController(DISPLAY_DATA_PIN, DISPLAY_CLOCK_PIN, DISPLAY_LATCH_PIN,
                                   DISPLAY_DIGIT1_PIN, DISPLAY_DIGIT2_PIN, 
                                   DISPLAY_DIGIT3_PIN, DISPLAY_DIGIT4_PIN);
    display->begin();
    
    // Initialize keypad controller
    keypad = new KeypadController(KEYPAD_ROW_PINS, KEYPAD_COL_PINS);
    keypad->begin();
    Serial.println(F("OK: Keypad initialized"));
    
    // Initialize countdown controller (default 10:00)
    countdown = new CountdownController();
    Serial.println(F("OK: Countdown initialized (10:00)"));
    
    // ─────────────────────────────────────────────────────────────────────
    // PHASE 3: System Initialization with Boot Sequence
    // ─────────────────────────────────────────────────────────────────────
    Serial.println(F("\nPhase 3: Boot sequence..."));
    
    // Run display diagnostic test
    Serial.println(F("[PHASE 3] Display diagnostic test..."));
    if (display && display->runDiagnosticTest()) {
        Serial.println(F("OK: Display test passed"));
    } else {
        Serial.println(F("ERROR: Display test failed!"));
    }
    
    systemInit = new SystemInitializer(systemConfig, buzzer);
    
    if (systemInit->initialize()) {
        Serial.println(F("OK: Boot complete"));
    } else {
        Serial.println(F("ERROR: Boot failed!"));
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // PHASE 4: Setup Hardware (Button & LED)
    // ─────────────────────────────────────────────────────────────────────
    Serial.println(F("\nPhase 4: Hardware setup..."));
    
    // Configure button pin with internal pull-up resistor
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    Serial.print(F("OK: Button pin "));
    Serial.println(BUTTON_PIN);
    
    // Configure LED pin
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, ledState);
    Serial.print(F("OK: LED pin "));
    Serial.println(LED_PIN);
    
    // Configure battery monitor LED pins
    pinMode(LED_FULL, OUTPUT);
    pinMode(LED_GOOD, OUTPUT);
    pinMode(LED_LOW, OUTPUT);
    pinMode(LED_CRITICAL, OUTPUT);
    Serial.println(F("OK: Battery LEDs initialized"));
    
    // Attach interrupt to button pin (trigger on both RISING and FALLING edges)
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, CHANGE);
    Serial.println(F("OK: Interrupt attached"));
    
    // ─────────────────────────────────────────────────────────────────────
    // PHASE 5: Display Initial Value
    // ─────────────────────────────────────────────────────────────────────
    Serial.println(F("\nPhase 5: Setting initial display..."));
    if (display) {
        display->displayTime(currentHours, currentMinutes);  // Mostra 12:34
        Serial.println(F("OK: Display set to 12:34"));
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // PHASE 6: System Ready
    // ─────────────────────────────────────────────────────────────────────
    Serial.println(F("\n=== SYSTEM READY ==="));
    
    // Disable boot LEDs to release pins (conflict with keypad rows 10,11,12)
    if (systemInit) {
        systemInit->disableBootLeds();
    }
    
    // Show system info
    if (systemInit) {
        systemInit->printSystemInfo();
    }
    
    // Show usage instructions
    displayUsage();
    
    Serial.println("Monitoring button presses...\n");
}

// ═══════════════════════════════════════════════════════════════════════════
// ARDUINO MAIN LOOP
// ═══════════════════════════════════════════════════════════════════════════

void loop() {
    // ══════════════════════════════════════════════════════════════════════
    // STEP 1: MULTIPLEXING REFRESH (PRIORITÀ ASSOLUTA - mai saltare!)
    // ══════════════════════════════════════════════════════════════════════
    if (display) {
        display->refresh();  // Chiamato OGNI loop - essenziale per multiplexing
    }
    
    // ══════════════════════════════════════════════════════════════════════
    // STEP 2: INPUT HANDLING
    // ══════════════════════════════════════════════════════════════════════
    
    // Button interrupt handling
    if (buttonPressed) {
        buttonPressed = false;
        if (handleButton()) {
            toggleLed();
        }
    }
    
    // Serial commands
    handleSerialCommands();
    
    // ══════════════════════════════════════════════════════════════════════
    // STEP 3: DISPLAY UPDATES (solo quando contenuto CAMBIA)
    // ══════════════════════════════════════════════════════════════════════
    
    // Colon blink (500ms) - solo toggle bit, no refresh
    if (millis() - lastColonBlink >= COLON_BLINK_INTERVAL) {
        colonBlinkState = !colonBlinkState;
        if (display) {
            display->setColonBlink(colonBlinkState);
        }
        lastColonBlink = millis();
    }
    
    // Time update (1 secondo) - chiama displayTime() solo quando cambia
    if (millis() - lastTimeUpdate >= TIME_UPDATE_INTERVAL) {
        currentMinutes++;
        if (currentMinutes >= 60) {
            currentMinutes = 0;
            currentHours++;
            if (currentHours >= 24) {
                currentHours = 0;
            }
        }
        
        // Update display SOLO quando tempo cambia
        if (display) {
            display->displayTime(currentHours, currentMinutes);
        }
        
        Serial.print(F("Time: "));
        if (currentHours < 10) Serial.print('0');
        Serial.print(currentHours);
        Serial.print(':');
        if (currentMinutes < 10) Serial.print('0');
        Serial.println(currentMinutes);
        
        lastTimeUpdate = millis();
    }
    
    // ══════════════════════════════════════════════════════════════════════
    // STEP 4: BACKGROUND TASKS (battery, stats)
    // ══════════════════════════════════════════════════════════════════════
    if (millis() - lastBatteryUpdate >= BATTERY_UPDATE_INTERVAL) {
        currentVoltage = readBatteryVoltage();
        updateBatteryLeds(currentVoltage);
        lastBatteryUpdate = millis();
        
        #ifdef DEBUG
        Serial.print(F("Battery: "));
        Serial.print(currentVoltage, 2);
        Serial.println(F("V"));
        #endif
    }
    
    // Periodic statistics display
    if (millis() - lastStatsDisplay > STATS_INTERVAL) {
        #ifdef DEBUG
        displayStats();
        #endif
        lastStatsDisplay = millis();
    }
}
