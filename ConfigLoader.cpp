#include "ConfigLoader.h"
#include <string.h>

// Default configuration values - stored in SRAM for mutability
static char defaultVersion[] = "1.0.0";
static char defaultDeviceName[] = "Arduino-Softair";
static char defaultNetworkSSID[] = "";

SystemConfig ConfigLoader::getDefaultConfig() {
    SystemConfig config;
    
    // String values
    config.version = defaultVersion;
    config.deviceName = defaultDeviceName;
    config.networkSSID = defaultNetworkSSID;
    
    // Numeric values
    config.baudRate = 115200;
    config.redLedPin = 10;
    config.orangeLedPin = 11;
    config.greenLedPin = 12;
    config.buzzerPin = 9;           // Buzzer on pin 9
    config.blinkIntervalMs = 200;
    
    return config;
}

bool ConfigLoader::loadFromJson(const char* jsonString, SystemConfig& config) {
    if (jsonString == nullptr || strlen(jsonString) == 0) {
        return false;
    }

    // For embedded systems without full JSON library, we use simple parsing
    // In production, consider using ArduinoJson library for robust parsing
    
    static char versionBuffer[32];
    static char deviceNameBuffer[64];
    static char networkSSIDBuffer[64];
    
    // Parse string values
    if (!parseJsonValue(jsonString, "version", versionBuffer, sizeof(versionBuffer))) {
        return false;
    }
    config.version = versionBuffer;
    
    if (!parseJsonValue(jsonString, "deviceName", deviceNameBuffer, sizeof(deviceNameBuffer))) {
        return false;
    }
    config.deviceName = deviceNameBuffer;
    
    // Network SSID is optional
    if (parseJsonValue(jsonString, "networkSSID", networkSSIDBuffer, sizeof(networkSSIDBuffer))) {
        config.networkSSID = networkSSIDBuffer;
    } else {
        config.networkSSID = "";
    }
    
    // Parse numeric values
    if (!parseJsonInt(jsonString, "baudRate", config.baudRate)) {
        config.baudRate = 115200; // Default
    }
    
    uint32_t tempBlinkInterval;
    if (parseJsonInt(jsonString, "blinkIntervalMs", tempBlinkInterval)) {
        config.blinkIntervalMs = (uint16_t)tempBlinkInterval;
    } else {
        config.blinkIntervalMs = 200; // Default
    }
    
    // Parse LED pins from nested object
    if (!parseJsonNestedInt(jsonString, "leds", "red", config.redLedPin)) {
        config.redLedPin = 10; // Default
    }
    
    if (!parseJsonNestedInt(jsonString, "leds", "orange", config.orangeLedPin)) {
        config.orangeLedPin = 11; // Default
    }
    
    if (!parseJsonNestedInt(jsonString, "leds", "green", config.greenLedPin)) {
        config.greenLedPin = 12; // Default
    }
    
    return true;
}

bool ConfigLoader::parseJsonValue(const char* json, const char* key, char* output, size_t maxLen) {
    // Simple JSON string parser: find "key":"value"
    char searchStr[64];
    snprintf(searchStr, sizeof(searchStr), "\"%s\"", key);
    
    const char* keyPos = strstr(json, searchStr);
    if (keyPos == nullptr) {
        return false;
    }
    
    // Find the opening quote of the value
    const char* valueStart = strchr(keyPos + strlen(searchStr), '"');
    if (valueStart == nullptr) {
        return false;
    }
    valueStart++; // Move past the opening quote
    
    // Find the closing quote
    const char* valueEnd = strchr(valueStart, '"');
    if (valueEnd == nullptr) {
        return false;
    }
    
    size_t valueLen = valueEnd - valueStart;
    if (valueLen >= maxLen) {
        valueLen = maxLen - 1;
    }
    
    strncpy(output, valueStart, valueLen);
    output[valueLen] = '\0';
    
    return true;
}

bool ConfigLoader::parseJsonInt(const char* json, const char* key, uint32_t& output) {
    // Simple JSON number parser: find "key":number
    char searchStr[64];
    snprintf(searchStr, sizeof(searchStr), "\"%s\"", key);
    
    const char* keyPos = strstr(json, searchStr);
    if (keyPos == nullptr) {
        return false;
    }
    
    // Find the colon
    const char* colonPos = strchr(keyPos + strlen(searchStr), ':');
    if (colonPos == nullptr) {
        return false;
    }
    
    // Skip whitespace
    colonPos++;
    while (*colonPos == ' ' || *colonPos == '\t' || *colonPos == '\n') {
        colonPos++;
    }
    
    // Parse the number
    output = (uint32_t)atol(colonPos);
    return true;
}

bool ConfigLoader::parseJsonNestedInt(const char* json, const char* parentKey, const char* childKey, uint8_t& output) {
    // Find the parent object
    char searchStr[64];
    snprintf(searchStr, sizeof(searchStr), "\"%s\"", parentKey);
    
    const char* parentPos = strstr(json, searchStr);
    if (parentPos == nullptr) {
        return false;
    }
    
    // Find the opening brace of the nested object
    const char* bracePos = strchr(parentPos + strlen(searchStr), '{');
    if (bracePos == nullptr) {
        return false;
    }
    
    // Find the closing brace
    const char* closeBracePos = strchr(bracePos, '}');
    if (closeBracePos == nullptr) {
        return false;
    }
    
    // Create a substring for the nested object
    size_t nestedLen = closeBracePos - bracePos + 1;
    char nestedJson[256];
    if (nestedLen >= sizeof(nestedJson)) {
        return false;
    }
    strncpy(nestedJson, bracePos, nestedLen);
    nestedJson[nestedLen] = '\0';
    
    // Parse the child key from the nested object
    uint32_t tempValue;
    if (!parseJsonInt(nestedJson, childKey, tempValue)) {
        return false;
    }
    
    output = (uint8_t)tempValue;
    return true;
}
