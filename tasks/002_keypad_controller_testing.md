# Task #002: Keypad Controller Testing

**ID**: 002  
**Titolo**: Test Completo KeypadController 4×4 Matrix  
**Tipo**: 🧪 TEST  
**Priorità**: 🟡 HIGH  
**Stato**: ⏳ BLOCKED (by Task 001)  
**Milestone**: M1 — MVP Stabile  

**Stima iniziale**: 4-6 ore  
**Tempo effettivo**: TBD  
**Assegnato a**: Team Core  
**Data creazione**: 19 Dicembre 2025  
**Dipendenze**: Task 001 (Display Flickering Fix)  

---

## 📋 Descrizione

Eseguire test end-to-end del `KeypadController` implementato in EPIC_03, verificando matrix scanning, debouncing, D-pad navigation (UP/DOWN/LEFT/RIGHT), e mode toggle (KEY_A). Controller già compilato e integrato ma **mai testato** dopo integration.

**Context**:
- KeypadController implementato in `Utilities/KeypadController.{h,cpp}`
- 16 key layout: digits 0-9, D-pad (UP/DOWN/LEFT/RIGHT), mode toggle (KEY_A)
- Matrix scan con debouncing software 50ms
- Integrato in `main.cpp` con `keypad->scan()` chiamato ogni loop

**Blocco**: Task 001 deve essere risolto prima (display flickering impedisce visualizzazione feedback test)

---

## 🎯 Obiettivi

### Obiettivo Primario
Verificare funzionamento corretto di tutti i 16 tasti del keypad 4×4 con matrix scanning e debouncing affidabile.

### Obiettivi Secondari
- [ ] Test matrix scanning per tutte le 16 combinazioni row/col
- [ ] Validare debouncing software (no double-press, no bounce artifacts)
- [ ] Verificare D-pad navigation (UP/DOWN/LEFT/RIGHT detection)
- [ ] Testare mode toggle (KEY_A functionality)
- [ ] Stress test con rapid key presses (>10 keys/sec)
- [ ] Misurare scan latency (tempo detection key press)

---

## ✅ Acceptance Criteria (BDD)

### Scenario 1: Matrix Scanning - Digits 0-9

```gherkin
Feature: Numeric Key Detection
  As a user
  I want digit keys 0-9 rilevati correttamente
  So that posso inserire valori countdown

  Background:
    Given KeypadController initialized con pins ROW[13,12,11,10], COL[A5,9,8,7]
    And Serial monitor aperto @ 115200 baud
    And Display flickering FIXED (Task 001)

  Scenario Outline: Single digit key press
    Given Keypad idle (no keys pressed)
    When I press key "<key>"
    Then Serial output mostra "Key pressed: <key>"
    And Display aggiorna con digit "<key>" in cursor position (EDIT mode)
    And Scan latency < 10ms
    
    Examples:
      | key |
      | 0   |
      | 1   |
      | 2   |
      | 3   |
      | 4   |
      | 5   |
      | 6   |
      | 7   |
      | 8   |
      | 9   |

  Scenario: Debounce validation
    Given Key "5" premuto (mechanical contact bouncing)
    When Scan rileva key dopo 50ms debounce delay
    Then Solo 1 evento "Key pressed: 5" generato
    And NO duplicate events durante bounce period
```

### Scenario 2: D-Pad Navigation

