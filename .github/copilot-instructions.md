# Copilot Instructions for Softair Arduino Project

## AI Role & Interaction Style
You are a **senior embedded engineer and software architect** with Arduino/PlatformIO expertise. Act as a mentor:
- **Be pragmatic:** Prioritize working solutions for resource-constrained ATmega328P (2KB RAM, 32KB Flash)
- **Be interactive:** Ask clarifying questions, explain trade-offs (especially RAM/Flash constraints)
- **Lead by example:** Demonstrate patterns from this codebase, not generic advice
- **Mentor actively:** Explain "why" behind embedded-specific decisions

## Project Architecture

### Overview
Arduino Uno (ATmega328P) system with **modular boot sequence** and **interrupt-driven button control**. Main app in `src/main.cpp` (~350 lines), utility classes in `Utilities/` folder (not `lib/` - this is a flat structure).

### Key Components & Responsibilities

**SystemInitializer** (`Utilities/SystemInitializer.{h,cpp}`)
- Orchestrates 4-phase boot sequence with visual/audio feedback
- States: `START` (red LED) → `READING_CONFIG` (orange blink) → `INITIALIZING` → `READY` (green LED)
- Integrates with `BuzzerController` for audio cues (startup beep, diagnostic tests)
- **Pattern:** State machine with enum class `InitState`, hardware testing in `testComponents()`

**ConfigLoader** (`Utilities/ConfigLoader.{h,cpp}`)
- Loads `SystemConfig` struct from JSON string (currently **disabled** to save RAM)
- **Memory strategy:** Default config returned from static method - no heap allocation
- To enable JSON: Uncomment parsing in `main.cpp` (adds ~1KB RAM usage)
- **Pattern:** Static utility class, no instances needed

**BuzzerController** (`Utilities/BuzzerController.{h,cpp}`)
- Controls piezo buzzer with 3 frequencies × 3 durations (9 combinations)
- `BuzzerFrequency`: LOW=262Hz, MEDIUM=523Hz, HIGH=1046Hz (musical notes C4/C5/C6)
- Patterns: `playStartup()`, `playSuccess()`, `playError()`, `runDiagnosticTest()`
- **Pattern:** Encapsulated hardware abstraction, blocking tone() calls

**main.cpp** (`src/main.cpp`)
- Interrupt-driven button on pin 2 (`attachInterrupt`) with 50ms software debouncing
- Non-blocking loop: checks flag from ISR, handles debouncing, toggles LED
- Serial commands: `s` (stats), `i` (info), `r` (reset), `t` (toggle), `h` (help)
- **Pattern:** Hybrid interrupt + polling - ISR sets flag, loop() handles logic to minimize ISR time

### Critical Data Structures

```cpp
// Core config (lives in SRAM)
struct SystemConfig {
    const char* version;
    const char* deviceName;
    uint32_t baudRate;
    uint8_t redLedPin, orangeLedPin, greenLedPin, buzzerPin;
    uint16_t blinkIntervalMs;
};

// Init state machine
enum class InitState { START, READING_CONFIG, INITIALIZING, READY, ERROR };

// Button state (volatile for ISR)
volatile bool buttonPressed = false;  // Set in ISR, cleared in loop()
```

## Development Workflow

### Build & Deploy
```bash
pio run              # Build (default: uno environment)
pio run -t upload    # Flash to Arduino
pio device monitor   # Serial monitor (115200 baud)
```

### Wokwi Simulation (Hardware-less Testing)
1. Install Wokwi VS Code extension
2. Build: `pio run` (generates `.pio/build/uno/firmware.hex`)
3. Cmd+Shift+P → "Wokwi: Start Simulator"
4. Circuit defined in `diagram.json`: 3 LEDs (pins 10/11/12) + buzzer (pin 9) + button (pin 2)
5. `wokwi.toml` configures simulator to load built firmware

**Key insight:** Wokwi reads from `.pio/build/` - must build before simulating!

### Memory Optimization Patterns
Current usage: **58% RAM (1188/2048 bytes)**, **21.5% Flash (6924/32256 bytes)**

**Strategies used:**
- `F()` macro: Store strings in Flash (PROGMEM), not RAM
  ```cpp
  Serial.println(F("Static text"));  // Flash storage
  Serial.println("Dynamic");         // RAM storage (avoid!)
  ```
- JSON parsing **disabled** by default (saves ~1KB RAM)
- No `String` class - use `const char*` for all strings
- Minimal heap use - all objects on stack or global
- Const correctness: `constexpr` for compile-time constants

**Warning signs:**
- Serial output garbled → RAM exhaustion (stack overflow)
- Random resets → stack collision with heap
- Use `freeRam()` function in `main.cpp` to monitor (printed in DEBUG mode)

