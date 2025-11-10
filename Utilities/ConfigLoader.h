#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <Arduino.h>
#include "SystemInitializer.h"

/**
 * @brief Loads system configuration from JSON string
 * 
 * Expected JSON format:
 * {
 *   "version": "1.0.0",
 *   "deviceName": "Arduino-Softair",
 *   "networkSSID": "MyNetwork",
 *   "baudRate": 115200,
 *   "leds": {
 *     "red": 10,
 *     "orange": 11,
 *     "green": 12
 *   },
 *   "blinkIntervalMs": 200
 * }
 */
class ConfigLoader {
public:
    /**
     * @brief Load configuration from JSON string
     * @param jsonString JSON configuration string
     * @param config Output configuration structure
     * @return true if parsing successful, false otherwise
     */
    static bool loadFromJson(const char* jsonString, SystemConfig& config);

    /**
     * @brief Get default configuration
     * @return SystemConfig Default configuration
     */
    static SystemConfig getDefaultConfig();

private:
    static bool parseJsonValue(const char* json, const char* key, char* output, size_t maxLen);
    static bool parseJsonInt(const char* json, const char* key, uint32_t& output);
    static bool parseJsonNestedInt(const char* json, const char* parentKey, const char* childKey, uint8_t& output);
};

#endif // CONFIG_LOADER_H