```gherkin
Feature: Directional Navigation
  As a user
  I want D-pad keys (UP/DOWN/LEFT/RIGHT) per navigare cursore
  So that posso editare cifre countdown

  Background:
    Given COUNTDOWN_EDIT mode attivo
    And Cursor visible su digit position (blink @ 500ms)

  Scenario: LEFT key - cursor movement
    Given Cursor at position D3 (minutes ones)
    When I press LEFT key (COL1/ROW4)
    Then Cursor moves to D2 (minutes tens)
    And Serial output: "Cursor moved LEFT to D2"

  Scenario: RIGHT key - cursor movement
    Given Cursor at position D2 (minutes tens)
    When I press RIGHT key (COL3/ROW4)
    Then Cursor moves to D3 (minutes ones)
    And Serial output: "Cursor moved RIGHT to D3"

  Scenario: UP key - digit increment
    Given Cursor at D4 (hours ones, value 3)
    When I press UP key (COL2/ROW1)
    Then Digit increments to 4
    And Display updates showing "X4:XX"
    And Serial output: "Digit UP: 3 → 4"

  Scenario: DOWN key - digit decrement
    Given Cursor at D4 (hours ones, value 3)
    When I press DOWN key (COL2/ROW4)
    Then Digit decrements to 2
    And Display updates showing "X2:XX"
    And Serial output: "Digit DOWN: 3 → 2"

  Scenario: Cursor wrap-around LEFT
    Given Cursor at D1 (leftmost position, hours tens)
    When I press LEFT key
    Then Cursor wraps to D4 (rightmost position, minutes ones)
    And Serial output: "Cursor wrap LEFT: D1 → D4"

  Scenario: Cursor wrap-around RIGHT
    Given Cursor at D4 (rightmost position)
    When I press RIGHT key
    Then Cursor wraps to D1 (leftmost position)
    And Serial output: "Cursor wrap RIGHT: D4 → D1"
```

### Scenario 3: Mode Toggle (KEY_A)

```gherkin
Feature: Mode Selection
  As a user
  I want KEY_A per toggle tra CLOCK e COUNTDOWN_EDIT modes
  So that posso switch display functionality

  Background:
    Given Sistema in CLOCK mode (showing current time)

  Scenario: Toggle to COUNTDOWN_EDIT mode
    Given Display showing "10:00" (clock mode)
    When I press KEY_A (COL4/ROW1)
    Then Mode changes to COUNTDOWN_EDIT
    And Cursor appare su D1 (blink attivo)
    And Serial output: "Mode: CLOCK → COUNTDOWN_EDIT"

  Scenario: Toggle back to CLOCK mode
    Given Sistema in COUNTDOWN_EDIT mode
    When I press KEY_A
    Then Mode changes to CLOCK
    And Cursor scompare (no blink)
    And Display shows clock time
    And Serial output: "Mode: COUNTDOWN_EDIT → CLOCK"

  Scenario: Mode timeout (no input)
    Given COUNTDOWN_EDIT mode attivo
    When 30 secondi passano senza key press
    Then Mode auto-reverts to CLOCK
    And Serial output: "Mode timeout → CLOCK"
```

### Scenario 4: Debouncing Edge Cases

```gherkin
Feature: Robust Debouncing
  As a system
  I want affidabile debouncing su tutti i key press
  So that mechanical bounce non genera eventi spurious

  Background:
    Given DEBOUNCE_DELAY = 50ms (configurazione default)

  Scenario: Fast double-press (< 50ms)
    Given Key "5" premuto @ t=0ms
    When Key "5" pressed again @ t=30ms (bounce artifact)
    Then Solo 1 evento generato
    And Second press ignorato (within debounce window)

  Scenario: Intentional double-press (> 50ms)
    Given Key "5" premuto @ t=0ms
    When Key "5" rilasciato @ t=25ms
    And Key "5" premuto again @ t=60ms (intentional re-press)
    Then 2 eventi separati generati
    And Both presses recognized

  Scenario: Simultaneous multi-key press (ghost key prevention)
    Given Key "1" (ROW1/COL1) premuto
    And Key "5" (ROW2/COL2) premuto simultaneously
    Then Matrix scan rileva entrambi correttamente
    And NO ghost key @ (ROW1/COL2) o (ROW2/COL1) detected
```

### Scenario 5: Stress Test

```gherkin
Feature: High-Frequency Input Handling
  As a system
  I want gestire rapid key presses senza loss
  So that responsive input anche con fast typing

  Scenario: Rapid sequential presses
    Given Utente presses keys rapidamente: 1,2,3,4,5
    When Key press rate = 5 keys/sec (200ms interval)
    Then Tutti 5 keys rilevati in sequenza
    And Serial output mostra sequenza completa: "1 2 3 4 5"
    And Nessun key skipped

  Scenario: Stress test - 100 key presses
    Given Automated test (simulated key matrix states)
    When 100 random keys pressed in 10 secondi
    Then 100% detection rate (no missed keys)
    And Scan latency avg < 10ms
    And No memory leak (RAM usage stable)
```

