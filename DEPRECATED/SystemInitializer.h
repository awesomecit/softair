#ifndef SYSTEM_INITIALIZER_H
#define SYSTEM_INITIALIZER_H

#include <Arduino.h>

// Forward declaration
class BuzzerController;

/**
 * @brief Configuration structure for system initialization
 */
struct SystemConfig {
    const char* version;
    const char* deviceName;
    const char* networkSSID;  // Non-sensitive network info
    uint32_t baudRate;
    uint8_t redLedPin;
    uint8_t orangeLedPin;
    uint8_t greenLedPin;
    uint8_t buzzerPin;        // Buzzer pin for audio feedback
    uint16_t blinkIntervalMs;
};

/**
 * @brief System initialization states
 */
enum class InitState {
    START,
    READING_CONFIG,
    INITIALIZING,
    READY,
    ERROR
};

/**
 * @brief Manages Arduino boot sequence with LED and buzzer indicators
 * 
 * Boot sequence:
 * 1. Red LED + Startup beep: System start
 * 2. Orange LED (blinking) + Buzzer test: Component initialization
 * 3. Green LED + Success beep: System ready
 */
class SystemInitializer {
public:
    /**
     * @brief Construct a new System Initializer
     * @param config System configuration
     * @param buzzer Optional buzzer controller for audio feedback
     */
    explicit SystemInitializer(const SystemConfig& config, BuzzerController* buzzer = nullptr);

    /**
     * @brief Initialize the system and run boot sequence
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get current initialization state
     * @return InitState Current state
     */
    InitState getState() const;

    /**
     * @brief Print system information to Serial
     */
    void printSystemInfo() const;

private:
    SystemConfig config_;
    InitState state_;
    BuzzerController* buzzer_;  // Optional buzzer for audio feedback

    void setupLeds();
    void setRedLed();
    void setOrangeLedBlinking();
    void setGreenLed();
    void turnOffAllLeds();
    void logConfigReading(const char* configItem);
    bool validateConfig();
    bool testComponents();  // Test all hardware components
};

#endif // SYSTEM_INITIALIZER_H
