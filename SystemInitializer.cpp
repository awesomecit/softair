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
    pinMode(config_.redLedPin, OUTPUT);
    pinMode(config_.orangeLedPin, OUTPUT);
    pinMode(config_.greenLedPin, OUTPUT);
    turnOffAllLeds();
    
    // Debug: Print LED pins
    Serial.print(F("LED Pins: R="));
    Serial.print(config_.redLedPin);
    Serial.print(F(" O="));
    Serial.print(config_.orangeLedPin);
    Serial.print(F(" G="));
    Serial.println(config_.greenLedPin);
}

void SystemInitializer::setRedLed() {
    turnOffAllLeds();
    Serial.println(F("[LED] RED ON"));
    digitalWrite(config_.redLedPin, HIGH);
}

void SystemInitializer::setOrangeLedBlinking() {
    turnOffAllLeds();
    
    Serial.println(F("[LED] ORANGE BLINK"));
    // Blink orange LED during config reading phase
    const int blinkCount = 5;
    for (int i = 0; i < blinkCount; i++) {
        digitalWrite(config_.orangeLedPin, HIGH);
        delay(config_.blinkIntervalMs / 2);
        digitalWrite(config_.orangeLedPin, LOW);
        delay(config_.blinkIntervalMs / 2);
    }
}

void SystemInitializer::setGreenLed() {
    Serial.println(F("[LED] GREEN ON"));
    digitalWrite(config_.greenLedPin, HIGH);
}

void SystemInitializer::turnOffAllLeds() {
    digitalWrite(config_.redLedPin, LOW);
    digitalWrite(config_.orangeLedPin, LOW);
    digitalWrite(config_.greenLedPin, LOW);
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
