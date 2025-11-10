#include "DisplayController.h"

// ═══════════════════════════════════════════════════════════════════════════
// CONSTRUCTOR & INITIALIZATION
// ═══════════════════════════════════════════════════════════════════════════

DisplayController::DisplayController(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin,
                                   uint8_t digit1Pin, uint8_t digit2Pin, uint8_t digit3Pin, uint8_t digit4Pin)
    : dataPin_(dataPin),
      clockPin_(clockPin),
      latchPin_(latchPin),
      decimalPoints_(0),
      currentDigit_(0),
      lastRefreshTime_(0) {
    // Initialize digit buffer to all off
    for (uint8_t i = 0; i < 4; i++) {
        digitBuffer_[i] = 0x00;
    }
    
    // Store digit selector pins
    digitPins_[0] = digit1Pin;
    digitPins_[1] = digit2Pin;
    digitPins_[2] = digit3Pin;
    digitPins_[3] = digit4Pin;
}

void DisplayController::begin() {
    // Configure shift register pins
    pinMode(dataPin_, OUTPUT);
    pinMode(clockPin_, OUTPUT);
    pinMode(latchPin_, OUTPUT);
    
    // Configure digit selector pins (common cathode - LOW = ON)
    for (uint8_t i = 0; i < 4; i++) {
        pinMode(digitPins_[i], OUTPUT);
        digitalWrite(digitPins_[i], HIGH);  // Turn all digits OFF initially
    }
    
    // Initialize all outputs LOW
    digitalWrite(dataPin_, LOW);
    digitalWrite(clockPin_, LOW);
    digitalWrite(latchPin_, LOW);
    
    // Clear display
    clearDisplay();
    
    Serial.println(F("[DISPLAY] Initialized"));
}

// ═══════════════════════════════════════════════════════════════════════════
// PUBLIC API
// ═══════════════════════════════════════════════════════════════════════════

void DisplayController::displayTime(uint8_t hours, uint8_t minutes) {
    // Validate input
    if (hours > 23) hours = 0;
    if (minutes > 59) minutes = 0;
    
    // Extract digits
    uint8_t hoursTens = hours / 10;
    uint8_t hoursOnes = hours % 10;
    uint8_t minutesTens = minutes / 10;
    uint8_t minutesOnes = minutes % 10;
    
    // Store segment patterns in buffer
    // Layout: HH:mm with D3 and D4 physically rotated 180° in diagram.json
    // D1 and D2 = normal orientation
    // D3 and D4 = rotated 180° → apply software rotation to patterns
    
    digitBuffer_[0] = getSegmentPattern(hoursTens, false);   // D1: normal
    digitBuffer_[1] = getSegmentPattern(hoursOnes, false);   // D2: normal
    digitBuffer_[2] = getSegmentPattern(minutesTens, true);  // D3: rotated 180°
    digitBuffer_[3] = getSegmentPattern(minutesOnes, true);  // D4: rotated 180°
    
    // Note: Decimal points (colon) are controlled by setColonBlink()
    // Do NOT set decimalPoints_ here - it's managed by the blink logic
}

void DisplayController::setColonBlink(bool enabled) {
    // Enable or disable colon decimal points (D2 and D3)
    if (enabled) {
        decimalPoints_ |= 0b0110;   // Set bits 1 and 2
    } else {
        decimalPoints_ &= ~0b0110;  // Clear bits 1 and 2
    }
}

bool DisplayController::runDiagnosticTest() {
    Serial.println(F("[DISPLAY] Running diagnostic test..."));
    
    const uint8_t ALL_SEGMENTS_ON = 0xFF;  // All 8 bits HIGH
    const uint8_t BLINK_CYCLES = 3;
    const uint16_t BLINK_INTERVAL = 500;   // 0.5 seconds
    const uint16_t INIT_DURATION = 2000;   // 2 seconds with all segments ON
    
    // Phase 1: All segments ON for 2 seconds (hardware test)
    Serial.println(F("[DISPLAY] Phase 1: All segments ON (2s)"));
    for (uint8_t digit = 0; digit < 4; digit++) {
        digitBuffer_[digit] = ALL_SEGMENTS_ON;
    }
    decimalPoints_ = 0xFF;  // All DPs on
    
    unsigned long startTime = millis();
    while (millis() - startTime < INIT_DURATION) {
        refresh();  // Keep display updated during delay
        delayMicroseconds(100);
    }
    
    // Phase 2: Blink test (3 cycles)
    Serial.println(F("[DISPLAY] Phase 2: Blink test (3 cycles)"));
    for (uint8_t cycle = 0; cycle < BLINK_CYCLES; cycle++) {
        // Turn ALL segments ON (including decimal points)
        for (uint8_t digit = 0; digit < 4; digit++) {
            digitBuffer_[digit] = ALL_SEGMENTS_ON;
        }
        decimalPoints_ = 0xFF;  // All DPs on
        
        // Display all segments for BLINK_INTERVAL
        startTime = millis();
        while (millis() - startTime < BLINK_INTERVAL) {
            refresh();  // Keep display updated during delay
            delayMicroseconds(100);  // Small delay to avoid CPU hogging
        }
        
        // Turn ALL segments OFF
        clearDisplay();
        
        // Keep display off for BLINK_INTERVAL
        startTime = millis();
        while (millis() - startTime < BLINK_INTERVAL) {
            refresh();  // Maintain blank display
            delayMicroseconds(100);
        }
    }
    
    Serial.println(F("[DISPLAY] Diagnostic test complete"));
    return true;
}