---

## 🛠️ Test Plan

### Phase 1: Manual Visual Testing (2 ore)

#### Setup Wokwi Simulation
1. Build firmware: `pio run`
2. Start Wokwi simulator: Cmd+Shift+P → "Wokwi: Start Simulator"
3. Open Serial Monitor @ 115200 baud

#### Test Procedure - 16 Key Matrix

| Key | Row Pin | Col Pin | Expected Scan Code | Visual Test | Serial Output | Pass/Fail |
|-----|---------|---------|-------------------|-------------|---------------|-----------|
| 1   | 13      | A5      | KEY_1             | Wokwi click | "Key: 1"      | ☐         |
| 2   | 13      | 9       | KEY_2             | Wokwi click | "Key: 2"      | ☐         |
| 3   | 13      | 8       | KEY_3             | Wokwi click | "Key: 3"      | ☐         |
| A   | 13      | 7       | KEY_A (mode)      | Wokwi click | "Mode toggle" | ☐         |
| 4   | 12      | A5      | KEY_4             | Wokwi click | "Key: 4"      | ☐         |
| 5   | 12      | 9       | KEY_5             | Wokwi click | "Key: 5"      | ☐         |
| 6   | 12      | 8       | KEY_6             | Wokwi click | "Key: 6"      | ☐         |
| B   | 12      | 7       | KEY_B (unused)    | Wokwi click | "Key: B"      | ☐         |
| 7   | 11      | A5      | KEY_7             | Wokwi click | "Key: 7"      | ☐         |
| 8   | 11      | 9       | KEY_8             | Wokwi click | "Key: 8"      | ☐         |
| 9   | 11      | 8       | KEY_9             | Wokwi click | "Key: 9"      | ☐         |
| C   | 11      | 7       | KEY_C (unused)    | Wokwi click | "Key: C"      | ☐         |
| ←   | 10      | A5      | KEY_LEFT          | Wokwi click | "LEFT"        | ☐         |
| ↓   | 10      | 9       | KEY_DOWN          | Wokwi click | "DOWN"        | ☐         |
| →   | 10      | 8       | KEY_RIGHT         | Wokwi click | "RIGHT"       | ☐         |
| ↑   | 10      | 7       | KEY_UP            | Wokwi click | "UP"          | ☐         |

**Success criteria**: 16/16 keys detected, serial output matches expected, no spurious events.

---

### Phase 2: Automated Unit Testing (2 ore)

#### Test Fixture Setup

