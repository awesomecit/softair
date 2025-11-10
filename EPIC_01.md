# Battery Monitor System - Complete Implementation Guide

**Version**: 1.0  
**Target**: Arduino Uno (ATmega328P)  
**Author**: Antonio - Promedital Team  
**Date**: 2025-11-10  

---

## 📋 Table of Contents

1. [Project Overview](#project-overview)
2. [System Architecture](#system-architecture)
3. [Hardware Design](#hardware-design)
4. [Software Design](#software-design)
5. [Implementation](#implementation)
6. [Testing & Debugging](#testing--debugging)
7. [Build Configuration](#build-configuration)

---

# Project Overview

## Goals

Implementare un sistema di monitoraggio batteria professionale per Arduino con:

- ✅ **4 stadi di scarica**: FULL (100-75%), GOOD (75-50%), LOW (50-25%), CRITICAL (<25%)
- ✅ **Feedback LED multiplo**: 4 LED con pattern diversi (steady/blink lento/blink veloce)
- ✅ **Configurazione compile-time**: Tutte le costanti configurabili via namespace
- ✅ **Simulazione batteria**: Modalità test con scarica virtuale temporizzata
- ✅ **Resource monitoring**: Debug con statistiche RAM/Flash/Timing
- ✅ **Clean Code**: Principi SOLID, DRY, const correctness, type safety
- ✅ **Basso consumo**: Ottimizzato per batteria, senza heap allocation

## Technical Specifications

| Specification | Value |
|--------------|-------|
| Microcontroller | ATmega328P (Arduino Uno) |
| SRAM | 2048 bytes |
| Flash | 32768 bytes (30720 usable) |
| ADC Resolution | 10-bit (1024 levels) |
| ADC Reference | 5.0V |
| Voltage Divider | R1=10kΩ, R2=10kΩ (1:1) |
| Battery Type | 9V alkaline (configurable) |
| Sampling Rate | 10 samples @ 100ms interval |
| Update Rate | 2 seconds (configurable) |
| LED Count | 4 (Green/Yellow/Orange/Red) |

---

# System Architecture

## Component Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                         ARDUINO UNO                              │
│                                                                   │
│  ┌─────────────────┐        ┌──────────────────┐               │
│  │ BatteryConfig   │◄───────│ BatteryMonitor   │               │
│  │ (namespace)     │        │ - voltage_       │               │
│  │ - Thresholds    │        │ - state_         │               │
│  │ - Pins          │        │ + update()       │               │
│  │ - Timing        │        │ + getVoltage()   │               │
│  └─────────────────┘        │ + getState()     │               │
│           ▲                  │ + getPercentage()│               │
│           │                  └────────┬─────────┘               │
│           │                           │                          │
│           │                           ▼                          │
│  ┌────────┴────────┐        ┌──────────────────┐               │
│  │ BatteryState    │        │ BatteryFeedback  │               │
│  │ (enum class)    │◄───────│ - ledPins_[4]    │               │
│  │ - FULL          │        │ + update(state)  │               │
│  │ - GOOD          │        │ + bootAnimation()│               │
│  │ - LOW           │        └──────────────────┘               │
│  │ - CRITICAL      │                 │                          │
│  └─────────────────┘                 │                          │
│                                       ▼                          │
│  ┌──────────────────────┐   ┌─────────────────┐                │
│  │ BatterySimulator     │   │ LED Pins        │                │
│  │ (debug/test only)    │   │ D2 D3 D4 D5     │                │
│  │ - currentVoltage_    │   └─────────────────┘                │
│  │ + getSimulatedADC()  │            │                          │
│  └──────────────────────┘            │                          │
│           │                           ▼                          │
│           │                  ┌─────────────────┐                │
│           └─────────────────►│ Voltage Divider │                │
│                               │ VBatt → A0      │                │
│                               └─────────────────┘                │
│                                                                   │
│  ┌───────────────────────────────────────────────────┐          │
│  │ ResourceMonitor (DEBUG_RESOURCES only)            │          │
│  │ - Tracks RAM/Flash/Timing                         │          │
│  │ - Prints periodic reports                         │          │
│  └───────────────────────────────────────────────────┘          │
└─────────────────────────────────────────────────────────────────┘
```

## Data Flow Diagram

```
┌─────────────┐
│   VBatt     │ (9V Battery)
└──────┬──────┘
       │
       ▼
┌─────────────────────────────────────┐
│  Voltage Divider (R1+R2)            │
│  Vout = VBatt × (R2/(R1+R2))        │
└──────┬──────────────────────────────┘
       │ (4.5V)
       ▼
┌─────────────────────────────────────┐
│  Arduino ADC (A0)                   │
│  10-bit: 0-1023 → 0-5V              │
└──────┬──────────────────────────────┘
       │ (ADC Value)
       ▼
┌─────────────────────────────────────┐
│  BatteryMonitor::readVoltage()      │
│  - ADC → Voltage conversion         │
│  - Voltage divider reverse calc     │
└──────┬──────────────────────────────┘
       │ (Single Sample)
       ▼
┌─────────────────────────────────────┐
│  BatteryMonitor::readFilteredV()    │
│  - Moving average (10 samples)      │
│  - Noise reduction                  │
└──────┬──────────────────────────────┘
       │ (Filtered Voltage)
       ▼
┌─────────────────────────────────────┐
│  BatteryMonitor::calculateState()   │
│  - Compare vs thresholds            │
│  - Apply hysteresis                 │
└──────┬──────────────────────────────┘
       │ (BatteryState enum)
       ▼
┌─────────────────────────────────────┐
│  BatteryFeedback::update()          │
│  - Map state → LED                  │
│  - Apply blink patterns             │
└──────┬──────────────────────────────┘
       │
       ▼
┌─────────────────────────────────────┐
│  LED Output (D2/D3/D4/D5)           │
│  - FULL: Green steady               │
│  - GOOD: Yellow steady              │
│  - LOW: Orange blink slow           │
│  - CRITICAL: Red blink fast         │
└─────────────────────────────────────┘
```

## State Machine Diagram

```
                    ┌─────────────────────────────────┐
                    │          UNKNOWN                │
                    │   (Initial state at boot)       │
                    └──────────────┬──────────────────┘
                                   │ begin()
                                   ▼
                    ┌─────────────────────────────────┐
                    │          FULL                   │
    ┌──────────────►│   V >= 8.5V (+ hysteresis)     │
    │               │   LED: Green (steady)           │
    │               └──────────────┬──────────────────┘
    │                              │ V < 8.5V
    │                              ▼
    │               ┌─────────────────────────────────┐
    │               │          GOOD                   │
    │   V >= 8.5V   │   8.0V <= V < 8.5V             │
    └───────────────┤   LED: Yellow (steady)          │
                    └──────────────┬──────────────────┘
    │                              │ V < 8.0V
    │                              ▼
    │               ┌─────────────────────────────────┐
    │               │          LOW                    │
    │   V >= 8.0V   │   7.5V <= V < 8.0V             │
    └───────────────┤   LED: Orange (blink 1Hz)      │
                    └──────────────┬──────────────────┘
    │                              │ V < 7.5V
    │                              ▼
    │               ┌─────────────────────────────────┐
    │               │        CRITICAL                 │
    │   V >= 7.5V   │   V < 7.5V                     │
    └───────────────┤   LED: Red (blink 3Hz)         │
                    │   + Warning messages            │
                    └─────────────────────────────────┘

Hysteresis: ±50mV to prevent rapid state changes
```

## Timing Diagram

```
Time (ms)     0    100   200   300   400   500   ...  2000  2100
              │     │     │     │     │     │          │     │
ADC Sample    ●─────●─────●─────●─────●─────●          ●─────●
              └─ Sample 1.....................Sample 10
              
Filtering     │                                   │
              └───────── Moving Average ──────────●
              
State Calc                                        │
                                                  └──● Calculate
              
LED Update    ●────────●────────●────────●────────●────────●
              50ms intervals for blink patterns
              
Serial Print                                             │
                                                         └─● Print
              Every 2000ms (UPDATE_INTERVAL_MS)
```

---

# Hardware Design

## Circuit Schematic

```
                         Arduino Uno
                    ┌─────────────────┐
                    │                 │
VBatt (9V) ─────┬───│ VIN             │
                │   │                 │
                │   │  D2 ├───[330Ω]───●──┐ LED Green (FULL)
                │   │                      ─
                │   │  D3 ├───[330Ω]───●──┐ LED Yellow (GOOD)
                │   │                      ─
                │   │  D4 ├───[330Ω]───●──┐ LED Orange (LOW)
                │   │                      ─
                │   │  D5 ├───[330Ω]───●──┐ LED Red (CRITICAL)
                │   │                      ─
                │   │                 │
                │   │  A0 ├───────────┼─── Voltage Sense
                │   │                 │
             [R1]   │                 │
             10kΩ   │                 │
                │   │                 │
                ├───┴─────────────────┘
                │
             [R2]
             10kΩ
                │
               GND

Voltage Divider Calculation:
- VBatt = 9.0V
- R1 = R2 = 10kΩ
- Vout = VBatt × (R2/(R1+R2)) = 9V × 0.5 = 4.5V
- Safe for 5V ADC ✓
- ADC Reading: (4.5V / 5V) × 1023 = 921 counts
```

## Component List

| Component | Quantity | Value | Notes |
|-----------|----------|-------|-------|
| Arduino Uno | 1 | - | ATmega328P |
| Resistor | 2 | 10kΩ | Voltage divider (1% tolerance recommended) |
| Resistor | 4 | 330Ω | LED current limiting |
| LED | 1 | Green | 5mm, ~20mA |
| LED | 1 | Yellow | 5mm, ~20mA |
| LED | 1 | Orange | 5mm, ~20mA |
| LED | 1 | Red | 5mm, ~20mA |
| Battery | 1 | 9V | Alkaline or Li-Ion |
| Wires | - | - | 22-24 AWG |

## Voltage Thresholds Table

| State | Voltage Range | Percentage | ADC Range (Vout) | ADC Counts |
|-------|--------------|------------|------------------|------------|
| FULL | ≥ 8.5V | 75-100% | ≥ 4.25V | ≥ 870 |
| GOOD | 8.0-8.5V | 50-75% | 4.0-4.25V | 819-870 |
| LOW | 7.5-8.0V | 25-50% | 3.75-4.0V | 767-819 |
| CRITICAL | < 7.5V | 0-25% | < 3.75V | < 767 |

---

# Software Design

## Class Hierarchy

```
BatteryConfig (namespace)
    ├── Voltage thresholds (constexpr)
    ├── Hardware pins (constexpr)
    └── Timing constants (constexpr)

BatteryState (enum class)
    ├── FULL
    ├── GOOD
    ├── LOW
    ├── CRITICAL
    └── UNKNOWN

BatteryMonitor (class)
    ├── Private Members:
    │   ├── float currentVoltage_
    │   ├── float previousVoltage_
    │   ├── BatteryState state_
    │   └── BatteryState previousState_
    ├── Public Methods:
    │   ├── void begin()
    │   ├── void update()
    │   ├── float getVoltage() const
    │   ├── BatteryState getState() const
    │   ├── uint8_t getPercentage() const
    │   ├── bool stateChanged() const
    │   └── bool needsCharge() const
    └── Private Methods:
        ├── float readVoltage() const
        ├── float readFilteredVoltage() const
        ├── BatteryState calculateState() const
        └── uint8_t calculatePercentage() const

BatteryFeedback (class)
    ├── Private Members:
    │   ├── unsigned long lastBlinkUpdate_
    │   └── bool blinkState_
    ├── Public Methods:
    │   ├── void begin()
    │   ├── void update(BatteryState)
    │   └── void bootAnimation()
    └── Private Methods:
        ├── void turnAllOff()
        ├── void updateBlinkState()
        └── bool getBlinkState(uint16_t) const

BatterySimulator (class - optional)
    ├── Private Members:
    │   ├── Config config_
    │   ├── float currentVoltage_
    │   ├── uint32_t startTime_
    │   └── bool isActive_
    ├── Public Methods:
    │   ├── void begin()
    │   ├── void update()
    │   ├── uint16_t getSimulatedADC() const
    │   └── float getCurrentVoltage() const
    └── Private Methods:
        └── float calculateRealisticVoltage(float) const

ResourceMonitor (class - optional)
    ├── Components:
    │   ├── MemoryInfo (static class)
    │   └── TimingInfo (class)
    └── Public Methods:
        ├── void begin()
        ├── void update()
        ├── void printReport()
        └── void printWarnings()
```

## Algorithm Details

### Voltage Reading Algorithm

```cpp
Algorithm: readFilteredVoltage()
Input: None (reads from ADC)
Output: Filtered battery voltage (float)

1. Initialize sum = 0.0
2. FOR i = 0 TO SAMPLES-1:
   a. Read ADC value (0-1023)
   b. Convert to voltage: vOut = (ADC/1023) × VREF
   c. Apply divider: vBatt = vOut × ((R1+R2)/R2)
   d. Add to sum
   e. IF i < SAMPLES-1: delay(SAMPLE_INTERVAL_MS)
3. Return sum / SAMPLES

Complexity: O(n) where n = SAMPLES
Time: ~1 second (10 samples × 100ms)
```

### State Calculation Algorithm

```cpp
Algorithm: calculateState()
Input: currentVoltage, previousVoltage
Output: BatteryState enum

1. Calculate hysteresis offset:
   IF currentVoltage > previousVoltage:
      hysteresis = HYSTERESIS (0.05V)
   ELSE:
      hysteresis = 0.0

2. Compare voltage with thresholds (descending):
   IF voltage >= FULL_THRESHOLD + hysteresis:
      RETURN FULL
   ELSE IF voltage >= GOOD_THRESHOLD + hysteresis:
      RETURN GOOD
   ELSE IF voltage >= LOW_THRESHOLD + hysteresis:
      RETURN LOW
   ELSE:
      RETURN CRITICAL

Complexity: O(1)
Hysteresis prevents rapid state oscillations
```

### Percentage Calculation Algorithm

```cpp
Algorithm: calculatePercentage()
Input: currentVoltage
Output: Battery percentage (0-100)

1. Define range:
   range = NOMINAL_VOLTAGE - CRITICAL_THRESHOLD
   
2. Calculate current position:
   current = currentVoltage - CRITICAL_THRESHOLD
   
3. Calculate percentage:
   percentage = (current / range) × 100
   
4. Constrain to valid range:
   RETURN constrain(percentage, 0, 100)

Example:
- NOMINAL = 9.0V, CRITICAL = 7.0V
- Range = 2.0V
- If voltage = 8.0V: current = 1.0V
- Percentage = (1.0/2.0) × 100 = 50%

Complexity: O(1)
```

---

# Implementation

## File Structure

```
battery-monitor/
├── platformio.ini                 # Build configuration
├── show_memory.py                 # Flash usage script
├── include/
│   ├── BatteryConfig.h           # Configuration (250 lines)
│   ├── BatteryState.h            # State enum (80 lines)
│   ├── BatteryMonitor.h          # Main monitor class (200 lines)
│   ├── BatteryFeedback.h         # LED controller (150 lines)
│   ├── BatterySimulator.h        # Simulator (200 lines)
│   └── ResourceMonitor.h         # Debug monitor (600 lines)
└── src/
    └── main.cpp                   # Application (250 lines)

Total: ~1730 lines of code
Estimated Flash: ~8-10KB
Estimated SRAM: ~400-500 bytes
```

## Complete Source Code

### include/BatteryConfig.h

```cpp
/**
 * @file BatteryConfig.h
 * @brief Compile-time configuration for battery monitoring system
 * 
 * INSTRUCTIONS FOR COPILOT:
 * - This file contains ONLY configuration constants
 * - All values are constexpr for compile-time optimization
 * - Modify these values to adapt to different battery types
 * - Do NOT include any logic or implementation here
 * 
 * CONFIGURATION EXAMPLES:
 * 
 * 9V Alkaline Battery:
 *   NOMINAL_VOLTAGE = 9.0f
 *   FULL_THRESHOLD = 8.5f
 *   GOOD_THRESHOLD = 8.0f
 *   LOW_THRESHOLD = 7.5f
 *   CRITICAL_THRESHOLD = 7.0f
 * 
 * Li-Ion 3.7V Battery:
 *   NOMINAL_VOLTAGE = 4.2f
 *   FULL_THRESHOLD = 4.0f
 *   GOOD_THRESHOLD = 3.7f
 *   LOW_THRESHOLD = 3.5f
 *   CRITICAL_THRESHOLD = 3.2f
 *   Voltage divider: R1=0Ω (direct), R2=∞ (or use 10k/47k)
 * 
 * 12V Lead-Acid Battery:
 *   NOMINAL_VOLTAGE = 12.6f
 *   FULL_THRESHOLD = 12.2f
 *   GOOD_THRESHOLD = 11.8f
 *   LOW_THRESHOLD = 11.4f
 *   CRITICAL_THRESHOLD = 10.8f
 *   Voltage divider: R1=22kΩ, R2=10kΩ (divides by ~3.2)
 */

#ifndef BATTERY_CONFIG_H
#define BATTERY_CONFIG_H

#include <Arduino.h>

namespace BatteryConfig {
    
    // ==================== VOLTAGE THRESHOLDS ====================
    
    // Battery voltage thresholds (Volts)
    // These define the 4 states: FULL, GOOD, LOW, CRITICAL
    constexpr float NOMINAL_VOLTAGE = 9.0f;      // Fully charged voltage
    constexpr float FULL_THRESHOLD = 8.5f;       // Full state: 75-100% (>= 8.5V)
    constexpr float GOOD_THRESHOLD = 8.0f;       // Good state: 50-75% (>= 8.0V)
    constexpr float LOW_THRESHOLD = 7.5f;        // Low state: 25-50% (>= 7.5V)
    constexpr float CRITICAL_THRESHOLD = 7.0f;   // Critical state: 0-25% (< 7.5V)
    
    // Hysteresis to prevent rapid state changes (Volts)
    // Applied when voltage is INCREASING to prevent oscillation
    constexpr float HYSTERESIS = 0.05f;          // 50mV hysteresis
    
    // ==================== HARDWARE CONFIGURATION ====================
    
    // ADC Configuration
    constexpr uint8_t ANALOG_PIN = A0;           // Analog input pin for voltage sensing
    constexpr float VREF = 5.0f;                 // ADC reference voltage (5V for Uno)
    
    // Voltage Divider Configuration
    // Formula: Vout = VBatt × (R2 / (R1 + R2))
    // For 9V battery with 1:1 divider: Vout = 4.5V (safe for 5V ADC)
    constexpr float R1 = 10000.0f;               // High-side resistor (Ω)
    constexpr float R2 = 10000.0f;               // Low-side resistor (Ω)
    
    // ==================== SAMPLING CONFIGURATION ====================
    
    // Moving average filter parameters
    constexpr uint8_t SAMPLES = 10;              // Number of samples for averaging
    constexpr uint16_t SAMPLE_INTERVAL_MS = 100; // Delay between samples (ms)
    
    // Note: Total sampling time = SAMPLES × SAMPLE_INTERVAL_MS
    // Example: 10 samples × 100ms = 1 second per reading
    
    // ==================== LED FEEDBACK CONFIGURATION ====================
    
    // LED Pin assignments (Digital pins)
    constexpr uint8_t LED_FULL = 2;              // Green LED for FULL state (D2)
    constexpr uint8_t LED_GOOD = 3;              // Yellow LED for GOOD state (D3)
    constexpr uint8_t LED_LOW = 4;               // Orange LED for LOW state (D4)
    constexpr uint8_t LED_CRITICAL = 5;          // Red LED for CRITICAL state (D5)
    
    // Blink timing (milliseconds)
    constexpr uint16_t BLINK_SLOW = 1000;        // Slow blink period: 1Hz (LOW state)
    constexpr uint16_t BLINK_FAST = 333;         // Fast blink period: 3Hz (CRITICAL state)
    
    // LED update rate for smooth blinking
    constexpr uint16_t LED_UPDATE_MS = 50;       // Update LEDs every 50ms
    
    // ==================== TIMING CONFIGURATION ====================
    
    // Main update intervals
    constexpr uint16_t UPDATE_INTERVAL_MS = 2000;    // Battery check interval (2 seconds)
    constexpr uint16_t SERIAL_PRINT_INTERVAL_MS = 2000; // Serial output interval
    
    // Warning intervals
    constexpr uint16_t CRITICAL_WARNING_INTERVAL_MS = 10000; // Warning every 10s when critical
    
    // ==================== SIMULATION CONFIGURATION ====================
    // (Used only when SIMULATE_BATTERY_DISCHARGE is defined)
    
    constexpr float SIM_INITIAL_VOLTAGE = NOMINAL_VOLTAGE;  // Starting voltage
    constexpr float SIM_FINAL_VOLTAGE = CRITICAL_THRESHOLD - 0.5f; // End voltage
    constexpr uint32_t SIM_DISCHARGE_TIME_MS = 45000;      // 45 seconds for demo
    constexpr bool SIM_AUTO_RESTART = true;                // Auto-recharge after discharge
    constexpr bool SIM_REALISTIC_CURVE = false;            // Use realistic Li-Ion curve
    
    // ==================== DEBUG CONFIGURATION ====================
    // (Used only when DEBUG_RESOURCES is defined)
    
    constexpr uint32_t RESOURCE_REPORT_INTERVAL_MS = 30000; // Report every 30 seconds
    constexpr uint16_t RAM_WARNING_THRESHOLD = 200;         // Warn if free RAM < 200 bytes
    constexpr uint8_t RAM_CRITICAL_PERCENT = 90;            // Critical at 90% usage
    
} // namespace BatteryConfig

// ==================== COMPILE-TIME VALIDATION ====================

// Static assertions to catch configuration errors at compile time
static_assert(BatteryConfig::NOMINAL_VOLTAGE > BatteryConfig::FULL_THRESHOLD,
              "NOMINAL_VOLTAGE must be greater than FULL_THRESHOLD");
              
static_assert(BatteryConfig::FULL_THRESHOLD > BatteryConfig::GOOD_THRESHOLD,
              "FULL_THRESHOLD must be greater than GOOD_THRESHOLD");
              
static_assert(BatteryConfig::GOOD_THRESHOLD > BatteryConfig::LOW_THRESHOLD,
              "GOOD_THRESHOLD must be greater than LOW_THRESHOLD");
              
static_assert(BatteryConfig::LOW_THRESHOLD > BatteryConfig::CRITICAL_THRESHOLD,
              "LOW_THRESHOLD must be greater than CRITICAL_THRESHOLD");
              
static_assert(BatteryConfig::CRITICAL_THRESHOLD > 0,
              "CRITICAL_THRESHOLD must be positive");
              
static_assert(BatteryConfig::SAMPLES > 0 && BatteryConfig::SAMPLES <= 50,
              "SAMPLES must be between 1 and 50");
              
static_assert(BatteryConfig::R1 > 0 && BatteryConfig::R2 > 0,
              "Voltage divider resistors must be positive");

// Calculate expected ADC voltage and verify it's safe
namespace BatteryConfigValidation {
    constexpr float maxVoltage = BatteryConfig::NOMINAL_VOLTAGE;
    constexpr float dividerRatio = BatteryConfig::R2 / (BatteryConfig::R1 + BatteryConfig::R2);
    constexpr float maxAdcVoltage = maxVoltage * dividerRatio;
    
    static_assert(maxAdcVoltage <= BatteryConfig::VREF,
                  "Voltage divider output exceeds ADC reference voltage! Risk of damage!");
}

#endif // BATTERY_CONFIG_H
```

### include/BatteryState.h

```cpp
/**
 * @file BatteryState.h
 * @brief Battery state enumeration and utility functions
 * 
 * INSTRUCTIONS FOR COPILOT:
 * - Define BatteryState as strongly-typed enum class
 * - Provide utility functions for state-to-string conversion
 * - Keep all functions inline for zero overhead
 * - Use const char* instead of String to save RAM
 */

#ifndef BATTERY_STATE_H
#define BATTERY_STATE_H

#include <Arduino.h>

// ==================== BATTERY STATE ENUM ====================

/**
 * @brief Battery charge state enumeration
 * 
 * States represent battery charge level:
 * - FULL: 75-100% charge, green LED steady
 * - GOOD: 50-75% charge, yellow LED steady
 * - LOW: 25-50% charge, orange LED slow blink (warning)
 * - CRITICAL: 0-25% charge, red LED fast blink (urgent)
 * - UNKNOWN: Initial state before first reading
 */
enum class BatteryState : uint8_t {
    FULL = 0,       // Battery is full (>= FULL_THRESHOLD)
    GOOD = 1,       // Battery is good (>= GOOD_THRESHOLD)
    LOW = 2,        // Battery is low (>= LOW_THRESHOLD) - WARNING
    CRITICAL = 3,   // Battery is critical (< LOW_THRESHOLD) - URGENT
    UNKNOWN = 255   // State not yet determined (initial state)
};

// ==================== UTILITY FUNCTIONS ====================

/**
 * @brief Convert battery state to human-readable string
 * @param state Battery state enum value
 * @return Pointer to static string (no heap allocation)
 * 
 * Example: batteryStateToString(BatteryState::FULL) returns "FULL"
 */
inline const char* batteryStateToString(BatteryState state) {
    switch (state) {
        case BatteryState::FULL:     return "FULL";
        case BatteryState::GOOD:     return "GOOD";
        case BatteryState::LOW:      return "LOW";
        case BatteryState::CRITICAL: return "CRITICAL";
        case BatteryState::UNKNOWN:  return "UNKNOWN";
        default:                     return "INVALID";
    }
}

/**
 * @brief Convert battery state to display string (Italian)
 * @param state Battery state enum value
 * @return Pointer to static string for display
 * 
 * Example: batteryStateToDisplayString(BatteryState::FULL) returns "Piena"
 */
inline const char* batteryStateToDisplayString(BatteryState state) {
    switch (state) {
        case BatteryState::FULL:     return "Piena";
        case BatteryState::GOOD:     return "Buona";
        case BatteryState::LOW:      return "Bassa";
        case BatteryState::CRITICAL: return "Critica";
        case BatteryState::UNKNOWN:  return "---";
        default:                     return "???";
    }
}

/**
 * @brief Get ASCII art icon for battery state (serial monitor)
 * @param state Battery state enum value
 * @return Pointer to static ASCII art string
 * 
 * Visual representation:
 * FULL:     [====] (100%)
 * GOOD:     [=== ] (75%)
 * LOW:      [==  ] (50%)
 * CRITICAL: [=   ] (25%)
 */
inline const char* getIconForState(BatteryState state) {
    switch (state) {
        case BatteryState::FULL:     return "[====]";
        case BatteryState::GOOD:     return "[=== ]";
        case BatteryState::LOW:      return "[==  ]";
        case BatteryState::CRITICAL: return "[=   ]";
        case BatteryState::UNKNOWN:  return "[????]";
        default:                     return "[ERR ]";
    }
}

/**
 * @brief Get single character code for state (compact display)
 * @param state Battery state enum value
 * @return Single character representing state
 * 
 * Codes: F=Full, G=Good, L=Low, C=Critical, ?=Unknown
 */
inline char getStateCode(BatteryState state) {
    switch (state) {
        case BatteryState::FULL:     return 'F';
        case BatteryState::GOOD:     return 'G';
        case BatteryState::LOW:      return 'L';
        case BatteryState::CRITICAL: return 'C';
        case BatteryState::UNKNOWN:  return '?';
        default:                     return 'X';
    }
}

/**
 * @brief Check if state requires user attention
 * @param state Battery state enum value
 * @return true if state is LOW or CRITICAL
 */
inline bool stateNeedsAttention(BatteryState state) {
    return (state == BatteryState::LOW || state == BatteryState::CRITICAL);
}

/**
 * @brief Check if state is critical (urgent)
 * @param state Battery state enum value
 * @return true if state is CRITICAL
 */
inline bool stateIsCritical(BatteryState state) {
    return (state == BatteryState::CRITICAL);
}

/**
 * @brief Get numeric priority level (0=best, 3=worst)
 * @param state Battery state enum value
 * @return Priority level (0-3, or 255 for unknown)
 */
inline uint8_t getStatePriority(BatteryState state) {
    return static_cast<uint8_t>(state);
}

#endif // BATTERY_STATE_H
```

### include/BatteryMonitor.h

```cpp
/**
 * @file BatteryMonitor.h
 * @brief Main battery monitoring class with ADC reading and state calculation
 * 
 * INSTRUCTIONS FOR COPILOT:
 * - Implement BatteryMonitor class with all required methods
 * - Use const correctness: mark read-only methods as const
 * - No dynamic allocation: all members on stack
 * - Use private methods for internal logic
 * - Apply moving average filter for noise reduction
 * - Implement hysteresis for stable state transitions
 * - Calculate percentage linearly from voltage range
 */

#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <Arduino.h>
#include "BatteryConfig.h"
#include "BatteryState.h"

/**
 * @class BatteryMonitor
 * @brief Monitors battery voltage and calculates charge state
 * 
 * RESPONSIBILITIES:
 * - Read ADC voltage through voltage divider
 * - Apply moving average filter (noise reduction)
 * - Calculate battery state with hysteresis
 * - Calculate battery percentage (0-100%)
 * - Track voltage changes and state transitions
 * 
 * USAGE:
 * @code
 * BatteryMonitor battery;
 * 
 * void setup() {
 *     battery.begin();
 * }
 * 
 * void loop() {
 *     battery.update();
 *     float voltage = battery.getVoltage();
 *     BatteryState state = battery.getState();
 *     uint8_t percent = battery.getPercentage();
 * }
 * @endcode
 */
class BatteryMonitor {
public:
    /**
     * @brief Constructor - initializes to unknown state
     */
    BatteryMonitor() 
        : currentVoltage_(0.0f),
          previousVoltage_(0.0f),
          state_(BatteryState::UNKNOWN),
          previousState_(BatteryState::UNKNOWN) {}
    
    /**
     * @brief Initialize the battery monitor
     * 
     * Call this in setup() before using the monitor.
     * Configures ADC pin and performs initial reading.
     */
    void begin() {
        pinMode(BatteryConfig::ANALOG_PIN, INPUT);
        update(); // Perform initial reading
    }
    
    /**
     * @brief Update battery voltage and state
     * 
     * Call this periodically (e.g., every 2 seconds).
     * Reads filtered voltage and recalculates state.
     * 
     * Note: This is a blocking call that takes ~1 second
     * due to moving average sampling (10 samples × 100ms).
     */
    void update() {
        previousVoltage_ = currentVoltage_;
        previousState_ = state_;
        
        currentVoltage_ = readFilteredVoltage();
        state_ = calculateState();
    }
    
    // ==================== GETTERS ====================
    
    /**
     * @brief Get current battery voltage
     * @return Voltage in volts (float)
     */
    float getVoltage() const { 
        return currentVoltage_; 
    }
    
    /**
     * @brief Get current battery state
     * @return BatteryState enum (FULL/GOOD/LOW/CRITICAL)
     */
    BatteryState getState() const { 
        return state_; 
    }
    
    /**
     * @brief Get battery charge percentage
     * @return Percentage (0-100)
     * 
     * Calculated linearly:
     * 100% at NOMINAL_VOLTAGE
     * 0% at CRITICAL_THRESHOLD
     */
    uint8_t getPercentage() const {
        return calculatePercentage();
    }
    
    /**
     * @brief Check if state changed since last update
     * @return true if state transitioned
     */
    bool stateChanged() const {
        return state_ != previousState_;
    }
    
    /**
     * @brief Get previous voltage reading
     * @return Previous voltage in volts
     */
    float getPreviousVoltage() const {
        return previousVoltage_;
    }
    
    /**
     * @brief Get previous battery state
     * @return Previous BatteryState enum
     */
    BatteryState getPreviousState() const {
        return previousState_;
    }
    
    // ==================== CONVENIENCE CHECKS ====================
    
    /**
     * @brief Check if battery is full
     * @return true if state is FULL
     */
    bool isFull() const { 
        return state_ == BatteryState::FULL; 
    }
    
    /**
     * @brief Check if battery is good
     * @return true if state is GOOD
     */
    bool isGood() const { 
        return state_ == BatteryState::GOOD; 
    }
    
    /**
     * @brief Check if battery is low
     * @return true if state is LOW
     */
    bool isLow() const { 
        return state_ == BatteryState::LOW; 
    }
    
    /**
     * @brief Check if battery is critical
     * @return true if state is CRITICAL
     */
    bool isCritical() const { 
        return state_ == BatteryState::CRITICAL; 
    }
    
    /**
     * @brief Check if battery needs recharging
     * @return true if LOW or CRITICAL
     */
    bool needsCharge() const {
        return state_ == BatteryState::LOW || state_ == BatteryState::CRITICAL;
    }
    
    // ==================== ANALYTICS ====================
    
    /**
     * @brief Calculate voltage drop rate
     * @return Voltage drop in V/s (positive = decreasing)
     * 
     * Useful for estimating remaining time
     * Example: 0.01 V/s means 1V drop every 100 seconds
     */
    float getVoltageDrop() const {
        const float deltaV = previousVoltage_ - currentVoltage_;
        const float deltaT = BatteryConfig::UPDATE_INTERVAL_MS / 1000.0f;
        return deltaV / deltaT;
    }
    
    /**
     * @brief Estimate time to critical (rough approximation)
     * @return Estimated seconds until CRITICAL, or 0 if already critical
     * 
     * Note: Assumes linear discharge (not accurate for real batteries)
     */
    uint32_t estimateTimeToCritical() const {
        if (isCritical()) return 0;
        
        const float dropRate = getVoltageDrop();
        if (dropRate <= 0.0f) return UINT32_MAX; // Not discharging or charging
        
        const float voltageRemaining = currentVoltage_ - BatteryConfig::CRITICAL_THRESHOLD;
        return static_cast<uint32_t>(voltageRemaining / dropRate);
    }

private:
    // ==================== MEMBER VARIABLES ====================
    
    float currentVoltage_;          ///< Current battery voltage (V)
    float previousVoltage_;         ///< Previous battery voltage (V)
    BatteryState state_;            ///< Current battery state
    BatteryState previousState_;    ///< Previous battery state
    
    // ==================== PRIVATE METHODS ====================
    
    /**
     * @brief Read single ADC sample and convert to battery voltage
     * @return Instantaneous battery voltage (V)
     * 
     * Algorithm:
     * 1. Read ADC (0-1023)
     * 2. Convert to voltage: vOut = (ADC / 1023) × VREF
     * 3. Reverse divider: vBatt = vOut × ((R1 + R2) / R2)
     */
    float readVoltage() const {
        const uint16_t adcValue = analogRead(BatteryConfig::ANALOG_PIN);
        const float vOut = (adcValue / 1023.0f) * BatteryConfig::VREF;
        
        // Reverse voltage divider calculation
        constexpr float multiplier = (BatteryConfig::R1 + BatteryConfig::R2) / BatteryConfig::R2;
        return vOut * multiplier;
    }
    
    /**
     * @brief Read multiple samples and return average (noise filtering)
     * @return Filtered battery voltage (V)
     * 
     * Uses moving average filter with N samples.
     * Total time: SAMPLES × SAMPLE_INTERVAL_MS (~1 second)
     * 
     * This is a BLOCKING call!
     */
    float readFilteredVoltage() const {
        float sum = 0.0f;
        
        for (uint8_t i = 0; i < BatteryConfig::SAMPLES; ++i) {
            sum += readVoltage();
            
            // Don't delay after last sample
            if (i < BatteryConfig::SAMPLES - 1) {
                delay(BatteryConfig::SAMPLE_INTERVAL_MS);
            }
        }
        
        return sum / BatteryConfig::SAMPLES;
    }
    
    /**
     * @brief Calculate battery state from voltage with hysteresis
     * @return Calculated BatteryState
     * 
     * Hysteresis: Add offset when voltage is INCREASING
     * This prevents rapid state oscillations near thresholds
     * 
     * Example: With 50mV hysteresis at 8.0V threshold:
     * - Decreasing: 8.01V→7.99V triggers GOOD→LOW
     * - Increasing: 7.99V→8.01V stays LOW, 8.05V triggers LOW→GOOD
     */
    BatteryState calculateState() const {
        // Apply hysteresis only when voltage is increasing
        const float hysteresis = (currentVoltage_ > previousVoltage_) 
                                ? BatteryConfig::HYSTERESIS 
                                : 0.0f;
        
        // Compare with thresholds (must be in descending order)
        if (currentVoltage_ >= BatteryConfig::FULL_THRESHOLD + hysteresis) {
            return BatteryState::FULL;
        } else if (currentVoltage_ >= BatteryConfig::GOOD_THRESHOLD + hysteresis) {
            return BatteryState::GOOD;
        } else if (currentVoltage_ >= BatteryConfig::LOW_THRESHOLD + hysteresis) {
            return BatteryState::LOW;
        } else {
            return BatteryState::CRITICAL;
        }
    }
    
    /**
     * @brief Calculate battery percentage (0-100%)
     * @return Percentage as uint8_t
     * 
     * Linear calculation:
     * percentage = ((V - V_critical) / (V_nominal - V_critical)) × 100
     * 
     * Example with 9V battery (critical=7V):
     * - 9.0V → 100%
     * - 8.0V → 50%
     * - 7.0V → 0%
     * - 6.5V → 0% (clamped)
     */
    uint8_t calculatePercentage() const {
        constexpr float range = BatteryConfig::NOMINAL_VOLTAGE - BatteryConfig::CRITICAL_THRESHOLD;
        const float current = currentVoltage_ - BatteryConfig::CRITICAL_THRESHOLD;
        
        const float percentage = (current / range) * 100.0f;
        
        // Clamp to 0-100 range
        return constrain(static_cast<uint8_t>(percentage), 0, 100);
    }
};

#endif // BATTERY_MONITOR_H
```

### include/BatteryFeedback.h

```cpp
/**
 * @file BatteryFeedback.h
 * @brief LED feedback controller for visual battery status indication
 * 
 * INSTRUCTIONS FOR COPILOT:
 * - Control 4 LEDs based on battery state
 * - Implement non-blocking blink patterns using millis()
 * - FULL: Green LED steady
 * - GOOD: Yellow LED steady
 * - LOW: Orange LED slow blink (1Hz)
 * - CRITICAL: Red LED fast blink (3Hz)
 * - Provide boot animation for visual feedback
 */

#ifndef BATTERY_FEEDBACK_H
#define BATTERY_FEEDBACK_H

#include <Arduino.h>
#include "BatteryConfig.h"
#include "BatteryState.h"

/**
 * @class BatteryFeedback
 * @brief Controls LED feedback for battery status
 * 
 * RESPONSIBILITIES:
 * - Map battery state to appropriate LED
 * - Implement blink patterns (steady/slow/fast)
 * - Non-blocking LED updates (call update() in loop)
 * - Boot animation for startup indication
 * 
 * LED MAPPING:
 * - FULL: Green (D2) - Steady ON
 * - GOOD: Yellow (D3) - Steady ON
 * - LOW: Orange (D4) - Blink 1Hz (on 500ms, off 500ms)
 * - CRITICAL: Red (D5) - Blink 3Hz (on 167ms, off 167ms)
 * 
 * USAGE:
 * @code
 * BatteryFeedback feedback;
 * 
 * void setup() {
 *     feedback.begin();
 *     feedback.bootAnimation(); // Optional
 * }
 * 
 * void loop() {
 *     feedback.update(battery.getState()); // Call frequently
 * }
 * @endcode
 */
class BatteryFeedback {
public:
    /**
     * @brief Constructor
     */
    BatteryFeedback() 
        : lastBlinkUpdate_(0),
          blinkState_(false) {}
    
    /**
     * @brief Initialize LED pins
     * 
     * Call this in setup() before using feedback.
     * Configures all LED pins as OUTPUT and turns them off.
     */
    void begin() {
        pinMode(BatteryConfig::LED_FULL, OUTPUT);
        pinMode(BatteryConfig::LED_GOOD, OUTPUT);
        pinMode(BatteryConfig::LED_LOW, OUTPUT);
        pinMode(BatteryConfig::LED_CRITICAL, OUTPUT);
        
        turnAllOff();
    }
    
    /**
     * @brief Update LED display based on battery state
     * @param state Current battery state
     * 
     * Call this frequently in loop() for smooth blinking.
     * This is non-blocking and uses millis() for timing.
     * 
     * LED behavior:
     * - FULL/GOOD: Steady ON
     * - LOW: Slow blink (1Hz)
     * - CRITICAL: Fast blink (3Hz)
     */
    void update(BatteryState state) {
        // Update internal blink timing
        updateBlinkState();
        
        // Turn all LEDs off first
        turnAllOff();
        
        // Turn on appropriate LED(s) based on state
        switch (state) {
            case BatteryState::FULL:
                // Green LED steady
                digitalWrite(BatteryConfig::LED_FULL, HIGH);
                break;
                
            case BatteryState::GOOD:
                // Yellow LED steady
                digitalWrite(BatteryConfig::LED_GOOD, HIGH);
                break;
                
            case BatteryState::LOW:
                // Orange LED slow blink (1Hz)
                digitalWrite(BatteryConfig::LED_LOW, 
                            getBlinkState(BatteryConfig::BLINK_SLOW) ? HIGH : LOW);
                break;
                
            case BatteryState::CRITICAL:
                // Red LED fast blink (3Hz)
                digitalWrite(BatteryConfig::LED_CRITICAL, 
                            getBlinkState(BatteryConfig::BLINK_FAST) ? HIGH : LOW);
                break;
                
            case BatteryState::UNKNOWN:
            default:
                // All LEDs off (error indication)
                break;
        }
    }
    
    /**
     * @brief Show startup animation
     * 
     * Sequential LED animation for visual feedback on boot.
     * Each LED lights up for 150ms in sequence, then all turn off.
     * 
     * Total duration: ~750ms (blocking)
     */
    void bootAnimation() {
        const uint16_t delayMs = 150;
        
        // Light up LEDs sequentially
        digitalWrite(BatteryConfig::LED_FULL, HIGH);
        delay(delayMs);
        
        digitalWrite(BatteryConfig::LED_GOOD, HIGH);
        delay(delayMs);
        
        digitalWrite(BatteryConfig::LED_LOW, HIGH);
        delay(delayMs);
        
        digitalWrite(BatteryConfig::LED_CRITICAL, HIGH);
        delay(delayMs);
        
        // Turn all off
        turnAllOff();
        delay(delayMs);
    }
    
    /**
     * @brief Show all LEDs on (test mode)
     * 
     * Useful for testing LED connections.
     */
    void testAllOn() {
        digitalWrite(BatteryConfig::LED_FULL, HIGH);
        digitalWrite(BatteryConfig::LED_GOOD, HIGH);
        digitalWrite(BatteryConfig::LED_LOW, HIGH);
        digitalWrite(BatteryConfig::LED_CRITICAL, HIGH);
    }
    
    /**
     * @brief Blink all LEDs (error indication)
     * @param times Number of blinks
     * @param delayMs Delay between blinks
     * 
     * Synchronous blink pattern for error signaling.
     */
    void blinkAllError(uint8_t times, uint16_t delayMs = 200) {
        for (uint8_t i = 0; i < times; ++i) {
            testAllOn();
            delay(delayMs);
            turnAllOff();
            delay(delayMs);
        }
    }

private:
    // ==================== MEMBER VARIABLES ====================
    
    unsigned long lastBlinkUpdate_;  ///< Last blink state change time
    bool blinkState_;                ///< Current blink state (for tracking)
    
    // ==================== PRIVATE METHODS ====================
    
    /**
     * @brief Turn all LEDs off
     * 
     * Called before updating to specific state
     */
    void turnAllOff() {
        digitalWrite(BatteryConfig::LED_FULL, LOW);
        digitalWrite(BatteryConfig::LED_GOOD, LOW);
        digitalWrite(BatteryConfig::LED_LOW, LOW);
        digitalWrite(BatteryConfig::LED_CRITICAL, LOW);
    }
    
    /**
     * @brief Update internal blink state tracking
     * 
     * Called periodically to track blink timing.
     * Uses LED_UPDATE_MS interval.
     */
    void updateBlinkState() {
        const unsigned long now = millis();
        
        if (now - lastBlinkUpdate_ >= BatteryConfig::LED_UPDATE_MS) {
            lastBlinkUpdate_ = now;
            blinkState_ = !blinkState_; // Toggle for tracking
        }
    }
    
    /**
     * @brief Get blink state for given period
     * @param period Blink period in milliseconds
     * @return true if LED should be ON, false if OFF
     * 
     * Uses millis() modulo for non-blocking operation.
     * LED is ON for first half of period, OFF for second half.
     * 
     * Example with 1000ms period:
     * - 0-499ms: true (ON)
     * - 500-999ms: false (OFF)
     * - 1000ms: wraps to 0 (ON again)
     */
    bool getBlinkState(uint16_t period) const {
        const unsigned long now = millis();
        const unsigned long phase = now % period;
        return phase < (period / 2);
    }
};

#endif // BATTERY_FEEDBACK_H
```

### include/BatterySimulator.h

```cpp
/**
 * @file BatterySimulator.h
 * @brief Battery discharge simulator for testing without hardware
 * 
 * INSTRUCTIONS FOR COPILOT:
 * - Only include this when SIMULATE_BATTERY_DISCHARGE is defined
 * - Override analogRead() to return simulated ADC values
 * - Implement linear discharge from initial to final voltage
 * - Optional: realistic Li-Ion discharge curve
 * - Auto-restart after complete discharge
 * - Print simulation status to serial
 */

#ifndef BATTERY_SIMULATOR_H
#define BATTERY_SIMULATOR_H

// Only compile when simulation is enabled
#ifdef SIMULATE_BATTERY_DISCHARGE

#include <Arduino.h>
#include "BatteryConfig.h"

/**
 * @class BatterySimulator
 * @brief Simulates battery discharge for testing
 * 
 * FEATURES:
 * - Linear voltage decay over time
 * - Optional realistic Li-Ion discharge curve
 * - Auto-restart (recharge) mode
 * - Overrides analogRead() transparently
 * 
 * USAGE:
 * 1. Define SIMULATE_BATTERY_DISCHARGE in platformio.ini
 * 2. Create global instance: BatterySimulator g_batterySimulator;
 * 3. Call begin() in setup()
 * 4. BatteryMonitor automatically reads simulated values
 * 
 * @code
 * #ifdef SIMULATE_BATTERY_DISCHARGE
 * BatterySimulator::Config simConfig;
 * simConfig.initialVoltage = 9.0f;
 * simConfig.finalVoltage = 6.5f;
 * simConfig.dischargeTimeMs = 30000; // 30 seconds
 * 
 * BatterySimulator g_batterySimulator(simConfig);
 * #endif
 * @endcode
 */
class BatterySimulator {
public:
    /**
     * @brief Simulator configuration
     */
    struct Config {
        float initialVoltage = BatteryConfig::NOMINAL_VOLTAGE;          ///< Start voltage
        float finalVoltage = BatteryConfig::CRITICAL_THRESHOLD - 0.5f;  ///< End voltage
        uint32_t dischargeTimeMs = 45000;                               ///< Discharge duration
        bool autoRestart = true;                                        ///< Auto-recharge after discharge
        bool useRealisticCurve = false;                                 ///< Use Li-Ion curve vs linear
    };
    
    /**
     * @brief Constructor with configuration
     * @param cfg Simulator configuration
     */
    BatterySimulator(const Config& cfg = Config())
        : config_(cfg),
          currentVoltage_(cfg.initialVoltage),
          startTime_(0),
          isActive_(false) {}
    
    /**
     * @brief Initialize simulator
     * 
     * Call this in setup() to start simulation.
     * Prints configuration to serial.
     */
    void begin() {
        startTime_ = millis();
        currentVoltage_ = config_.initialVoltage;
        isActive_ = true;
        
        Serial.println(F("========================================"));
        Serial.println(F("  BATTERY SIMULATOR ACTIVE"));
        Serial.println(F("========================================"));
        Serial.print(F("Initial Voltage: ")); 
        Serial.print(config_.initialVoltage, 2); 
        Serial.println(F("V"));
        
        Serial.print(F("Final Voltage: ")); 
        Serial.print(config_.finalVoltage, 2); 
        Serial.println(F("V"));
        
        Serial.print(F("Discharge Time: ")); 
        Serial.print(config_.dischargeTimeMs / 1000); 
        Serial.println(F("s"));
        
        Serial.print(F("Curve Type: ")); 
        Serial.println(config_.useRealisticCurve ? F("Realistic") : F("Linear"));
        
        Serial.print(F("Auto-Restart: ")); 
        Serial.println(config_.autoRestart ? F("Yes") : F("No"));
        
        Serial.println(F("========================================\n"));
    }
    
    /**
     * @brief Update simulated voltage
     * 
     * Called automatically by overridden analogRead().
     * Calculates voltage based on elapsed time.
     */
    void update() {
        if (!isActive_) return;
        
        const uint32_t elapsed = millis() - startTime_;
        
        // Check if discharge complete
        if (elapsed >= config_.dischargeTimeMs) {
            currentVoltage_ = config_.finalVoltage;
            isActive_ = false;
            
            // Auto-restart if enabled
            if (config_.autoRestart) {
                Serial.println(F("\n[SIMULATOR] Battery depleted - Recharging...\n"));
                delay(2000);  // Pause before restart
                begin();      // Restart simulation
            } else {
                Serial.println(F("\n[SIMULATOR] Battery depleted - Simulation ended\n"));
            }
        } else {
            // Calculate voltage based on progress
            const float progress = static_cast<float>(elapsed) / config_.dischargeTimeMs;
            
            if (config_.useRealisticCurve) {
                currentVoltage_ = calculateRealisticVoltage(progress);
            } else {
                // Linear discharge
                const float voltageRange = config_.initialVoltage - config_.finalVoltage;
                currentVoltage_ = config_.initialVoltage - (voltageRange * progress);
            }
        }
    }
    
    /**
     * @brief Get simulated ADC value
     * @return ADC counts (0-1023)
     * 
     * Converts current voltage to ADC value through voltage divider.
     */
    uint16_t getSimulatedADC() const {
        // Calculate voltage after divider
        const float vDivider = currentVoltage_ * 
            (BatteryConfig::R2 / (BatteryConfig::R1 + BatteryConfig::R2));
        
        // Convert to ADC counts
        const uint16_t adcValue = static_cast<uint16_t>(
            (vDivider / BatteryConfig::VREF) * 1023.0f
        );
        
        return constrain(adcValue, 0, 1023);
    }
    
    /**
     * @brief Get current simulated voltage
     * @return Voltage in volts
     */
    float getCurrentVoltage() const {
        return currentVoltage_;
    }
    
    /**
     * @brief Get discharge progress
     * @return Progress (0.0 = start, 1.0 = end)
     */
    float getProgress() const {
        const uint32_t elapsed = millis() - startTime_;
        const float progress = static_cast<float>(elapsed) / config_.dischargeTimeMs;
        return constrain(progress, 0.0f, 1.0f);
    }
    
    /**
     * @brief Check if simulation is active
     * @return true if currently discharging
     */
    bool isActive() const {
        return isActive_;
    }

private:
    Config config_;              ///< Simulator configuration
    float currentVoltage_;       ///< Current simulated voltage
    uint32_t startTime_;         ///< Simulation start time
    bool isActive_;              ///< Simulation active flag
    
    /**
     * @brief Calculate realistic Li-Ion discharge curve
     * @param progress Discharge progress (0.0-1.0)
     * @return Voltage at given progress
     * 
     * Li-Ion discharge characteristics:
     * - High plateau (80-100%): Nearly flat voltage
     * - Linear region (20-80%): Gradual decline
     * - Sharp drop (0-20%): Rapid voltage decrease
     * 
     * This is a piecewise linear approximation.
     */
    float calculateRealisticVoltage(float progress) const {
        // Invert progress (1.0 = full, 0.0 = empty)
        const float percentage = (1.0f - progress) * 100.0f;
        const float range = config_.initialVoltage - config_.finalVoltage;
        
        float normalizedV;
        
        if (percentage > 80.0f) {
            // High plateau: 95-100% of voltage range
            normalizedV = 0.95f + (percentage - 80.0f) * 0.0025f;
        } else if (percentage > 20.0f) {
            // Linear region: 35-95% of voltage range
            normalizedV = 0.35f + (percentage - 20.0f) * 0.01f;
        } else {
            // Sharp drop: 0-35% of voltage range
            normalizedV = percentage * 0.0175f;
        }
        
        return config_.finalVoltage + (range * normalizedV);
    }
};

// ==================== GLOBAL SIMULATOR INSTANCE ====================
// Must be defined in main.cpp
extern BatterySimulator g_batterySimulator;

// ==================== ANALOGREAD OVERRIDE ====================

/**
 * @brief Override Arduino analogRead() function
 * @param pin Analog pin number
 * @return Simulated ADC value for battery pin, 0 for other pins
 * 
 * This macro overrides the built-in analogRead() to return
 * simulated values when reading from the battery monitoring pin.
 * 
 * Note: This is done via #undef and inline function to maintain
 * compatibility with other analogRead() calls in the codebase.
 */
#undef analogRead
inline uint16_t analogRead(uint8_t pin) {
    if (pin == BatteryConfig::ANALOG_PIN) {
        // Update simulator and return simulated value
        g_batterySimulator.update();
        const uint16_t adc = g_batterySimulator.getSimulatedADC();
        
        // Debug output
        Serial.print(F("[SIM] V: "));
        Serial.print(g_batterySimulator.getCurrentVoltage(), 2);
        Serial.print(F("V → ADC: "));
        Serial.print(adc);
        Serial.print(F(" ("));
        Serial.print(g_batterySimulator.getProgress() * 100.0f, 0);
        Serial.println(F("%)"));
        
        return adc;
    }
    
    // For other pins, return 0 (no simulation)
    return 0;
}

#endif // SIMULATE_BATTERY_DISCHARGE
#endif // BATTERY_SIMULATOR_H
```

### include/ResourceMonitor.h

```cpp
/**
 * @file ResourceMonitor.h
 * @brief System resource monitoring for Arduino debugging
 * 
 * INSTRUCTIONS FOR COPILOT:
 * - Only include when DEBUG_RESOURCES is defined
 * - Track SRAM usage (stack + heap + free)
 * - Track loop timing (min/max/avg)
 * - Detect memory leaks (RAM growth over time)
 * - Print formatted reports with ASCII bar graphs
 * - Provide warning messages for critical conditions
 * 
 * COMPONENTS:
 * - MemoryInfo: Static RAM monitoring
 * - TimingInfo: Loop performance tracking
 * - ResourceMonitor: Main monitoring class
 */

#ifndef RESOURCE_MONITOR_H
#define RESOURCE_MONITOR_H

// Only compile when debug resources enabled
#ifdef DEBUG_RESOURCES

#include <Arduino.h>

// ==================== MEMORY INFO ====================

/**
 * @class MemoryInfo
 * @brief Static class for SRAM monitoring
 * 
 * Provides functions to query SRAM usage on AVR microcontrollers.
 * All methods are static (no instantiation needed).
 */
class MemoryInfo {
public:
    /**
     * @brief Memory report structure
     */
    struct Report {
        uint16_t totalRAM;          ///< Total SRAM available (bytes)
        uint16_t freeRAM;           ///< Current free RAM (bytes)
        uint16_t usedRAM;           ///< Used RAM (bytes)
        uint8_t usagePercent;       ///< RAM usage percentage
        uint16_t stackSize;         ///< Approximate stack size (bytes)
        uint16_t heapSize;          ///< Approximate heap size (bytes)
    };
    
    /**
     * @brief Get current free RAM
     * @return Free RAM in bytes
     * 
     * This is the most important metric for detecting memory issues.
     * 
     * Algorithm:
     * - Calculate space between stack pointer and heap pointer
     * - If no heap, calculate from stack to heap start
     */
    static uint16_t getFreeRAM() {
        extern int __heap_start, *__brkval;
        int v;
        return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    }
    
    /**
     * @brief Get total SRAM size for current platform
     * @return Total SRAM in bytes
     * 
     * Platform detection via preprocessor:
     * - ATmega328P (Uno/Nano): 2048 bytes
     * - ATmega2560 (Mega): 8192 bytes
     * - ATmega32U4 (Leonardo): 2560 bytes
     */
    static constexpr uint16_t getTotalRAM() {
        #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
            return 2048;  // Arduino Uno/Nano
        #elif defined(__AVR_ATmega2560__)
            return 8192;  // Arduino Mega
        #elif defined(__AVR_ATmega32U4__)
            return 2560;  // Arduino Leonardo/Micro
        #else
            return 2048;  // Default assumption
        #endif
    }
    
    /**
     * @brief Get used RAM
     * @return Used RAM in bytes
     */
    static uint16_t getUsedRAM() {
        return getTotalRAM() - getFreeRAM();
    }
    
    /**
     * @brief Get RAM usage percentage
     * @return Usage percentage (0-100)
     */
    static uint8_t getUsagePercent() {
        return (getUsedRAM() * 100UL) / getTotalRAM();
    }
    
    /**
     * @brief Estimate stack size (approximate)
     * @return Approximate stack size in bytes
     * 
     * Note: This is an approximation. Stack size varies during execution.
     */
    static uint16_t getStackSize() {
        extern int __heap_start;
        int stackVar;
        return (int)RAMEND - (int)&stackVar;
    }
    
    /**
     * @brief Estimate heap size
     * @return Heap size in bytes, or 0 if no heap allocated
     */
    static uint16_t getHeapSize() {
        extern int __heap_start, *__brkval;
        if (__brkval == 0) return 0;
        return (int)__brkval - (int)&__heap_start;
    }
    
    /**
     * @brief Get complete memory report
     * @return Report structure with all memory info
     */
    static Report getReport() {
        Report report;
        report.totalRAM = getTotalRAM();
        report.freeRAM = getFreeRAM();
        report.usedRAM = getUsedRAM();
        report.usagePercent = getUsagePercent();
        report.stackSize = getStackSize();
        report.heapSize = getHeapSize();
        return report;
    }
};

// ==================== TIMING INFO ====================

/**
 * @class TimingInfo
 * @brief Loop performance monitoring
 * 
 * Tracks loop execution time statistics:
 * - Minimum loop time (best case)
 * - Maximum loop time (worst case)
 * - Average loop time
 * - Loop frequency (Hz)
 */
class TimingInfo {
public:
    /**
     * @brief Timing statistics structure
     */
    struct Stats {
        uint32_t minLoopTime;       ///< Fastest loop iteration (µs)
        uint32_t maxLoopTime;       ///< Slowest loop iteration (µs)
        uint32_t avgLoopTime;       ///< Average loop time (µs)
        uint32_t totalLoops;        ///< Total loop count
        float loopsPerSecond;       ///< Loop frequency (Hz)
    };
    
    /**
     * @brief Constructor
     */
    TimingInfo() 
        : lastLoopTime_(0),
          minLoopTime_(UINT32_MAX),
          maxLoopTime_(0),
          totalLoopTime_(0),
          loopCount_(0) {}
    
    /**
     * @brief Update timing statistics
     * 
     * Call this at the beginning of every loop() iteration.
     * Measures time since last call.
     */
    void update() {
        const uint32_t now = micros();
        
        if (lastLoopTime_ > 0) {
            const uint32_t elapsed = now - lastLoopTime_;
            
            // Update statistics
            if (elapsed < minLoopTime_) minLoopTime_ = elapsed;
            if (elapsed > maxLoopTime_) maxLoopTime_ = elapsed;
            
            totalLoopTime_ += elapsed;
            loopCount_++;
        }
        
        lastLoopTime_ = now;
    }
    
    /**
     * @brief Get timing statistics
     * @return Stats structure
     */
    Stats getStats() const {
        Stats stats;
        stats.minLoopTime = (minLoopTime_ == UINT32_MAX) ? 0 : minLoopTime_;
        stats.maxLoopTime = maxLoopTime_;
        stats.avgLoopTime = (loopCount_ > 0) ? (totalLoopTime_ / loopCount_) : 0;
        stats.totalLoops = loopCount_;
        
        // Calculate frequency
        if (stats.avgLoopTime > 0) {
            stats.loopsPerSecond = 1000000.0f / stats.avgLoopTime;
        } else {
            stats.loopsPerSecond = 0.0f;
        }
        
        return stats;
    }
    
    /**
     * @brief Reset statistics
     */
    void reset() {
        lastLoopTime_ = 0;
        minLoopTime_ = UINT32_MAX;
        maxLoopTime_ = 0;
        totalLoopTime_ = 0;
        loopCount_ = 0;
    }

private:
    uint32_t lastLoopTime_;     ///< Last update timestamp (µs)
    uint32_t minLoopTime_;      ///< Minimum loop time
    uint32_t maxLoopTime_;      ///< Maximum loop time
    uint32_t totalLoopTime_;    ///< Cumulative loop time
    uint32_t loopCount_;        ///< Loop iteration count
};

// ==================== RESOURCE MONITOR ====================

/**
 * @class ResourceMonitor
 * @brief Main resource monitoring class
 * 
 * Combines memory and timing monitoring with reporting.
 * 
 * USAGE:
 * @code
 * ResourceMonitor monitor;
 * 
 * void setup() {
 *     monitor.begin();
 * }
 * 
 * void loop() {
 *     monitor.update();  // Call every iteration
 *     
 *     // Your code here
 *     
 *     monitor.autoReport(30000);  // Report every 30s
 * }
 * @endcode
 */
class ResourceMonitor {
public:
    /**
     * @brief Constructor
     */
    ResourceMonitor() 
        : initialFreeRAM_(MemoryInfo::getFreeRAM()),
          lowestFreeRAM_(initialFreeRAM_),
          lastReportTime_(0) {}
    
    /**
     * @brief Initialize monitor
     * 
     * Call in setup(). Prints initial configuration.
     */
    void begin() {
        Serial.println(F("\n========================================"));
        Serial.println(F("   Resource Monitor Initialized"));
        Serial.println(F("========================================"));
        printFlashInfo();
        Serial.println();
    }
    
    /**
     * @brief Update resource tracking
     * 
     * Call this at the BEGINNING of every loop() iteration.
     * Updates timing and tracks memory high-water mark.
     */
    void update() {
        timing_.update();
        
        // Track lowest free RAM (high-water mark)
        const uint16_t currentFree = MemoryInfo::getFreeRAM();
        if (currentFree < lowestFreeRAM_) {
            lowestFreeRAM_ = currentFree;
        }
    }
    
    /**
     * @brief Print comprehensive resource report
     * 
     * Displays:
     * - Memory usage with bar graph
     * - Initial vs current RAM
     * - Loop timing statistics
     * - Performance classification
     */
    void printReport() {
        Serial.println(F("\n========================================"));
        Serial.println(F("   RESOURCE REPORT"));
        Serial.println(F("========================================"));
        
        // Memory info
        printMemoryReport();
        Serial.println();
        
        // Timing info
        printTimingReport();
        Serial.println();
        
        Serial.println(F("========================================\n"));
        
        lastReportTime_ = millis();
    }
    
    /**
     * @brief Print only critical warnings
     * 
     * Lighter-weight monitoring for production.
     * Only prints if:
     * - Free RAM < 200 bytes
     * - RAM usage > 90%
     */
    void printWarnings() {
        const uint16_t freeRAM = MemoryInfo::getFreeRAM();
        const uint8_t usage = MemoryInfo::getUsagePercent();
        
        constexpr uint16_t MIN_FREE_RAM = 200;
        constexpr uint8_t CRITICAL_PERCENT = 90;
        
        if (freeRAM < MIN_FREE_RAM || usage >= CRITICAL_PERCENT) {
            Serial.println(F("\n!!! CRITICAL: RAM USAGE TOO HIGH !!!"));
            Serial.print(F("Free RAM: ")); 
            Serial.print(freeRAM); 
            Serial.println(F(" bytes"));
            Serial.print(F("Usage: ")); 
            Serial.print(usage); 
            Serial.println(F("%\n"));
        }
    }
    
    /**
     * @brief Auto-report at interval
     * @param intervalMs Report interval in milliseconds
     * 
     * Call this in loop() for periodic reporting.
     * Non-blocking (uses millis()).
     */
    void autoReport(uint32_t intervalMs = 30000) {
        if (millis() - lastReportTime_ >= intervalMs) {
            printReport();
        }
    }
    
    /**
     * @brief Get lowest free RAM observed
     * @return Minimum free RAM in bytes
     */
    uint16_t getLowestFreeRAM() const { 
        return lowestFreeRAM_; 
    }
    
    /**
     * @brief Get initial free RAM (at startup)
     * @return Initial free RAM in bytes
     */
    uint16_t getInitialFreeRAM() const { 
        return initialFreeRAM_; 
    }
    
    /**
     * @brief Get RAM growth since startup
     * @return Bytes consumed (positive = growth/leak)
     */
    uint16_t getRAMGrowth() const { 
        return initialFreeRAM_ - lowestFreeRAM_; 
    }

private:
    uint16_t initialFreeRAM_;       ///< Free RAM at startup
    uint16_t lowestFreeRAM_;        ///< Minimum free RAM observed
    unsigned long lastReportTime_;  ///< Last report timestamp
    TimingInfo timing_;             ///< Timing tracker
    
    /**
     * @brief Print Flash memory info
     */
    void printFlashInfo() {
        Serial.println(F("Flash Memory:"));
        Serial.print(F("  Available: 30720 bytes (Arduino Uno)\n"));
        Serial.println(F("  Note: Run 'pio run -t size' for usage details"));
    }
    
    /**
     * @brief Print memory report with bar graph
     */
    void printMemoryReport() {
        const auto mem = MemoryInfo::getReport();
        
        Serial.println(F("Memory (SRAM):"));
        Serial.print(F("  Total:        ")); 
        Serial.print(mem.totalRAM); 
        Serial.println(F(" bytes"));
        
        Serial.print(F("  Used:         ")); 
        Serial.print(mem.usedRAM); 
        Serial.print(F(" bytes ("));
        Serial.print(mem.usagePercent); 
        Serial.println(F("%)"));
        
        Serial.print(F("  Free:         ")); 
        Serial.print(mem.freeRAM); 
        Serial.println(F(" bytes"));
        Serial.println();
        
        Serial.print(F("  Initial Free: ")); 
        Serial.print(initialFreeRAM_); 
        Serial.println(F(" bytes"));
        
        Serial.print(F("  Lowest Free:  ")); 
        Serial.print(lowestFreeRAM_); 
        Serial.println(F(" bytes"));
        
        Serial.print(F("  Growth:       ")); 
        Serial.print(getRAMGrowth()); 
        Serial.println(F(" bytes"));
        Serial.println();
        
        Serial.print(F("  ~Stack:       ")); 
        Serial.print(mem.stackSize); 
        Serial.println(F(" bytes"));
        
        Serial.print(F("  ~Heap:        ")); 
        Serial.print(mem.heapSize); 
        Serial.println(F(" bytes"));
        
        // ASCII bar graph
        printMemoryBar(mem.usagePercent);
    }
    
    /**
     * @brief Print timing report
     */
    void printTimingReport() {
        const auto stats = timing_.getStats();
        
        Serial.println(F("Loop Timing:"));
        Serial.print(F("  Total Loops:  ")); 
        Serial.println(stats.totalLoops);
        
        Serial.print(F("  Min Time:     ")); 
        Serial.print(stats.minLoopTime); 
        Serial.println(F(" µs"));
        
        Serial.print(F("  Max Time:     ")); 
        Serial.print(stats.maxLoopTime); 
        Serial.println(F(" µs"));
        
        Serial.print(F("  Avg Time:     ")); 
        Serial.print(stats.avgLoopTime); 
        Serial.println(F(" µs"));
        
        Serial.print(F("  Frequency:    ")); 
        Serial.print(stats.loopsPerSecond, 1); 
        Serial.println(F(" Hz"));
        
        // Performance rating
        Serial.print(F("  Performance:  "));
        if (stats.maxLoopTime < 1000) {
            Serial.println(F("Excellent (<1ms)"));
        } else if (stats.maxLoopTime < 10000) {
            Serial.println(F("Good (<10ms)"));
        } else if (stats.maxLoopTime < 50000) {
            Serial.println(F("Fair (<50ms)"));
        } else {
            Serial.println(F("Poor (>50ms) ⚠"));
        }
    }
    
    /**
     * @brief Print ASCII memory usage bar
     * @param percent Usage percentage
     */
    void printMemoryBar(uint8_t percent) {
        Serial.print(F("  Usage Bar:    ["));
        
        constexpr uint8_t barLength = 20;
        const uint8_t filled = (percent * barLength) / 100;
        
        for (uint8_t i = 0; i < barLength; ++i) {
            if (i < filled) {
                Serial.print(F("#"));
            } else {
                Serial.print(F("-"));
            }
        }
        
        Serial.print(F("] "));
        Serial.print(percent);
        Serial.println(F("%"));
    }
};

// ==================== CONVENIENCE MACROS ====================

// Global instance (must be defined in main.cpp)
extern ResourceMonitor g_resourceMonitor;

// Macros for easy usage
#define RESOURCE_MONITOR_BEGIN()     g_resourceMonitor.begin()
#define RESOURCE_MONITOR_UPDATE()    g_resourceMonitor.update()
#define RESOURCE_MONITOR_REPORT()    g_resourceMonitor.printReport()
#define RESOURCE_MONITOR_WARNINGS()  g_resourceMonitor.printWarnings()
#define RESOURCE_MONITOR_AUTO(ms)    g_resourceMonitor.autoReport(ms)

#else
// Stub macros when DEBUG_RESOURCES not defined (zero overhead)
#define RESOURCE_MONITOR_BEGIN()
#define RESOURCE_MONITOR_UPDATE()
#define RESOURCE_MONITOR_REPORT()
#define RESOURCE_MONITOR_WARNINGS()
#define RESOURCE_MONITOR_AUTO(ms)

#endif // DEBUG_RESOURCES
#endif // RESOURCE_MONITOR_H
```

### src/main.cpp

```cpp
/**
 * @file main.cpp
 * @brief Main application - Battery Monitor with 4-stage feedback
 * 
 * INSTRUCTIONS FOR COPILOT:
 * - Initialize all components in setup()
 * - Update battery and feedback in loop()
 * - Print formatted status to serial
 * - Handle state transitions
 * - Warn on critical battery
 * - Support simulation mode (SIMULATE_BATTERY_DISCHARGE)
 * - Support resource monitoring (DEBUG_RESOURCES)
 */

#include <Arduino.h>
#include "BatteryMonitor.h"
#include "BatteryFeedback.h"

// Optional: Simulation mode
#ifdef SIMULATE_BATTERY_DISCHARGE
#include "BatterySimulator.h"

// Configure and instantiate simulator
BatterySimulator::Config simConfig = {
    .initialVoltage = BatteryConfig::SIM_INITIAL_VOLTAGE,
    .finalVoltage = BatteryConfig::SIM_FINAL_VOLTAGE,
    .dischargeTimeMs = BatteryConfig::SIM_DISCHARGE_TIME_MS,
    .autoRestart = BatteryConfig::SIM_AUTO_RESTART,
    .useRealisticCurve = BatteryConfig::SIM_REALISTIC_CURVE
};

BatterySimulator g_batterySimulator(simConfig);
#endif

// Optional: Resource monitoring
#ifdef DEBUG_RESOURCES
#include "ResourceMonitor.h"
ResourceMonitor g_resourceMonitor;
#endif

// ==================== GLOBAL INSTANCES ====================

BatteryMonitor battery;      // Battery voltage monitor
BatteryFeedback feedback;    // LED feedback controller

// ==================== TIMING VARIABLES ====================

unsigned long lastBatteryUpdate = 0;
unsigned long lastSerialPrint = 0;
unsigned long lastCriticalWarning = 0;

// ==================== FUNCTION PROTOTYPES ====================

void printConfiguration();
void printHeader();
void printBatteryStatus();
void handleStateChange();
void handleCriticalBattery();

// ==================== SETUP ====================

/**
 * @brief Setup function - called once at startup
 * 
 * Initializes:
 * - Serial communication
 * - Battery simulator (if enabled)
 * - Resource monitor (if enabled)
 * - Battery monitor
 * - LED feedback
 */
void setup() {
    // Initialize serial
    Serial.begin(9600);
    while (!Serial && millis() < 3000); // Wait up to 3 seconds
    
    // Print banner
    Serial.println(F("\n========================================"));
    Serial.println(F("   Battery Monitor System v1.0"));
    Serial.println(F("   4-Stage Monitoring with Feedback"));
    Serial.println(F("========================================\n"));
    
    // Print configuration
    printConfiguration();
    
    // Initialize simulator (if enabled)
    #ifdef SIMULATE_BATTERY_DISCHARGE
    g_batterySimulator.begin();
    #endif
    
    // Initialize resource monitor (if enabled)
    #ifdef DEBUG_RESOURCES
    RESOURCE_MONITOR_BEGIN();
    #endif
    
    // Initialize feedback with boot animation
    feedback.begin();
    feedback.bootAnimation();
    
    // Initialize battery monitor
    battery.begin();
    
    Serial.println(F("System initialized. Starting monitoring...\n"));
    printHeader();
}

// ==================== MAIN LOOP ====================

/**
 * @brief Main loop - called repeatedly
 * 
 * Tasks:
 * - Update resource monitor (if enabled)
 * - Update battery reading (every 2s)
 * - Update LED feedback (every iteration)
 * - Print status (every 2s)
 * - Handle state changes
 * - Warn on critical battery
 */
void loop() {
    const unsigned long now = millis();
    
    // Update resource monitor (call early)
    RESOURCE_MONITOR_UPDATE();
    
    // Update battery reading (every 2 seconds)
    if (now - lastBatteryUpdate >= BatteryConfig::UPDATE_INTERVAL_MS) {
        lastBatteryUpdate = now;
        
        // Read battery (blocks for ~1 second due to filtering)
        battery.update();
        
        // Check for state changes
        if (battery.stateChanged()) {
            handleStateChange();
        }
    }
    
    // Update LED feedback (non-blocking, call frequently)
    feedback.update(battery.getState());
    
    // Print status (every 2 seconds)
    if (now - lastSerialPrint >= BatteryConfig::SERIAL_PRINT_INTERVAL_MS) {
        lastSerialPrint = now;
        printBatteryStatus();
    }
    
    // Handle critical battery warnings
    if (battery.isCritical()) {
        handleCriticalBattery();
    }
    
    // Auto-report resources (every 30 seconds)
    RESOURCE_MONITOR_AUTO(BatteryConfig::RESOURCE_REPORT_INTERVAL_MS);
}

// ==================== HELPER FUNCTIONS ====================

/**
 * @brief Print system configuration
 */
void printConfiguration() {
    Serial.println(F("Configuration:"));
    
    Serial.print(F("  Nominal Voltage:  ")); 
    Serial.print(BatteryConfig::NOMINAL_VOLTAGE, 2); 
    Serial.println(F("V"));
    
    Serial.print(F("  FULL Threshold:   ")); 
    Serial.print(BatteryConfig::FULL_THRESHOLD, 2); 
    Serial.println(F("V (75-100%)"));
    
    Serial.print(F("  GOOD Threshold:   ")); 
    Serial.print(BatteryConfig::GOOD_THRESHOLD, 2); 
    Serial.println(F("V (50-75%)"));
    
    Serial.print(F("  LOW Threshold:    ")); 
    Serial.print(BatteryConfig::LOW_THRESHOLD, 2); 
    Serial.println(F("V (25-50%)"));
    
    Serial.print(F("  CRITICAL Threshold: ")); 
    Serial.print(BatteryConfig::CRITICAL_THRESHOLD, 2); 
    Serial.println(F("V (0-25%)"));
    
    Serial.print(F("  Hysteresis:       ")); 
    Serial.print(BatteryConfig::HYSTERESIS, 3); 
    Serial.println(F("V"));
    
    Serial.print(F("  Update Interval:  ")); 
    Serial.print(BatteryConfig::UPDATE_INTERVAL_MS / 1000); 
    Serial.println(F("s"));
    
    Serial.println();
}

/**
 * @brief Print table header
 */
void printHeader() {
    Serial.println(F("Time(s) | Voltage | State    | %   | Icon  | Drop(mV/s)"));
    Serial.println(F("--------|---------|----------|-----|-------|------------"));
}

/**
 * @brief Print current battery status (single line)
 */
void printBatteryStatus() {
    // Time (seconds since boot)
    const uint32_t seconds = millis() / 1000;
    if (seconds < 10) Serial.print(F(" "));
    if (seconds < 100) Serial.print(F(" "));
    if (seconds < 1000) Serial.print(F(" "));
    Serial.print(seconds);
    Serial.print(F("\t| "));
    
    // Voltage
    Serial.print(battery.getVoltage(), 2);
    Serial.print(F("V\t| "));
    
    // State (padded to 8 chars)
    const char* stateStr = batteryStateToString(battery.getState());
    Serial.print(stateStr);
    const uint8_t padding = 8 - strlen(stateStr);
    for (uint8_t i = 0; i < padding; ++i) Serial.print(F(" "));
    Serial.print(F(" | "));
    
    // Percentage (padded to 3 digits)
    const uint8_t pct = battery.getPercentage();
    if (pct < 10) Serial.print(F(" "));
    if (pct < 100) Serial.print(F(" "));
    Serial.print(pct);
    Serial.print(F("%\t| "));
    
    // Icon
    Serial.print(getIconForState(battery.getState()));
    Serial.print(F(" | "));
    
    // Voltage drop rate (mV/s)
    const float dropMvPerSec = battery.getVoltageDrop() * 1000.0f;
    if (dropMvPerSec >= 0) Serial.print(F(" ")); // Align positive
    Serial.print(dropMvPerSec, 1);
    
    Serial.println();
}

/**
 * @brief Handle battery state transitions
 * 
 * Prints notification when state changes.
 */
void handleStateChange() {
    Serial.println();
    Serial.print(F(">>> State changed to: "));
    Serial.print(batteryStateToDisplayString(battery.getState()));
    Serial.print(F(" ("));
    Serial.print(batteryStateToString(battery.getState()));
    Serial.println(F(") <<<"));
    Serial.println();
    
    // Reprint header after state change for clarity
    printHeader();
}

/**
 * @brief Handle critical battery condition
 * 
 * Prints urgent warning messages periodically.
 * Add additional critical actions here (e.g., save state, shutdown).
 */
void handleCriticalBattery() {
    const unsigned long now = millis();
    
    // Print warning every 10 seconds
    if (now - lastCriticalWarning >= BatteryConfig::CRITICAL_WARNING_INTERVAL_MS) {
        lastCriticalWarning = now;
        
        Serial.println(F("\n!!! WARNING: CRITICAL BATTERY LEVEL !!!"));
        Serial.print(F("!!! Current Voltage: "));
        Serial.print(battery.getVoltage(), 2);
        Serial.println(F("V !!!"));
        Serial.println(F("!!! PLEASE RECHARGE IMMEDIATELY !!!\n"));
        
        // TODO: Add critical actions here
        // - Save application state
        // - Reduce power consumption
        // - Send alert notification
        // - Initiate safe shutdown sequence
    }
}
```

---

# Testing & Debugging

## Test Procedures

### 1. Hardware Test (Production Build)

```bash
# Build and upload production firmware
pio run -e production -t upload

# Open serial monitor
pio device monitor

# Expected output:
# - Configuration printed
# - Battery readings every 2 seconds
# - State transitions as voltage changes
# - LED feedback active
```

### 2. Simulation Test

```bash
# Build and upload simulation firmware
pio run -e simulation -t upload

# Open serial monitor
pio device monitor

# Expected output:
# - "BATTERY SIMULATOR ACTIVE" message
# - Voltage decreasing over 45 seconds
# - State transitions: FULL → GOOD → LOW → CRITICAL
# - Auto-restart after complete discharge
```

### 3. Resource Monitoring Test

```bash
# Build and upload debug firmware
pio run -e debug_resources -t upload

# Open serial monitor
pio device monitor

# Expected output:
# - Resource monitor initialized
# - Memory report every 30 seconds
# - Loop timing statistics
# - Warnings if RAM usage high
```

## Verification Checklist

- [ ] **Voltage Reading**: Verify ADC values are reasonable (0-1023)
- [ ] **Voltage Calculation**: Check voltage matches actual battery voltage (±0.1V)
- [ ] **State Transitions**: Confirm thresholds trigger correct states
- [ ] **Hysteresis**: Verify no rapid oscillation near thresholds
- [ ] **Percentage**: Check 100% at nominal, 0% at critical
- [ ] **LED Feedback**: All LEDs light up correctly for each state
- [ ] **LED Blink**: LOW blinks at 1Hz, CRITICAL at 3Hz
- [ ] **Serial Output**: Table formatted correctly, no garbage
- [ ] **Memory Usage**: Free RAM > 500 bytes (check with resource monitor)
- [ ] **Loop Timing**: Max loop time < 10ms (except during battery update)
- [ ] **Simulation**: Voltage decreases linearly, auto-restart works
- [ ] **Critical Warning**: Messages print every 10s when critical

## Common Issues & Solutions

### Issue: Voltage reading is 0V or incorrect

**Cause**: Wiring error or wrong pin configuration

**Solution**:
1. Check voltage divider connections
2. Verify `ANALOG_PIN` matches physical wiring
3. Test with multimeter at ADC pin
4. Check `R1` and `R2` values in config

### Issue: State never changes

**Cause**: Thresholds not appropriate for battery

**Solution**:
1. Measure actual battery voltage range
2. Adjust thresholds in `BatteryConfig.h`
3. Ensure `NOMINAL_VOLTAGE` > all thresholds

### Issue: Rapid state oscillation

**Cause**: Hysteresis too small or disabled

**Solution**:
1. Increase `HYSTERESIS` to 0.1V or higher
2. Ensure hysteresis logic is enabled in `calculateState()`

### Issue: LED not blinking

**Cause**: LED pins incorrect or update() not called

**Solution**:
1. Verify LED pin numbers in config
2. Ensure `feedback.update()` called in main loop
3. Check LED polarity (anode to pin, cathode to GND through resistor)

### Issue: RAM usage too high

**Cause**: Too many string literals or large buffers

**Solution**:
1. Use `F()` macro for all string literals
2. Reduce `SAMPLES` count
3. Check for memory leaks with resource monitor
4. Avoid String class (use char arrays)

### Issue: Slow loop performance

**Cause**: Battery update takes ~1 second (blocking)

**Solution**:
1. This is expected due to 10-sample averaging
2. Increase `UPDATE_INTERVAL_MS` if needed
3. Reduce `SAMPLES` for faster updates (less filtering)
4. Consider non-blocking ADC reads (advanced)

---

# Build Configuration

## platformio.ini

```ini
; ========================================
; Battery Monitor Project
; Platform: Arduino Uno (ATmega328P)
; ========================================

[platformio]
default_envs = production

; ==================== COMMON SETTINGS ====================
[env]
platform = atmelavr
framework = arduino
board = uno
lib_deps = 
monitor_speed = 9600
monitor_filters = direct
build_flags = 
    -Wall          ; Enable all warnings
    -Wextra        ; Extra warnings
    -Werror=return-type  ; Error on missing return

; ==================== PRODUCTION BUILD ====================
; Real hardware, optimized for size
[env:production]
build_flags = 
    ${env.build_flags}
    -Os            ; Optimize for size
build_type = release

; ==================== SIMULATION BUILD ====================
; Virtual battery discharge for testing
[env:simulation]
build_flags = 
    ${env.build_flags}
    -DSIMULATE_BATTERY_DISCHARGE
    -Os
build_type = release

; ==================== DEBUG BUILD ====================
; Production with resource monitoring
[env:debug_resources]
build_flags = 
    ${env.build_flags}
    -DDEBUG_RESOURCES
    -Og            ; Optimize for debugging
build_type = debug

; ==================== DEBUG + SIMULATION ====================
; Full debug with virtual battery
[env:debug_simulation]
build_flags = 
    ${env.build_flags}
    -DDEBUG_RESOURCES
    -DSIMULATE_BATTERY_DISCHARGE
    -Og
build_type = debug

; ==================== WOKWI SIMULATION ====================
; For wokwi.com web simulator
[env:wokwi]
build_flags = 
    ${env.build_flags}
    -DSIMULATE_BATTERY_DISCHARGE
    -Os
```

## Build Commands

```bash
# Production (real hardware)
pio run -e production -t upload
pio device monitor

# Simulation only
pio run -e simulation -t upload
pio device monitor

# Debug with resource monitoring
pio run -e debug_resources -t upload
pio device monitor

# Debug with simulation
pio run -e debug_simulation -t upload
pio device monitor

# Clean build
pio run -t clean

# Check memory usage
pio run -e production -t size

# Format code (if clang-format installed)
pio run -t format

# Check static analysis
pio check
```

## Memory Targets

| Build | Flash Target | RAM Target |
|-------|-------------|-----------|
| Production | < 10 KB (33%) | < 500 bytes (24%) |
| Simulation | < 12 KB (39%) | < 600 bytes (29%) |
| Debug | < 15 KB (49%) | < 800 bytes (39%) |

---

# Implementation Instructions for GitHub Copilot

## Step-by-Step Implementation

1. **Create Project Structure**
   ```bash
   mkdir battery-monitor
   cd battery-monitor
   pio init --board uno
   mkdir -p include src
   ```

2. **Copy Configuration Files**
   - Copy `platformio.ini` to project root
   - Create all header files in `include/`
   - Create `main.cpp` in `src/`

3. **Implementation Order**
   - Start with `BatteryConfig.h` (configuration only)
   - Then `BatteryState.h` (enum and utilities)
   - Then `BatteryMonitor.h` (core logic)
   - Then `BatteryFeedback.h` (LED control)
   - Then `BatterySimulator.h` (optional)
   - Then `ResourceMonitor.h` (optional)
   - Finally `main.cpp` (application)

4. **Compile and Test**
   ```bash
   # Test compilation
   pio run -e production
   
   # Check for warnings
   pio run -e production --verbose
   
   # Upload to hardware
   pio run -e production -t upload
   
   # Monitor serial output
   pio device monitor
   ```

5. **Verification**
   - Verify voltage readings are correct
   - Test all 4 state transitions
   - Check LED feedback for each state
   - Monitor RAM usage with debug build
   - Run simulation mode for end-to-end test

## Code Style Guidelines

- **Naming**:
  - Classes: `PascalCase` (e.g., `BatteryMonitor`)
  - Methods: `camelCase` (e.g., `getVoltage()`)
  - Private members: `camelCase_` with trailing underscore (e.g., `currentVoltage_`)
  - Constants: `SCREAMING_SNAKE_CASE` (e.g., `NOMINAL_VOLTAGE`)
  - Namespaces: `PascalCase` (e.g., `BatteryConfig`)

- **Comments**:
  - Use Doxygen style for public API: `/** @brief ... */`
  - Use `//` for inline comments
  - Use `/* */` for block comments

- **Formatting**:
  - Indentation: 4 spaces (no tabs)
  - Line length: max 100 characters
  - Braces: K&R style (opening brace on same line)

- **Memory**:
  - No dynamic allocation (`new`/`delete`)
  - Use `F()` macro for string literals
  - Prefer `constexpr` over `#define`
  - Use `const` for read-only methods

## Critical Implementation Notes

1. **ADC Reading**: The `readFilteredVoltage()` method is BLOCKING and takes ~1 second. This is intentional for noise reduction.

2. **Hysteresis**: Must be applied ONLY when voltage is increasing to prevent oscillation.

3. **LED Update**: `feedback.update()` must be called frequently (every loop iteration) for smooth blinking.

4. **Resource Monitor**: `update()` must be called at the BEGINNING of loop() for accurate timing.

5. **Simulation**: When `SIMULATE_BATTERY_DISCHARGE` is defined, `analogRead()` is overridden globally. The simulator instance must be global.

6. **Serial Output**: Always use `F()` macro to keep strings in Flash, not RAM.

7. **Const Correctness**: All getter methods and non-modifying methods must be marked `const`.

8. **State Machine**: State calculation uses descending threshold checks (FULL → GOOD → LOW → CRITICAL).

---

# Expected Output Example

```
========================================
   Battery Monitor System v1.0
   4-Stage Monitoring with Feedback
========================================

Configuration:
  Nominal Voltage:  9.00V
  FULL Threshold:   8.50V (75-100%)
  GOOD Threshold:   8.00V (50-75%)
  LOW Threshold:    7.50V (25-50%)
  CRITICAL Threshold: 7.00V (0-25%)
  Hysteresis:       0.050V
  Update Interval:  2s

System initialized. Starting monitoring...

Time(s) | Voltage | State    | %   | Icon  | Drop(mV/s)
--------|---------|----------|-----|-------|------------
   2    | 9.00V   | FULL     | 100%| [====]|  0.0
   4    | 8.95V   | FULL     |  97%| [====]| 25.0
   6    | 8.85V   | FULL     |  92%| [====]| 50.0
   8    | 8.70V   | FULL     |  85%| [====]| 75.0
  10    | 8.45V   | GOOD     |  72%| [=== ]|125.0

>>> State changed to: Buona (GOOD) <

Time(s) | Voltage | State    | %   | Icon  | Drop(mV/s)
--------|---------|----------|-----|-------|------------
  12    | 8.25V   | GOOD     |  62%| [=== ]|100.0
  14    | 8.05V   | GOOD     |  52%| [=== ]|100.0
  16    | 7.85V   | LOW      |  42%| [==  ]|100.0

>>> State changed to: Bassa (LOW) <

Time(s) | Voltage | State    | %   | Icon  | Drop(mV/s)
--------|---------|----------|-----|-------|------------
  18    | 7.65V   | LOW      |  32%| [==  ]|100.0
  20    | 7.45V   | CRITICAL |  22%| [=   ]|100.0

>>> State changed to: Critica (CRITICAL) <

Time(s) | Voltage | State    | %   | Icon  | Drop(mV/s)
--------|---------|----------|-----|-------|------------
  22    | 7.25V   | CRITICAL |  12%| [=   ]|100.0

!!! WARNING: CRITICAL BATTERY LEVEL !!!
!!! Current Voltage: 7.25V !!!
!!! PLEASE RECHARGE IMMEDIATELY !!!

  24    | 7.05V   | CRITICAL |   2%| [=   ]|100.0
  26    | 6.85V   | CRITICAL |   0%| [=   ]|100.0
```

---

**END OF DOCUMENT**

This complete specification document contains everything needed to implement the battery monitoring system. GitHub Copilot can use this as context to generate all code files correctly.