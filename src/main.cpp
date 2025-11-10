#include <Arduino.h>
#include <SystemInitializer.h>
#include <ConfigLoader.h>
#include <BuzzerController.h>

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

// ═══════════════════════════════════════════════════════════════════════════
// GLOBAL STATE
// ═══════════════════════════════════════════════════════════════════════════

SystemInitializer* systemInit = nullptr;
BuzzerController* buzzer = nullptr;
SystemConfig systemConfig;

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
    
    // ─────────────────────────────────────────────────────────────────────
    // PHASE 3: System Initialization with Boot Sequence
    // ─────────────────────────────────────────────────────────────────────
    Serial.println(F("\nPhase 3: Boot sequence..."));
    
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
    
    // Attach interrupt to button pin (trigger on both RISING and FALLING edges)
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, CHANGE);
    Serial.println(F("OK: Interrupt attached"));
    
    // ─────────────────────────────────────────────────────────────────────
    // PHASE 5: System Ready
    // ─────────────────────────────────────────────────────────────────────
    Serial.println(F("\n=== SYSTEM READY ==="));
    
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
    // Check if button interrupt flag was set
    if (buttonPressed) {
        buttonPressed = false;  // Clear the flag
        
        // Handle button with debouncing
        if (handleButton()) {
            toggleLed();
        }
    }
    
    // Handle serial commands
    handleSerialCommands();
    
    // Periodic statistics display
    if (millis() - lastStatsDisplay > STATS_INTERVAL) {
        #ifdef DEBUG
        displayStats();
        #endif
        lastStatsDisplay = millis();
    }
}