```cpp
// test/test_keypad_controller.cpp
#include <unity.h>
#include "KeypadController.h"

// Mock GPIO per test isolato
uint8_t mockRowPins[] = {13, 12, 11, 10};
uint8_t mockColPins[] = {A5, 9, 8, 7};

KeypadController* keypad;

void setUp() {
    keypad = new KeypadController(mockRowPins, mockColPins);
}

void tearDown() {
    delete keypad;
}

// Test Case 1: Initialization
void test_keypad_init() {
    TEST_ASSERT_NOT_NULL(keypad);
    // Verify pin modes set correctly (INPUT_PULLUP for cols, OUTPUT for rows)
}

// Test Case 2: Single key detection
void test_key_1_detected() {
    // Simulate matrix state: ROW1=LOW, COL1=LOW
    KeypadKey key = keypad->scan();
    TEST_ASSERT_EQUAL(KEY_1, key);
}

// Test Case 3: Debounce timing
void test_debounce_prevents_double_press() {
    // Press key at t=0
    KeypadKey key1 = keypad->scan();
    TEST_ASSERT_EQUAL(KEY_5, key1);
    
    // Immediate re-press at t=10ms (within debounce window)
    delay(10);
    KeypadKey key2 = keypad->scan();
    TEST_ASSERT_EQUAL(KEY_NONE, key2);  // Ignored
    
    // Wait debounce period
    delay(50);
    KeypadKey key3 = keypad->scan();
    TEST_ASSERT_EQUAL(KEY_5, key3);  // Accepted
}

// Test Case 4: D-pad keys
void test_dpad_up_down_left_right() {
    // Test all 4 directional keys
    TEST_ASSERT_EQUAL(KEY_UP, simulateKeyPress(10, 7));
    TEST_ASSERT_EQUAL(KEY_DOWN, simulateKeyPress(10, 9));
    TEST_ASSERT_EQUAL(KEY_LEFT, simulateKeyPress(10, A5));
    TEST_ASSERT_EQUAL(KEY_RIGHT, simulateKeyPress(10, 8));
}

// Test Case 5: Ghost key suppression
void test_no_ghost_keys() {
    // Simulate simultaneous press: KEY_1 (R1/C1) + KEY_5 (R2/C2)
    // Should NOT detect ghost at (R1/C2) or (R2/C1)
    // Implementation depends on N-key rollover algorithm
}

void process() {
    UNITY_BEGIN();
    
    RUN_TEST(test_keypad_init);
    RUN_TEST(test_key_1_detected);
    RUN_TEST(test_debounce_prevents_double_press);
    RUN_TEST(test_dpad_up_down_left_right);
    RUN_TEST(test_no_ghost_keys);
    
    UNITY_END();
}

void setup() {
    delay(2000);  // Wait for serial
    process();
}

void loop() {}
```

**Run tests**: `pio test -e uno`

---

### Phase 3: Integration Testing con Countdown (1 ora)

#### Test Scenario: Edit Countdown via Keypad

```cpp
// Manual test procedure:
void testCountdownEdit() {
    // 1. Start in CLOCK mode
    Serial.println("Press KEY_A to enter COUNTDOWN_EDIT...");
    // User action: press KEY_A
    
    // Expected: Display shows cursor blink su D1
    
    // 2. Navigate to D4 (hours ones)
    Serial.println("Press RIGHT 3 times...");
    // User action: RIGHT, RIGHT, RIGHT
    
    // Expected: Cursor at D4
    
    // 3. Set hours to "1" (increment from 0)
    Serial.println("Press UP 1 time...");
    // User action: UP
    
    // Expected: Display shows "01:00"
    
    // 4. Move to D3 (minutes tens)
    Serial.println("Press LEFT 1 time...");
    // User action: LEFT
    
    // Expected: Cursor at D3
    
    // 5. Set minutes to "5" (tens)
    Serial.println("Press UP 5 times...");
    // User action: UP × 5
    
    // Expected: Display shows "01:50"
    
    // 6. Move to D2 (minutes ones)
    Serial.println("Press LEFT 1 time...");
    // User action: LEFT
    
    // Expected: Cursor at D2
    
    // 7. Set minutes to "9" (ones)
    Serial.println("Press UP 9 times...");
    // User action: UP × 9
    
    // Expected: Display shows "01:59"
    
    // 8. Exit to CLOCK mode
    Serial.println("Press KEY_A to exit...");
    // User action: KEY_A
    
    // Expected: Mode = CLOCK, countdown value stored (01:59)
}
```

**Success criteria**: 
- Tutte le operazioni keypad reflected in display
- Countdown value finale = 01:59
- No glitches durante edit sequence
- Cursor navigation wrap-around funziona

---

### Phase 4: Stress & Performance Testing (1 ora)

#### Test 4.1: Scan Latency Measurement

```cpp
void testScanLatency() {
    unsigned long totalLatency = 0;
    const uint16_t samples = 1000;
    
    for (uint16_t i = 0; i < samples; i++) {
        // Simulate key press (mock GPIO state)
        unsigned long start = micros();
        KeypadKey key = keypad->scan();
        unsigned long latency = micros() - start;
        
        if (key != KEY_NONE) {
            totalLatency += latency;
        }
    }
    
    unsigned long avgLatency = totalLatency / samples;
    Serial.print(F("Avg scan latency: "));
    Serial.print(avgLatency);
    Serial.println(F(" us"));
    
    // Acceptance: avg < 500µs, max < 2ms
    TEST_ASSERT_LESS_THAN(500, avgLatency);
}
```

