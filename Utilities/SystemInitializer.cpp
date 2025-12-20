#include "SystemInitializer.h"
#include "BuzzerController.h"

SystemInitializer::SystemInitializer(const SystemConfig& config, BuzzerController* buzzer)
    : config_(config), state_(InitState::START), buzzer_(buzzer) {
}

bool SystemInitializer::initialize() {
    setupLeds();
    
    // Phase 1: System start - Red LED + Startup beep
    setRedLed();
    state_ = InitState::START;
    
    if (buzzer_) {
        buzzer_->playStartup();
    }
    
    #ifdef DEBUG
    Serial.println(F("=== Boot Started ==="));
    #endif
    
    delay(500);
    
    // Phase 2: Reading configuration - Orange LED (blinking)
    state_ = InitState::READING_CONFIG;
    setOrangeLedBlinking();
    
    #ifdef DEBUG
    Serial.println(F("Reading config..."));
    #endif
    
    // Simulate configuration reading with logs
    logConfigReading("Device name");
    delay(300);
    logConfigReading("Version");
    delay(300);
    logConfigReading("Network");
    delay(300);
    logConfigReading("Baud rate");
    delay(300);
    
    // Validate configuration
    if (!validateConfig()) {
        state_ = InitState::ERROR;
        #ifdef DEBUG
        Serial.println(F("ERROR: Config invalid!"));
        #endif
        if (buzzer_) {
            buzzer_->playError();
        }
        return false;
    }
    
    // Phase 3: Initializing - Test all components
    state_ = InitState::INITIALIZING;
    #ifdef DEBUG
    Serial.println(F("Initializing components..."));
    #endif
    
    if (!testComponents()) {
        state_ = InitState::ERROR;
        if (buzzer_) {
            buzzer_->playError();
        }
        return false;
    }
    
    // Phase 4: Ready - Green LED + Success beep
    turnOffAllLeds();
    setGreenLed();
    state_ = InitState::READY;
    
    if (buzzer_) {
        buzzer_->playSuccess();
    }
    
    #ifdef DEBUG
    Serial.println(F("=== Init Complete ==="));
    printSystemInfo();
    #endif
    
    return true;
}

void SystemInitializer::setupLeds() {
    // DISABLED: Old LED pins (10,11,12) now used for display multiplexing
    // Status feedback now via Neopixel on pin 8
    
    #ifdef DEBUG
    Serial.println(F("[BOOT] Legacy LED setup skipped (using Neopixel instead)"));
    #endif
}

void SystemInitializer::setRedLed() {
    // DISABLED: Using Neopixel instead
    #ifdef DEBUG
    Serial.println(F("[BOOT] Phase 1: START"));
    #endif
}

void SystemInitializer::setOrangeLedBlinking() {
    // DISABLED: Using Neopixel instead
    #ifdef DEBUG
    Serial.println(F("[BOOT] Phase 2: READING CONFIG"));
    #endif
    
    // Simulate config reading delay (no LED blink)
    delay(1000);
}

void SystemInitializer::setGreenLed() {
    // DISABLED: Using Neopixel instead
    #ifdef DEBUG
    Serial.println(F("[BOOT] Phase 3: READY"));
    #endif
}

void SystemInitializer::turnOffAllLeds() {
    // DISABLED: No longer controlling LED pins
}

void SystemInitializer::disableBootLeds() {
    // No action needed - pins never configured for LEDs
    #ifdef DEBUG
    Serial.println(F("[BOOT] Boot sequence complete"));
    #endif
}

void SystemInitializer::logConfigReading(const char* configItem) {
    #ifdef DEBUG
    Serial.print(F("  [CFG] "));
    Serial.println(configItem);
    #endif
}

bool SystemInitializer::validateConfig() {
    // Validate required configuration fields
    if (config_.version == nullptr || strlen(config_.version) == 0) {
        return false;
    }
    if (config_.deviceName == nullptr || strlen(config_.deviceName) == 0) {
        return false;
    }
    if (config_.baudRate == 0) {
        return false;
    }
    return true;
}

InitState SystemInitializer::getState() const {
    return state_;
}

void SystemInitializer::printSystemInfo() const {
    Serial.println(F("\n=== SYSTEM INFO ==="));
    
    Serial.print(F("Device: "));
    Serial.println(config_.deviceName);
    
    Serial.print(F("Version: "));
    Serial.println(config_.version);
    
    Serial.print(F("Baud: "));
    Serial.println(config_.baudRate);
    
    if (config_.networkSSID && strlen(config_.networkSSID) > 0) {
        Serial.print(F("Network: "));
        Serial.println(config_.networkSSID);
    }
    
    Serial.print(F("Status: "));
    switch (state_) {
        case InitState::READY:
            Serial.println(F("READY"));
            break;
        case InitState::ERROR:
            Serial.println(F("ERROR"));
            break;
        default:
            Serial.println(F("INIT"));
            break;
    }
    
    Serial.println(F("===================\n"));
}

bool SystemInitializer::testComponents() {
    Serial.println(F("[TEST] Hardware components..."));
    
    // Test buzzer if available
    if (buzzer_) {
        Serial.println(F("[TEST] Buzzer diagnostic..."));
        if (!buzzer_->runDiagnosticTest()) {
            Serial.println(F("[ERROR] Buzzer test failed!"));
            return false;
        }
        Serial.println(F("[OK] Buzzer test passed"));
    }
    
    // Add more component tests here as needed
    // Example: testButton(), testSensor(), etc.
    
    Serial.println(F("[TEST] All components OK"));
    return true;
}