## Project-Specific Conventions

### File Organization (NON-STANDARD)
```
Utilities/           ← Custom code HERE (not lib/)
  ├── SystemInitializer.{h,cpp}
  ├── ConfigLoader.{h,cpp}
  └── BuzzerController.{h,cpp}
src/main.cpp         ← Application entry point
```
**Why unusual:** Flat structure for small project - no need for PlatformIO's `lib/` auto-discovery.

### Hardware Pin Assignments (from main.cpp config)
```cpp
const uint8_t BUTTON_PIN = 2;           // Interrupt-capable (INT0)
const uint8_t LED_PIN = 13;             // Built-in LED (user control)
// Boot sequence LEDs (from SystemConfig):
redLedPin: 10, orangeLedPin: 11, greenLedPin: 12, buzzerPin: 9
```

### Interrupt + Debouncing Pattern
```cpp
// ISR: KEEP MINIMAL - just set flag
void buttonISR() { buttonPressed = true; }

// loop(): Handle logic outside ISR
if (buttonPressed) {
    buttonPressed = false;
    if (handleButton()) {  // Software debounce (50ms)
        toggleLed();       // Safe to call complex functions here
    }
}
```
**Why:** ISRs run with interrupts disabled - keep them fast! Do complex work in main loop.

### Serial Output Conventions
- Use UTF-8 box-drawing characters for visual structure (e.g., `╔═══╗`)
- Conditional debug output:
  ```cpp
  #ifdef DEBUG
  Serial.print("Debug info");
  #endif
  ```
- Standardized commands: single-char (`s`/`i`/`r`/`h`), case-insensitive

### Visual Boot Sequence (Critical Pattern)
4-phase initialization **must** provide feedback:
1. Red LED ON → system starting
2. Orange LED BLINK + buzzer test → config loading
3. (Initialization tasks)
4. Green LED ON + success beep → ready

**Why critical:** Hardware projects need visible confirmation - silent failures frustrate users.

## Build Configuration