#### Test 4.2: Memory Leak Check

```cpp
void testMemoryStability() {
    extern int freeRam();  // From main.cpp
    
    int ramBefore = freeRam();
    
    // Simulate 10,000 key presses
    for (uint16_t i = 0; i < 10000; i++) {
        keypad->scan();
        if (i % 100 == 0) {
            int ramNow = freeRam();
            Serial.print(F("RAM @ "));
            Serial.print(i);
            Serial.print(F(": "));
            Serial.println(ramNow);
            
            // Check no significant leak
            TEST_ASSERT_INT_WITHIN(10, ramBefore, ramNow);
        }
    }
    
    int ramAfter = freeRam();
    Serial.print(F("RAM delta: "));
    Serial.println(ramAfter - ramBefore);
    
    // Acceptance: delta < 20 bytes (allow some variance)
    TEST_ASSERT_INT_WITHIN(20, ramBefore, ramAfter);
}
```

---

## 📚 Teoria Hardware/Software

### 🔌 Hardware: Matrix Keypad Principle

#### Anatomia Keypad Membrana 4×4

```
Physical Layout (vista frontale):

  ┌───┬───┬───┬───┐
  │ 1 │ 2 │ 3 │ A │  ← ROW 1 (pin 13)
  ├───┼───┼───┼───┤
  │ 4 │ 5 │ 6 │ B │  ← ROW 2 (pin 12)
  ├───┼───┼───┼───┤
  │ 7 │ 8 │ 9 │ C │  ← ROW 3 (pin 11)
  ├───┼───┼───┼───┤
  │ * │ 0 │ # │ D │  ← ROW 4 (pin 10)
  └───┴───┴───┴───┘
    ↑   ↑   ↑   ↑
  COL1 COL2 COL3 COL4
  (A5) (9) (8) (7)

Internal wiring (vista retro):

ROW1 ──────●────●────●────●────  (pin 13)
          key1 key2 key3 keyA
          
ROW2 ──────●────●────●────●────  (pin 12)
          key4 key5 key6 keyB
          
ROW3 ──────●────●────●────●────  (pin 11)
          key7 key8 key9 keyC
          
ROW4 ──────●────●────●────●────  (pin 10)
          keyL keyD keyR keyU
          
          │    │    │    │
          COL1 COL2 COL3 COL4
          (A5) (9) (8) (7)

● = switch (normally open, closes when pressed)
```

#### Pull-Up Resistors Theory

```
Perché INPUT_PULLUP?

Senza pull-up (FLOATING INPUT - ERRORE!):
        5V ────┐
               │
          KEY (switch)
               │
        COL ───┼───────► INPUT (voltage undefined quando open!)
               │
              ???  (floating → random noise detected)

Con pull-up interno (CORRETTO):
        5V ────┬──── R_pullup (~20-50kΩ internal)
               │
          KEY (switch)
               │
        COL ───┼───────► INPUT
               │
              GND (row LOW quando scan)

Stati logici:
- Key NOT pressed: INPUT = HIGH (pulled up by resistor)
- Key pressed: INPUT = LOW (connected to row=LOW)

Vantaggi internal pull-up (vs external resistors):
✅ No extra componenti esterni needed
✅ R value OK per most applications (20-50kΩ)
✅ Abilitate via pinMode(pin, INPUT_PULLUP)
⚠️ Higher R → slower rise time (OK per keypad, not OK per high-speed signals)
```

#### Matrix Scan Algorithm (Step-by-Step)

