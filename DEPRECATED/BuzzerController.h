#ifndef BUZZER_CONTROLLER_H
#define BUZZER_CONTROLLER_H

#include <Arduino.h>

/**
 * @brief Buzzer frequency modes
 */
// Frequenze disponibili
enum BuzzerFrequency {
    FREQ_LOW = 0,      // 262 Hz (C4 - Do)
    FREQ_MEDIUM = 1,   // 523 Hz (C5 - Do)
    FREQ_HIGH = 2      // 1046 Hz (C6 - Do)
};

/**
 * @brief Buzzer tonality modes
 */
enum class BuzzerTone {
    SHORT = 0,    // 100ms
    MEDIUM = 1,   // 200ms
    LONG = 2      // 400ms
};

/**
 * @brief Controller for piezoelectric buzzer with multiple sound patterns
 * 
 * Manages buzzer with 3 frequency modes and 3 tonality durations.
 * Used for system feedback and component testing during initialization.
 */
class BuzzerController {
public:
    /**
     * @brief Construct a new Buzzer Controller
     * @param pin Digital pin connected to buzzer
     */
    explicit BuzzerController(uint8_t pin);

    /**
     * @brief Initialize buzzer hardware
     */
    void begin();

    /**
     * @brief Play tone with specific frequency and duration
     * @param frequency Frequency mode (LOW, MEDIUM, HIGH)
     * @param tonality Duration mode (SHORT, MEDIUM, LONG)
     */
    void playTone(BuzzerFrequency frequency, BuzzerTone tonality);

    /**
     * @brief Stop any playing tone
     */
    void stop();

    /**
     * @brief Test all combinations of frequency and tonality
     * Used during system initialization to verify buzzer functionality
     * @return true if test completed successfully
     */
    bool runDiagnosticTest();

    /**
     * @brief Play success sound pattern
     */
    void playSuccess();

    /**
     * @brief Play error sound pattern
     */
    void playError();

    /**
     * @brief Play startup sound pattern
     */
    void playStartup();

private:
    uint8_t pin_;
    
    uint16_t getFrequency(BuzzerFrequency freq) const;
    uint16_t getDuration(BuzzerTone tone) const;
};

#endif // BUZZER_CONTROLLER_H
