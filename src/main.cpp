#include <Arduino.h>
#include <SystemInitializer.h>
#include <BuzzerController.h>

// ═══════════════════════════════════════════════════════════════════════════
// MINIMAL TEST - BOOT SEQUENCE + BATTERY MONITOR
// ═══════════════════════════════════════════════════════════════════════════
// Test: SystemInitializer + BuzzerController + Battery Monitor
// Valida: boot LEDs (red→orange→green) + buzzer + battery status LEDs
// ═══════════════════════════════════════════════════════════════════════════

// Battery monitor configuration
const uint8_t BATTERY_PIN = A0;
const uint8_t LED_FULL = 2;
const uint8_t LED_GOOD = 3;
const uint8_t LED_LOW = 4;
const uint8_t LED_CRITICAL = 5;

// Voltage thresholds (0-5V range from potentiometer)
const float VREF = 5.0f;
const float V_FULL = 3.75f;     // 75% of 5V
const float V_GOOD = 2.50f;     // 50% of 5V
const float V_LOW = 1.25f;      // 25% of 5V

// Battery update interval (60 seconds = 60000ms)
const unsigned long BATTERY_LOG_INTERVAL = 60000;

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

// Global objects
SystemInitializer* systemInit = nullptr;
BuzzerController* buzzer = nullptr;

// Battery monitor state
unsigned long lastBatteryLog = 0;
float currentVoltage = 0.0f;

// ═══════════════════════════════════════════════════════════════════════════
// BATTERY MONITOR FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

float readBatteryVoltage() {
    int raw = analogRead(BATTERY_PIN);
    return (raw / 1023.0f) * VREF;
}

void updateBatteryLeds(float voltage) {
    // Turn off all LEDs first
    digitalWrite(LED_FULL, LOW);
    digitalWrite(LED_GOOD, LOW);
    digitalWrite(LED_LOW, LOW);
    digitalWrite(LED_CRITICAL, LOW);
    
    // Turn on appropriate LED based on voltage
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

void setup() {
    Serial.begin(systemConfig.baudRate);
    while (!Serial) { delay(10); }
    
    Serial.println(F("\n\n╔══════════════════════════════════════════════════════╗"));
    Serial.println(F("║   MINIMAL TEST - Boot + Battery Monitor             ║"));
    Serial.println(F("╚══════════════════════════════════════════════════════╝\n"));
    
    // Initialize battery monitor pins
    pinMode(BATTERY_PIN, INPUT);
    pinMode(LED_FULL, OUTPUT);
    pinMode(LED_GOOD, OUTPUT);
    pinMode(LED_LOW, OUTPUT);
    pinMode(LED_CRITICAL, OUTPUT);
    Serial.println(F("✓ Battery monitor pins initialized"));
    
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
            Serial.println(F("╠══════════════════════════════════════════════════════╣"));
            Serial.println(F("║  Boot sequence verified:                             ║"));
            Serial.println(F("║    ✓ RED LED → ORANGE blink → GREEN LED              ║"));
            Serial.println(F("║    ✓ Buzzer audio feedback                           ║"));
            Serial.println(F("╠══════════════════════════════════════════════════════╣"));
            Serial.println(F("║  Battery monitor active:                             ║"));
            Serial.println(F("║    • Voltage logged every 60 seconds                 ║"));
            Serial.println(F("║    • Status LED shows battery level                  ║"));
            Serial.println(F("║    • Adjust potentiometer to test thresholds         ║"));
            Serial.println(F("╚══════════════════════════════════════════════════════╝\n"));
            
            // Initial battery reading
            currentVoltage = readBatteryVoltage();
            updateBatteryLeds(currentVoltage);
            Serial.print(F("[BATTERY] Initial voltage: "));
            Serial.print(currentVoltage, 2);
            Serial.println(F("V\n"));
            
            lastBatteryLog = millis();
        } else {
            Serial.println(F("\n✗ BOOT TEST FAILED - Check hardware connections"));
        }
    }
}

void loop() {
    // Battery voltage logging every 60 seconds
    if (millis() - lastBatteryLog >= BATTERY_LOG_INTERVAL) {
        currentVoltage = readBatteryVoltage();
        updateBatteryLeds(currentVoltage);
        
        Serial.print(F("[BATTERY] Voltage: "));
        Serial.print(currentVoltage, 2);
        Serial.print(F("V | Status: "));
        
        if (currentVoltage >= V_FULL) {
            Serial.println(F("FULL ✓"));
        } else if (currentVoltage >= V_GOOD) {
            Serial.println(F("GOOD"));
        } else if (currentVoltage >= V_LOW) {
            Serial.println(F("LOW ⚠"));
        } else {
            Serial.println(F("CRITICAL ✗"));
        }
        
        lastBatteryLog = millis();
    }
    
    delay(100);  // Small delay to prevent CPU hogging
}