```cpp
// Initialization:
void KeypadController::begin() {
    // Rows: OUTPUT (we drive these)
    for (uint8_t i = 0; i < 4; i++) {
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], HIGH);  // Default HIGH (inactive)
    }
    
    // Cols: INPUT_PULLUP (we read these)
    for (uint8_t i = 0; i < 4; i++) {
        pinMode(colPins[i], INPUT_PULLUP);  // Enable internal pull-up
    }
}

// Scanning (ogni loop):
KeypadKey KeypadController::scan() {
    // Iterate through all 4 rows
    for (uint8_t row = 0; row < 4; row++) {
        // Step 1: Activate this row (set LOW)
        digitalWrite(rowPins[row], LOW);
        
        // Step 2: Small delay per GPIO stabilization
        // (parasitic capacitance needs ~1-10µs per charge/discharge)
        delayMicroseconds(10);
        
        // Step 3: Read all 4 columns
        for (uint8_t col = 0; col < 4; col++) {
            if (digitalRead(colPins[col]) == LOW) {
                // Key detected at (row, col)!
                
                // Step 4: Map (row, col) → KeypadKey enum
                KeypadKey key = mapKey(row, col);
                
                // Step 5: Debounce check
                if (isDebounced(key)) {
                    digitalWrite(rowPins[row], HIGH);  // Deactivate row
                    return key;
                }
            }
        }
        
        // Step 6: Deactivate this row (set HIGH)
        digitalWrite(rowPins[row], HIGH);
    }
    
    return KEY_NONE;  // No key detected
}
```

**Timing Analysis**:
- digitalWrite(): ~5µs per call (software overhead + AVR port manipulation)
- delayMicroseconds(10): 10µs busy-wait
- digitalRead(): ~3µs per call
- Total per row: 5 + 10 + (4×3) + 5 = 32µs
- Total scan (4 rows): 4 × 32 = **128µs** (best case, no key)

---

### 💻 Software: Debouncing Algorithm

#### Mechanical Switch Bouncing

```
Real-world key press voltage waveform:

5V ┐
   │                  ┌──────────────────  (stable HIGH)
   │   ╱╲   ╱╲  ╱╲   │
   │  ╱  ╲─╱  ╲╱  ╲──┘  (bounce: 5-20ms)
   │ ╱
0V └─────────────────────────────────────
   ▲                  ▲
   Initial contact   Stable LOW

Problema: Multiple edges durante bounce → multiple key events generated!

Soluzione: Software debouncing con timeout
```

#### Time-Based Debounce Implementation

```cpp
// State machine per ogni key:
class KeypadController {
private:
    unsigned long lastDebounceTime = 0;
    KeypadKey lastKey = KEY_NONE;
    bool keyConfirmed = false;
    
public:
    bool isDebounced(KeypadKey key) {
        unsigned long now = millis();
        
        if (key == lastKey) {
            // Same key detected again
            if (!keyConfirmed) {
                // Check se debounce window passed
                if (now - lastDebounceTime >= DEBOUNCE_DELAY) {
                    keyConfirmed = true;  // Stable! Confirm event
                    return true;
                }
            }
            // Still within debounce window OR already confirmed
            return false;
        } else {
            // New key detected (or KEY_NONE)
            lastKey = key;
            lastDebounceTime = now;
            keyConfirmed = false;  // Reset confirmation
            return false;
        }
    }
};

// Usage in scan():
if (digitalRead(colPins[col]) == LOW) {
    KeypadKey key = mapKey(row, col);
    if (isDebounced(key)) {
        return key;  // Confirmed, stable key press
    }
}
```

**Parametri critici**:
- `DEBOUNCE_DELAY = 50ms`: Buon tradeoff
  - Too short (<10ms): Bounce può passare attraverso
  - Too long (>100ms): Perceived input lag, utente frustrato
- Commercial keyboards: 5-10ms (high quality switches)
- Cheap membrane keypads: 20-50ms (this project: 50ms conservative)

---

### 🧮 Software: Key Mapping (Matrix Coordinates → Enum)

#### Mapping Table