void DisplayController::refresh() {
    // Non-blocking multiplexing: only update if enough time has passed
    unsigned long currentMicros = micros();
    if (currentMicros - lastRefreshTime_ < REFRESH_INTERVAL_US) {
        return;  // Not time yet
    }
    lastRefreshTime_ = currentMicros;
    
    // Get segment pattern for current digit
    uint8_t segments = digitBuffer_[currentDigit_];
    
    // Check if decimal point is enabled for this digit
    bool dp = (decimalPoints_ >> currentDigit_) & 0x01;
    
    // Write to display
    writeDigit(currentDigit_, segments, dp);
    
    // Move to next digit (wrap around)
    currentDigit_ = (currentDigit_ + 1) % 4;
}

void DisplayController::clearDisplay() {
    for (uint8_t i = 0; i < 4; i++) {
        digitBuffer_[i] = 0x00;
    }
    decimalPoints_ = 0x00;
    
    // Immediately update hardware
    shiftOut8(0x00);  // All segments OFF
    for (uint8_t i = 0; i < 4; i++) {
        digitalWrite(digitPins_[i], HIGH);  // All digits OFF (common cathode)
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// PRIVATE METHODS
// ═══════════════════════════════════════════════════════════════════════════

void DisplayController::writeDigit(uint8_t digitIndex, uint8_t segments, bool dp) {
    // Add decimal point to segment pattern if enabled
    if (dp) {
        segments |= 0x80;  // Set bit 7 (DP)
    }
    
    // Turn OFF all digits first (common cathode - HIGH = OFF)
    for (uint8_t i = 0; i < 4; i++) {
        digitalWrite(digitPins_[i], HIGH);
    }
    
    // Send segment data to 74HC595
    shiftOut8(segments);
    
    // Turn ON the selected digit (common cathode - LOW = ON)
    digitalWrite(digitPins_[digitIndex], LOW);
}

void DisplayController::selectDigit(uint8_t digitIndex) {
    // Helper method (currently unused - logic integrated in writeDigit)
    // Could be used for alternative multiplexing patterns
    writeDigit(digitIndex, 0x00, false);
}

uint8_t DisplayController::getSegmentPattern(uint8_t number, bool rotated) {
    // Validate input
    if (number > 9) number = 0;
    
    // Get base pattern
    uint8_t pattern = SEGMENT_PATTERNS[number];
    
    // Apply 180° rotation if requested
    if (rotated) {
        // Rotation logic: swap A↔D, B↔E, C↔F, G unchanged
        // Original: DP G F E D C B A (bit 7-0)
        // Rotated:  DP G C B A F E D
        
        uint8_t a = (pattern >> 0) & 0x01;
        uint8_t b = (pattern >> 1) & 0x01;
        uint8_t c = (pattern >> 2) & 0x01;
        uint8_t d = (pattern >> 3) & 0x01;
        uint8_t e = (pattern >> 4) & 0x01;
        uint8_t f = (pattern >> 5) & 0x01;
        uint8_t g = (pattern >> 6) & 0x01;
        
        // Reconstruct with swapped segments
        pattern = (g << 6) | (c << 5) | (b << 4) | (a << 3) | (f << 2) | (e << 1) | d;
    }
    
    return pattern;
}

void DisplayController::shiftOut16(uint16_t data) {
    // Legacy method - not used with direct digit control
    // Kept for compatibility
    shiftOut8(data & 0xFF);
}

void DisplayController::shiftOut8(uint8_t segments) {
    // Pull latch LOW to start data transfer
    digitalWrite(latchPin_, LOW);
    
    // Shift out 8 bits MSB first
    for (int8_t i = 7; i >= 0; i--) {
        // Set data pin to current bit value
        digitalWrite(dataPin_, (segments >> i) & 0x01);
        
        // Pulse clock to shift bit into register
        digitalWrite(clockPin_, HIGH);
        digitalWrite(clockPin_, LOW);
    }
    
    // Pull latch HIGH to transfer data to output pins
    digitalWrite(latchPin_, HIGH);
}
