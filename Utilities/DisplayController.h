#ifndef DISPLAY_CONTROLLER_H
#define DISPLAY_CONTROLLER_H

#include <Arduino.h>

/**
 * @file DisplayController.h
 * @brief Controller for 4× seven-segment displays with time-division multiplexing
 * 
 * Hardware: 4 common-cathode 7-segment displays controlled via 74HC595 shift register
 * Layout: D1:D2 D3:D4 where D2 and D3 are rotated 180° (digital clock effect)
 * Decimal points between D2:D3 act as colon separator for time display
 * 
 * Pin configuration (74HC595):
 * - DATA_PIN (DS): Serial data input
 * - CLOCK_PIN (SHCP): Shift register clock
 * - LATCH_PIN (STCP): Storage register clock (output latch)
 * 
 * Memory footprint: ~50 bytes RAM (4 digit buffers + multiplexing state)
 * Timing: Minimum 50Hz refresh rate (20ms full cycle, 5ms per digit)
 */

class DisplayController {
public:
    /**
     * @brief Constructor with 74HC595 shift register pins + direct digit control
     * @param dataPin DS (serial data input)
     * @param clockPin SHCP (shift register clock)
     * @param latchPin STCP (storage register clock / output latch)
     * @param digit1Pin Direct control for digit 1 (COM cathode)
     * @param digit2Pin Direct control for digit 2 (COM cathode)
     * @param digit3Pin Direct control for digit 3 (COM cathode)
     * @param digit4Pin Direct control for digit 4 (COM cathode)
     */
    DisplayController(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin,
                     uint8_t digit1Pin, uint8_t digit2Pin, uint8_t digit3Pin, uint8_t digit4Pin);
    
    /**
     * @brief Initialize display hardware (set pin modes, clear display)
     */
    void begin();
    
    /**
     * @brief Display time in HH:MM format with colon separator
     * @param hours 0-23 (24-hour format)
     * @param minutes 0-59
     * @note Call refresh() in loop() to maintain display
     */
    void displayTime(uint8_t hours, uint8_t minutes);
    
    /**
     * @brief Set colon blinking state (for animated time separator)
     * @param enabled true to show colon, false to hide
     * @note Call every 500ms with toggled state for blinking effect
     */
    void setColonBlink(bool enabled);
    
    /**
     * @brief Run hardware diagnostic test (3× blink all segments)
     * @return true if test completed successfully
     * @note Blocking call (~3 seconds duration)
     */
    bool runDiagnosticTest();
    
    /**
     * @brief Non-blocking refresh for time-division multiplexing
     * @note MUST be called frequently in loop() (ideally every 5ms)
     */
    void refresh();
    
    /**
     * @brief Clear all display segments and decimal points
     */
    void clearDisplay();

private:
    // Hardware pins (74HC595)
    uint8_t dataPin_;
    uint8_t clockPin_;
    uint8_t latchPin_;
    
    // Digit selector pins (direct control)
    uint8_t digitPins_[4];  // One pin per digit (COM cathode)
    
    // Display buffer (stores segment patterns for each digit)
    uint8_t digitBuffer_[4];  // One byte per digit (segments a-g + dp)
    
    // Decimal points state (bit mask for 4 digits)
    uint8_t decimalPoints_;   // Bit 0-3 = DP for digit 0-3
    
    // Multiplexing state
    uint8_t currentDigit_;    // Currently active digit (0-3)
    unsigned long lastRefreshTime_; // For non-blocking refresh timing
    
    // Refresh interval (5ms = 200Hz per digit → 50Hz full cycle)
    static constexpr unsigned long REFRESH_INTERVAL_US = 5000;  // 5ms in microseconds
    
    /**
     * @brief Write segment pattern + digit selector to shift register
     * @param digitIndex Digit to activate (0-3)
     * @param segments Segment pattern (a-g + dp)
     * @param dp Decimal point state
     */
    void writeDigit(uint8_t digitIndex, uint8_t segments, bool dp);
    
    /**
     * @brief Activate specific digit selector (turn off others)
     * @param digitIndex Digit to activate (0-3)
     */
    void selectDigit(uint8_t digitIndex);
    
    /**
     * @brief Get 7-segment pattern for a digit (0-9)
     * @param number Digit to display (0-9)
     * @param rotated Apply 180° rotation (for digits D2, D3)
     * @return Segment bit pattern (DP-G-F-E-D-C-B-A)
     */
    uint8_t getSegmentPattern(uint8_t number, bool rotated);
    
    /**
     * @brief Send 16 bits to dual 74HC595 (8 segments + 4 digit selectors)
     * @param data 16-bit data (high byte = digit selectors, low byte = segments)
     */
    void shiftOut16(uint16_t data);
    
    /**
     * @brief Send 8 bits to 74HC595 (segments only)
     * @param segments 8-bit segment data (DP-G-F-E-D-C-B-A)
     */
    void shiftOut8(uint8_t segments);
};

// Segment encoding constants (common cathode)
// Bit layout: DP G F E D C B A (bit 7-0)
//
//      A
//     ┌─┐
//   F │ │ B
//     ├─┤ G
//   E │ │ C
//     └─┘ . DP
//      D
//
constexpr uint8_t SEGMENT_PATTERNS[10] = {
    0b00111111,  // 0: segments A-F (no G)
    0b00000110,  // 1: segments B,C
    0b01011011,  // 2: segments A,B,D,E,G
    0b01001111,  // 3: segments A,B,C,D,G
    0b01100110,  // 4: segments B,C,F,G
    0b01101101,  // 5: segments A,C,D,F,G
    0b01111101,  // 6: segments A,C-G (no B)
    0b00000111,  // 7: segments A,B,C
    0b01111111,  // 8: all segments A-G
    0b01101111   // 9: segments A-D,F,G (no E)
};

// 180° rotation mask: swap segments (A↔D, B↔E, C↔F, G unchanged)
// For rotated digits D2 and D3 in clock layout
constexpr uint8_t ROTATION_180_MASK = 0b00111111;  // XOR mask for A-F bits

#endif // DISPLAY_CONTROLLER_H