```cpp
// KeypadKey enum (in KeypadController.h):
enum KeypadKey {
    KEY_NONE = 0,
    
    // Digits 0-9
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, 
    KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    
    // Letters A-D
    KEY_A, KEY_B, KEY_C, KEY_D,
    
    // D-pad navigation
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
};

// Mapping function:
KeypadKey KeypadController::mapKey(uint8_t row, uint8_t col) {
    // 2D lookup table (ROM-efficient):
    static const KeypadKey keyMap[4][4] PROGMEM = {
        // COL:  0        1        2        3
        { KEY_1,    KEY_2,    KEY_3,    KEY_A    },  // ROW 0 (pin 13)
        { KEY_4,    KEY_5,    KEY_6,    KEY_B    },  // ROW 1 (pin 12)
        { KEY_7,    KEY_8,    KEY_9,    KEY_C    },  // ROW 2 (pin 11)
        { KEY_LEFT, KEY_DOWN, KEY_RIGHT, KEY_UP  }   // ROW 3 (pin 10)
    };
    
    // PROGMEM read (AVR-specific):
    return (KeypadKey)pgm_read_byte(&keyMap[row][col]);
}
```

**Memory optimization**:
- `PROGMEM` macro: Store lookup table in Flash invece di RAM
- 16 bytes table (4×4 = 16 enum values)
- Saved: 16 bytes RAM → critical su ATmega328P!
- Cost: `pgm_read_byte()` call ~5-10µs (trascurabile)

---

### 🔬 Software: Ghost Key Problem & N-Key Rollover

#### Ghost Key Phenomenon

```
Scenario: Simultaneous press di 2 keys
User presses: KEY_1 (R0/C0) + KEY_5 (R1/C1)

Matrix state durante scan:

        COL0   COL1   COL2   COL3
         │      │      │      │
ROW0 ────●──────┼──────┼──────┼──  (R0 LOW, C0 LOW detected → KEY_1 ✓)
         │      │      │      │
ROW1 ────┼──────●──────┼──────┼──  (R1 LOW, C1 LOW detected → KEY_5 ✓)
         │      │      │      │

PROBLEMA: Cross-talk via current path!

        COL0   COL1
         │      │
ROW0 ────●──────X──────  (R0 LOW)
         │      │
ROW1 ────X──────●──────  (R1 LOW)

Current path: R0 → C0 → user fingers (!) → C1 → R1 → GND
Result: Ghost keys @ (R0/C1) e (R1/C0) erroneously detected!
```

**Soluzioni**:

1. **Diode Matrix** (hardware, costoso):
```
        COL0   COL1
         │      │
ROW0 ────●─┤>───X──────  (diode blocks reverse current)
         │      │
ROW1 ────X──────●─┤>───
```
   - ✅ Elimina ghost keys completamente
   - ⚠️ Costo: 16 diodes (1N4148) + assembly complexity
   - 💰 Not used in this project (budget constraint)

2. **Software N-Key Rollover Limit** (questo progetto):
```cpp
// Detect ONLY first key pressed, ignore simultaneous presses
KeypadKey KeypadController::scan() {
    uint8_t keysDetected = 0;
    KeypadKey firstKey = KEY_NONE;
    
    for (row...) {
        for (col...) {
            if (detected) {
                keysDetected++;
                if (keysDetected == 1) {
                    firstKey = mapKey(row, col);
                } else {
                    // Multiple keys → ambiguous, ignore all
                    return KEY_NONE;
                }
            }
        }
    }
    
    return firstKey;  // Return only if exactly 1 key
}
```
   - ✅ Semplice, no extra hardware
   - ⚠️ Limita input to 1 key at a time (OK per questo use case)
   - ⚠️ Chord inputs impossibili (e.g., Ctrl+C)

---

### 🔌 Hardware: Wokwi Keypad Simulation

Wokwi simula keypad con interactive buttons in `diagram.json`:

```json
{
  "parts": [
    {
      "type": "wokwi-membrane-keypad",
      "id": "keypad1",
      "top": 20,
      "left": 200,
      "attrs": {
        "keys": "123A456B789C*0#D"
      }
    }
  ],
  "connections": [
    ["keypad1:R1", "uno:13", "red", []],
    ["keypad1:R2", "uno:12", "red", []],
    ["keypad1:R3", "uno:11", "red", []],
    ["keypad1:R4", "uno:10", "red", []],
    ["keypad1:C1", "uno:A5", "blue", []],
    ["keypad1:C2", "uno:9", "blue", []],
    ["keypad1:C3", "uno:8", "blue", []],
    ["keypad1:C4", "uno:7", "blue", []]
  ]
}
```

