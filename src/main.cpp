#include <Arduino.h>
#include <SystemInitializer.h>
#include <BuzzerController.h>

// ═══════════════════════════════════════════════════════════════════════════
// MINIMAL TEST - BOOT SEQUENCE VALIDATION
// ═══════════════════════════════════════════════════════════════════════════
// Test SOLO SystemInitializer + BuzzerController
// Valida: boot LEDs (red→orange→green) + buzzer audio feedback
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

// Global objects
SystemInitializer* systemInit = nullptr;
BuzzerController* buzzer = nullptr;

void setup() {
    Serial.begin(systemConfig.baudRate);
    while (!Serial) { delay(10); }
    
    Serial.println(F("\n\n╔══════════════════════════════════════════════════════╗"));
    Serial.println(F("║       MINIMAL BOOT TEST - SystemInitializer         ║"));
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
            Serial.println(F("╠══════════════════════════════════════════════════════╣"));
            Serial.println(F("║  Expected sequence:                                  ║"));
            Serial.println(F("║    1. RED LED + startup beep                         ║"));
            Serial.println(F("║    2. ORANGE LED blink (5x) + config reading         ║"));
            Serial.println(F("║    3. Buzzer diagnostic test (9 tones)               ║"));
            Serial.println(F("║    4. GREEN LED + success beep                       ║"));
            Serial.println(F("╠══════════════════════════════════════════════════════╣"));
            Serial.println(F("║  Verify visually:                                    ║"));
            Serial.println(F("║    • RED LED turned on first                         ║"));
            Serial.println(F("║    • ORANGE LED blinked 5 times                      ║"));
            Serial.println(F("║    • GREEN LED is now ON                             ║"));
            Serial.println(F("║    • Heard 3 beeps (startup, diagnostic, success)    ║"));
            Serial.println(F("╚══════════════════════════════════════════════════════╝\n"));
            
            // Keep green LED on permanently as success indicator
            Serial.println(F("Green LED will remain ON as success indicator."));
            Serial.println(F("Test complete - you can stop simulation.\n"));
        } else {
            Serial.println(F("\n✗ BOOT TEST FAILED - Check hardware connections"));
        }
    }
}

void loop() {
    // Nothing to do - this is a one-shot boot test
    // Green LED stays on to indicate successful boot
    delay(1000);
}