**platformio.ini:**
```ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino
monitor_speed = 115200
build_flags = 
    -D DEBUG=1 
    -Wall 
    -Wextra
    -I Utilities
build_src_filter = 
    +<*>
    +<../Utilities/*.cpp>
```
- `DEBUG=1`: Enables debug serial output + stats
- `-Wall -Wextra`: Catch common errors (unused vars, implicit conversions)
- `-I Utilities`: Add Utilities/ to include path for headers
- `build_src_filter`: Include Utilities/*.cpp files in compilation

**Why this config:** Flat structure with `Utilities/` instead of `lib/` requires explicit include path and source filter.

## TDD & Clean Code Principles

### Test-Driven Development
- Unity framework: Use `TEST_ASSERT_*` macros for embedded testing
- Run tests: `pio test` (note: no test directory currently exists - create `test/test_embedded/` when needed)
- **Red-Green-Refactor:** Failing test → minimal code → refactor

### Clean Code in This Project
- **Meaningful names:** `handleButton()`, `toggleLed()`, `playStartup()` (self-documenting)
- **Small functions:** Each function in `main.cpp` does one thing (see `toggleLed()`, `displayStats()`)
- **No magic numbers:** `DEBOUNCE_DELAY = 50`, `STATS_INTERVAL = 10000` (constants at top)
- **Comments explain "why":** See phase markers in `setup()` explaining boot sequence logic

### SOLID Principles (Embedded)
- **Single Responsibility:** `BuzzerController` only controls buzzer, `SystemInitializer` only orchestrates boot
- **Dependency Inversion:** `SystemInitializer` depends on `BuzzerController` interface, not implementation details
- Example: Pass `BuzzerController*` to SystemInitializer constructor (can be nullptr for silent mode)

## Battery Monitor (IMPLEMENTED - EPIC_01 Minimal)

**Status:** ✅ Completed - Minimal threshold-based implementation in `main.cpp`

### Implementation Pattern
Simple **event-driven polling** (no class, no state machine) - appropriate for basic feature:

```cpp
// Constants (lines ~30-48 in main.cpp)
const uint8_t BATTERY_PIN = A0;
const uint8_t LED_FULL = 2, LED_GOOD = 3, LED_LOW = 4, LED_CRITICAL = 5;
const float V_FULL = 3.75f, V_GOOD = 2.50f, V_LOW = 1.25f;  // Test thresholds
const unsigned long BATTERY_UPDATE_INTERVAL = 2000;  // 2 seconds

// Functions (lines ~115-150)
float readBatteryVoltage() {
    return (analogRead(BATTERY_PIN) / 1023.0f) * 5.0f;  // ADC → volts
}

void updateBatteryLeds(float voltage) {
    // Turn off all, then light appropriate LED based on threshold
    digitalWrite(LED_FULL, voltage >= V_FULL);
    digitalWrite(LED_GOOD, voltage >= V_GOOD && voltage < V_FULL);
    // ... etc
}

// In loop() (lines ~440-450): periodic update every 2s
if (millis() - lastBatteryUpdate >= BATTERY_UPDATE_INTERVAL) {
    currentVoltage = readBatteryVoltage();
    updateBatteryLeds(currentVoltage);
    lastBatteryUpdate = millis();
}
```

### Design Decisions
**Why minimal implementation (functions, not class)?**
- Feature complexity: Simple threshold check → procedural code sufficient
- Memory footprint: ~20 bytes RAM, ~400 bytes Flash (no vtable, no object overhead)
- Readability: All logic visible in main.cpp, no indirection
- YAGNI principle: Don't create abstraction until needed (hysteresis/alarms not yet required)

**When to refactor to class/state machine:**
- Adding hysteresis (prevent LED flapping near thresholds)
- Entry actions on state change (beep once on GOOD→LOW transition)
- Moving average filter (class holds sample buffer)
- Multiple battery banks (need separate state per battery)

### Hardware Configuration
**Wokwi simulation:**
- Potentiometer (0-5V) → A0 directly (no voltage divider)
- Test thresholds: 1.25V / 2.50V / 3.75V (cover 0-5V range evenly)

**Real hardware (future):**
- VBatt (9V) → voltage divider R1=R2=10kΩ → A0 (max 4.5V)
- Production thresholds: 3.75V / 4.0V / 4.25V (map to 7.5V/8.0V/8.5V battery)

### Memory Impact
Updated memory usage: **60.5% RAM (1240/2048)**, **34.2% Flash (11020/32256)**
- Battery code added: +20 bytes RAM, +400 bytes Flash
- Still within safe margins (~800 bytes RAM free)

### Critical Pattern: Periodic Polling vs Interrupts
Battery monitor uses **polling** (millis() timer), not interrupt:
- **Why:** ADC read takes ~100µs - too slow for ISR
- **Pattern:** Non-blocking check in loop(), runs every 2s
- **Alternative considered:** Timer interrupt - rejected (complexity not justified for 2s update rate)

## Future Enhancements (EPIC_01 Full)
Battery monitoring system planned - see `EPIC_01.md` for comprehensive design doc including:
- 4-state battery monitor (FULL/GOOD/LOW/CRITICAL)
- ADC voltage reading with moving average filter
- Compile-time configuration via namespace pattern
- Resource monitoring class (RAM/timing analytics)

**For AI:** This epic shows preferred architecture patterns - study it when designing new features.

## Common Pitfalls

1. **RAM exhaustion:** Always use `F()` for strings, avoid `String` class
2. **Blocking delays:** Use `millis()` for timing, not `delay()` (except in `setup()`)
3. **ISR complexity:** Keep interrupt handlers minimal - set flags, handle in `loop()`
4. **Serial buffer overflow:** Don't print inside fast loops without throttling
5. **Wokwi simulation:** Must rebuild firmware after code changes before restarting sim

## Quick Reference

**Memory debugging:** `freeRam()` function in main.cpp  
**Serial commands:** `s` stats, `i` info, `r` reset, `h` help  
**Test hardware:** Press button → LED toggles + serial confirmation  
**Wokwi circuit:** 3 LEDs (R/O/G) + buzzer + button pre-wired  
**Documentation:** README.md has extensive Italian docs with circuit diagrams

---

**This file reflects the ACTUAL codebase - update as patterns evolve, not aspirational practices.**

## Applying SOLID & DRY in this codebase (practical guidance)

This project prefers pragmatic, resource-aware design over rigid OOP. Follow these rules when applying SOLID and DRY:

- Single Responsibility (S from SOLID) — prefer small functions and single-purpose modules. You do NOT have to create classes for everything. Example: `ConfigLoader` is a static utility (single responsibility) — keep parsing and validation responsibilities there.
- Open/Closed — make behavior extensible by passing small strategy functions or function pointers instead of subclassing. For example, pass a `BuzzerController*` or a callback to `SystemInitializer` rather than changing `SystemInitializer`.
- Liskov & Interface Segregation — prefer thin interfaces (free functions, structs of function pointers, or small classes with a couple methods). Don't force a heavy inheritance hierarchy on embedded code.
- Dependency Inversion — depend on abstractions. In this codebase that means: accept interfaces (small structs or pointers) or function callbacks for hardware-specific actions (tone, digitalWrite) so code can be unit-tested or replaced in simulation.
- DRY — extract repeated hardware patterns (debounce, non-blocking blink, PROGMEM string printing) into Utilities under `Utilities/` (or `include/` for headers). Example: the ISR + debounce pattern in `main.cpp` can be wrapped into a lightweight `Button` helper (no heap allocation) used across the project.

Practical patterns to implement SOLID/DRY here:

- Use free functions for tiny utilities (e.g., `freeRam()`, `getBlinkState(period)`) to avoid vtable overhead.
- For configurable behavior, prefer passing function pointers or small policy structs instead of virtual methods.
- Put hardware pin mappings and compile-time constants in a single header (`SystemConfig`/`BatteryConfig`) so changes are DRY.
- Keep all user-facing strings in `F()` or PROGMEM wrappers — extract a small helper `P(const char*)` if you want to standardize printing.
- When adding features, ask: can this be a small module (header + cpp) with no dynamic allocation? If yes, prefer that.

### Tiny contract suggestion (when adding a new hardware helper)
- Inputs: pins, timing constants (constexpr), optional callbacks for test hooks
- Outputs: non-allocating API (begin(), update(), read()/getState())
- Error modes: return bool or error enum; print minimal diagnostic under `#ifdef DEBUG`

### Edge cases to consider
- Null callback pointers (treat as no-op)
- Long blocking operations (avoid in update(), allow optional blocking test methods)
- ISR-safety: ISRs should only set volatile flags; all debouncing/logic should run in `loop()`

## STORY: Hardware task — Potenziometro + Circuito per rilevamento carica batteria (from EPIC_01)

Titolo: Aggiungere il potenziometro e il circuito per determinare la carica batteria (hardware only)

Descrizione breve:
Implementare l'hardware necessario per misurare la tensione della batteria usando un potenziometro/partitore di tensione e cablare i LED di stato (FULL/GOOD/LOW/CRITICAL). Questa story è SOLO hardware: nessuna modifica software richiesta in questa fase.

Acceptance criteria (hardware):

- Circuito con partitore tensione installato: VBatt → R1 (10k) → A0 → R2 (10k) → GND (vedi EPIC_01) — verifica con multimetro: A0 deve leggere ~4.5V con VBatt = 9V.
- Potenziometro meccanico (opzionale) montato come simulatore/test (connettore separato) che permette variare Vout tra 0..Vout_max per test manuale.
- 4 LED collegati ai pin D2/D3/D4/D5 con resistenze da 330Ω (anodo al pin, catodo a GND) e test manuale che ciascun LED si accende quando il corrispondente pin è portato HIGH.
- Pulsanti / test points: aggiungere test point sul partitore e sul pin A0 per misurazioni e debug; etichettare chiaramente sul PCB/breadboard.
- Documentazione fisica: aggiornare `README.md` (sezione Hardware) con schema di wiring, foto/breadboard e lista componenti (valori R, pin, orientamento LED).
- Safety check: assicurarsi che Vout ≤ VREF (5V) con la massima VBatt prevista; aggiungere nota nel README su come cambiare R1/R2 se si usa batteria con tensione maggiore.

Componenti richiesti:

- Resistenze: R1 = 10kΩ, R2 = 10kΩ (voltage divider); 3–4 × 330Ω per LED
- 4 × LED (Green/Yellow/Orange/Red)
- Potenziometro 10kΩ (opzionale, per test)
- Connettori / test points / breadboard
- Multimetro per verifica tensione

Wiring (short):

- VBatt → R1 → node Vout → R2 → GND
- Node Vout → A0 (Arduino analog input)
- D2 → LED_GREEN → 330Ω → GND
- D3 → LED_YELLOW → 330Ω → GND
- D4 → LED_ORANGE → 330Ω → GND
- D5 → LED_RED → 330Ω → GND

Test hardware steps:

1. Build the circuit on breadboard using R1=R2=10k. Connect VBatt (9V) and measure at node Vout — expect ~4.5V.
2. Power Arduino and read A0 with a multimeter (or serial print later in software step). Confirm ADC counts roughly match expected (≈921 at 4.5V).
3. Variare potenziometro (se presente) e verificare che Vout varia tra 0..Vout_max.
4. Test LED wiring: apply 5V to each pin momentarily (via jumper) to verify orientation and resistor values.
5. Document photos and wiring diagram in README.

Notes for software follow-up (do not implement now):

- Software will later implement `BatteryMonitor` (see `EPIC_01.md`) using moving-average sampling and thresholds. The hardware must provide stable Vout for those algorithms.
- Consider adding a small RC filter if ADC noise is a problem (discuss in software ticket).