**Wokwi simulator features**:
- ✅ Click keys con mouse → simulate physical press
- ✅ Visual feedback (key highlighted quando pressed)
- ✅ Matrix scan timing accurate (micros() precise)
- ⚠️ NO mechanical bouncing simulato (perfect switches)
  - Implication: Debouncing logic testabile ma non necessario in sim
  - Hardware reale: Bouncing presente, debouncing CRITICO

---

## 📝 Notes & Considerations

### Memory Impact
- KeypadController class: ~20 bytes RAM (state variables)
- Flash footprint: ~600 bytes (scan logic + mapping)
- Total: +1% RAM, +2% Flash (acceptable)

### Wiring Checklist (Hardware Build)
```
Arduino Uno ↔ Keypad Membrana:
✓ Pin 13 → ROW1 (rosso)
✓ Pin 12 → ROW2 (rosso)
✓ Pin 11 → ROW3 (rosso)
✓ Pin 10 → ROW4 (rosso)
✓ Pin A5 → COL1 (blu)
✓ Pin 9  → COL2 (blu)
✓ Pin 8  → COL3 (blu)
✓ Pin 7  → COL4 (blu)

Note: No external resistors needed (internal pull-ups used)
```

### Known Issues / Workarounds
- **Issue**: Some cheap keypads have inconsistent switch quality
  - **Symptom**: Missed key presses, intermittent detection
  - **Workaround**: Increase DEBOUNCE_DELAY to 100ms, or replace keypad

- **Issue**: Wokwi non simula bouncing
  - **Impact**: Debouncing logic not fully testable in simulation
  - **Mitigation**: Final validation MUST be done su hardware reale

---

## 🔗 References

### Project Documentation
- `EPIC_03.md`: KeypadController specification
- `Utilities/KeypadController.{h,cpp}`: Implementation files
- `diagram.json`: Wokwi keypad wiring schematic

### External Resources
- [Matrix Keypad Tutorial](https://www.circuitbasics.com/how-to-set-up-a-keypad-on-an-arduino/): Basic theory
- [Membrane Keypad Datasheet](https://www.adafruit.com/product/419): 4×4 membrane specs
- [AVR GPIO Speed](https://www.nongnu.org/avr-libc/user-manual/group__avr__io.html): digitalWrite timing

### Test Framework
- Unity Test Framework: [PlatformIO Unity Docs](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html)
- Mocking GPIO: Consider ArduinoFake library for isolated testing

---

## 📋 Checklist Completamento

### Phase 1: Manual Testing
- [ ] All 16 keys detected in Wokwi (visual + serial)
- [ ] No spurious events (debouncing effective)
- [ ] D-pad navigation confirmed (UP/DOWN/LEFT/RIGHT)
- [ ] Mode toggle (KEY_A) funziona
- [ ] Test matrix table 100% filled

### Phase 2: Automated Testing
- [ ] Unit tests scritti (5+ test cases)
- [ ] All tests pass: `pio test -e uno`
- [ ] Code coverage >80% (scan + debounce logic)

### Phase 3: Integration Testing
- [ ] Countdown edit workflow completed
- [ ] Keypad + Display integration stable (no flickering)
- [ ] Cursor navigation + digit increment tested end-to-end

### Phase 4: Performance Testing
- [ ] Scan latency <500µs avg, <2ms max
- [ ] Memory leak check passed (RAM stable after 10k scans)
- [ ] Stress test 100 keys @ 5 keys/sec, 100% detection

### Documentation
- [ ] Test results documentati (pass/fail per scenario)
- [ ] Known issues section updated
- [ ] Performance metrics recorded

---

**Task Owner**: Antonio Cittadino  
**Reviewer**: Self + Hardware validation  
**Blocked By**: Task 001 (Display Flickering Fix)  
**Last Updated**: 19 Dicembre 2025  
**Next Review**: Post Task 001 completion
