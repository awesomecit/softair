#include "BuzzerController.h"

// Duration values in milliseconds
static const uint16_t DUR_SHORT = 100;
static const uint16_t DUR_MEDIUM = 200;
static const uint16_t DUR_LONG = 400;

// Pause between tones during test
static const uint16_t TEST_PAUSE = 50;

BuzzerController::BuzzerController(uint8_t pin) : pin_(pin) {
}

void BuzzerController::begin() {
    pinMode(pin_, OUTPUT);
    digitalWrite(pin_, LOW);
    
    #ifdef DEBUG
    Serial.print(F("[BUZZER] Init on pin "));
    Serial.println(pin_);
    #endif
}

void BuzzerController::playTone(BuzzerFrequency frequency, BuzzerTone tonality) {
    uint16_t freq = getFrequency(frequency);
    uint16_t duration = getDuration(tonality);
    
    tone(pin_, freq, duration);
    delay(duration);
}

void BuzzerController::stop() {
    noTone(pin_);
    digitalWrite(pin_, LOW);
}

bool BuzzerController::runDiagnosticTest() {
    Serial.println(F("[BUZZER] Diagnostic test..."));
    
    // Test all 9 combinations (3 frequencies × 3 tonalities)
    for (uint8_t f = 0; f < 3; f++) {
        for (uint8_t t = 0; t < 3; t++) {
            BuzzerFrequency freq = static_cast<BuzzerFrequency>(f);
            BuzzerTone tone = static_cast<BuzzerTone>(t);
            
            #ifdef DEBUG
            Serial.print(F("  Freq: "));
            Serial.print(f);
            Serial.print(F(" Tone: "));
            Serial.println(t);
            #endif
            
            playTone(freq, tone);
            delay(TEST_PAUSE);
        }
    }
    
    stop();
    Serial.println(F("[BUZZER] Test complete"));
    return true;
}

void BuzzerController::playSuccess() {
    // Rising tone pattern: LOW → MEDIUM → HIGH
    playTone(BuzzerFrequency::FREQ_LOW, BuzzerTone::SHORT);
    delay(50);
    playTone(BuzzerFrequency::FREQ_MEDIUM, BuzzerTone::SHORT);
    delay(50);
    playTone(BuzzerFrequency::FREQ_HIGH, BuzzerTone::MEDIUM);
    stop();
}

void BuzzerController::playError() {
    // Descending tone pattern with long duration
    playTone(BuzzerFrequency::FREQ_HIGH, BuzzerTone::LONG);
    delay(100);
    playTone(BuzzerFrequency::FREQ_LOW, BuzzerTone::LONG);
    stop();
}

void BuzzerController::playStartup() {
    // Beep rapido di avvio
    playTone(BuzzerFrequency::FREQ_MEDIUM, BuzzerTone::SHORT);
}

uint16_t BuzzerController::getFrequency(BuzzerFrequency freq) const {
    switch (freq) {
        case BuzzerFrequency::FREQ_LOW:
            return 262;  // C4 (Do)
        case BuzzerFrequency::FREQ_MEDIUM:
            return 523;  // C5 (Do)
        case BuzzerFrequency::FREQ_HIGH:
            return 1046; // C6 (Do)
        default:
            return 523;
    }
}

uint16_t BuzzerController::getDuration(BuzzerTone tone) const {
    switch (tone) {
        case BuzzerTone::SHORT:
            return DUR_SHORT;
        case BuzzerTone::MEDIUM:
            return DUR_MEDIUM;
        case BuzzerTone::LONG:
            return DUR_LONG;
        default:
            return DUR_MEDIUM;
    }
}
