# Task #003: Countdown Controller Testing

**ID**: 003  
**Titolo**: Test Completo CountdownController - Digit Validation & Cursor Navigation  
**Tipo**: 🧪 TEST  
**Priorità**: 🟡 HIGH  
**Stato**: ⏳ BLOCKED (by Task 001)  
**Milestone**: M1 — MVP Stabile  

**Stima iniziale**: 4-6 ore  
**Tempo effettivo**: TBD  
**Assegnato a**: Team Core  
**Data creazione**: 19 Dicembre 2025  
**Dipendenze**: Task 001 (Display Flickering Fix), Task 002 (Keypad Testing)  

---

## 📋 Descrizione

Testare logica `CountdownController` per edit/validation countdown time (HH:MM format, max 23:59), cursor navigation (wrap-around ciclico D1↔D2↔D3↔D4), e digit increment/decrement con constraints time-valid.

**Context**:
- CountdownController implementato in `Utilities/CountdownController.{h,cpp}`
- Interfaccia con DisplayController (cursor blink) + KeypadController (UP/DOWN/LEFT/RIGHT input)
- Constraints time validation: H1 ≤ 2, H2 ≤ 3 quando H1=2, M1 ≤ 5, M2 ≤ 9
- Controller compilato ma **mai testato** dopo integration

**Blocco**: Task 001 (display fix) e Task 002 (keypad validation) devono essere completati prima.

---

## 🎯 Obiettivi

### Obiettivo Primario
Validare funzionamento corretto di countdown edit logic con constraints time HH:MM (00:00 - 23:59) e cursor navigation ciclica.

### Obiettivi Secondari
- [ ] Test cursor navigation con wrap-around (D1→D2→D3→D4→D1)
- [ ] Validare digit increment/decrement con time constraints
- [ ] Verificare edge cases: max values (23:59), min values (00:00), overflow wrap
- [ ] Testare cursor blink visual (500ms period)
- [ ] Integration con DisplayController + KeypadController
- [ ] Stress test rapid input sequence

---

## ✅ Acceptance Criteria (BDD)

### Scenario 1: Cursor Navigation - LEFT/RIGHT Movement

```gherkin
Feature: Cursor Navigation Ciclica
  As a user
  I want navigare tra digit positions con LEFT/RIGHT keys
  So that posso selezionare quale cifra editare

  Background:
    Given COUNTDOWN_EDIT mode attivo
    And Display showing "00:00" (initial state)
    And Cursor visible @ D1 (hours tens, leftmost)

  Scenario: Move RIGHT attraverso tutti digit
    Given Cursor @ D1 (hours tens)
    When I press RIGHT key
    Then Cursor moves to D2 (hours ones)
    And Display cursor blinks @ D2
    
    When I press RIGHT key again
    Then Cursor moves to D3 (minutes tens)
    And Display cursor blinks @ D3
    
    When I press RIGHT key again
    Then Cursor moves to D4 (minutes ones)
    And Display cursor blinks @ D4

  Scenario: Wrap-around RIGHT (D4 → D1)
    Given Cursor @ D4 (rightmost position, minutes ones)
    When I press RIGHT key
    Then Cursor wraps to D1 (leftmost position, hours tens)
    And Serial output: "Cursor wrap RIGHT: D4 → D1"

  Scenario: Move LEFT attraverso tutti digit (reverse)
    Given Cursor @ D4 (minutes ones)
    When I press LEFT key
    Then Cursor moves to D3 (minutes tens)
    
    When I press LEFT key
    Then Cursor moves to D2 (hours ones)
    
    When I press LEFT key
    Then Cursor moves to D1 (hours tens)

  Scenario: Wrap-around LEFT (D1 → D4)
    Given Cursor @ D1 (leftmost position)
    When I press LEFT key
    Then Cursor wraps to D4 (rightmost position)
    And Serial output: "Cursor wrap LEFT: D1 → D4"
```

### Scenario 2: Digit Increment/Decrement - Hours Tens (D1)

```gherkin
Feature: Hours Tens Digit (D1) Validation
  As a system
  I want D1 limitato a 0-2 (max hours = 23)
  So that countdown time sempre valido

  Background:
    Given Cursor @ D1 (hours tens)
    And Current value D1 = 0

  Scenario: Increment D1 da 0 a 2
    When I press UP key
    Then D1 increments to 1
    And Display shows "1X:XX"
    
    When I press UP key again
    Then D1 increments to 2
    And Display shows "2X:XX"

  Scenario: Max constraint D1 = 2 (no overflow)
    Given D1 = 2
    When I press UP key
    Then D1 wraps to 0 (circular increment)
    And Display shows "0X:XX"
    And Serial output: "D1 wrap: 2 → 0"

  Scenario: Decrement D1 da 2 a 0
    Given D1 = 2
    When I press DOWN key
    Then D1 decrements to 1
    And Display shows "1X:XX"
    
    When I press DOWN key again
    Then D1 decrements to 0
    And Display shows "0X:XX"

  Scenario: Min constraint D1 = 0 (wrap to max)
    Given D1 = 0
    When I press DOWN key
    Then D1 wraps to 2 (circular decrement)
    And Display shows "2X:XX"
```

### Scenario 3: Digit Increment/Decrement - Hours Ones (D2) con Dependency D1

```gherkin
Feature: Hours Ones Digit (D2) Validation con Constraint D1-Dependent
  As a system
  I want D2 max value dipenda da D1 (se D1=2 → D2 max=3, altrimenti D2 max=9)
  So that hours mai supera 23

  Background:
    Given Cursor @ D2 (hours ones)

  Scenario: D2 range quando D1 = 0 o 1 (max hours 19)
    Given D1 = 1
    And D2 = 0
    When I press UP key 9 volte
    Then D2 increments to 9
    And Display shows "19:XX"
    
    When I press UP key again (10th time)
    Then D2 wraps to 0
    And Display shows "10:XX"

  Scenario: D2 range quando D1 = 2 (max hours 23)
    Given D1 = 2
    And D2 = 0
    When I press UP key 3 volte
    Then D2 increments to 3
    And Display shows "23:XX"
    
    When I press UP key again (4th time)
    Then D2 wraps to 0 (constraint: max=3 quando D1=2)
    And Display shows "20:XX"
    And Serial output: "D2 wrap @ D1=2: 3 → 0"

  Scenario: D2 auto-clamp quando D1 cambia 1→2 e D2>3
    Given D1 = 1, D2 = 9 (displaying "19:XX")
    And Cursor @ D1
    When I press UP key (D1: 1 → 2)
    Then D2 automatically clamped to 3 (max valid per D1=2)
    And Display shows "23:XX"
    And Serial output: "D2 clamped: 9 → 3 (D1 changed to 2)"
```

### Scenario 4: Digit Increment/Decrement - Minutes Tens (D3)

```gherkin
Feature: Minutes Tens Digit (D3) Validation
  As a system
  I want D3 limitato a 0-5 (max minutes = 59)
  So that minutes sempre valido

  Background:
    Given Cursor @ D3 (minutes tens)
    And Current value D3 = 0

  Scenario: Increment D3 da 0 a 5
    When I press UP key 5 volte
    Then D3 increments to 5
    And Display shows "XX:5X"

  Scenario: Max constraint D3 = 5 (wrap to 0)
    Given D3 = 5
    When I press UP key
    Then D3 wraps to 0
    And Display shows "XX:0X"

  Scenario: Decrement D3 from 0 (wrap to max)
    Given D3 = 0
    When I press DOWN key
    Then D3 wraps to 5
    And Display shows "XX:5X"
```

### Scenario 5: Digit Increment/Decrement - Minutes Ones (D4)

```gherkin
Feature: Minutes Ones Digit (D4) Validation
  As a system
  I want D4 limitato a 0-9 (standard decimal digit)
  So that minutes ones sempre valido

  Background:
    Given Cursor @ D4 (minutes ones)
    And Current value D4 = 0

  Scenario: Increment D4 da 0 a 9
    When I press UP key 9 volte
    Then D4 increments to 9
    And Display shows "XX:X9"

  Scenario: Max constraint D4 = 9 (wrap to 0)
    Given D4 = 9
    When I press UP key
    Then D4 wraps to 0
    And Display shows "XX:X0"

  Scenario: Decrement D4 from 0 (wrap to max)
    Given D4 = 0
    When I press DOWN key
    Then D4 wraps to 9
    And Display shows "XX:X9"
```

### Scenario 6: Cursor Blink Visual

```gherkin
Feature: Cursor Blink Indicator
  As a user
  I want cursor blink @ 500ms period su digit corrente
  So that vedo chiaramente quale digit sto editando

  Background:
    Given COUNTDOWN_EDIT mode attivo
    And Display refresh @ 50Hz (multiplexing stabile)

  Scenario: Cursor blink timing
    Given Cursor @ D2
    When Timer 500ms elapses
    Then Display DP (decimal point) su D2 toggles (ON → OFF)
    
    When Timer 500ms elapses again
    Then DP toggles (OFF → ON)
    
    # Continue: blink alternates ogni 500ms

  Scenario: Cursor position change resets blink state
    Given Cursor @ D2 con DP ON (visible)
    When I press RIGHT key (cursor moves to D3)
    Then DP @ D2 turns OFF
    And DP @ D3 turns ON (blink reset, visible)
    And Blink timer resets (start new 500ms period)
```

### Scenario 7: Edge Cases - Max/Min Time Values

```gherkin
Feature: Time Boundary Validation
  As a system
  I want correct handling di edge case time values
  So that no invalid states possibili

  Scenario: Edit to max time 23:59
    Given Display showing "00:00"
    And Cursor @ D1
    When I press UP key 2 volte (D1 = 2)
    And I press RIGHT, then UP 3 volte (D2 = 3)
    And I press RIGHT, then UP 5 volte (D3 = 5)
    And I press RIGHT, then UP 9 volte (D4 = 9)
    Then Display shows "23:59"
    And All digits validated correctly

  Scenario: Edit to min time 00:00
    Given Display showing "12:34"
    And Cursor @ D1
    When I set all digits to 0 (via DOWN keys)
    Then Display shows "00:00"
    And All digits validated correctly

  Scenario: Overflow attempt 23:59 → increment any digit
    Given Display showing "23:59" (max time)
    And Cursor @ D1
    When I press UP key
    Then D1 wraps to 0 (result: 03:59, D2 auto-clamped)
    # Note: No "carry" logic (each digit independent wrap)
```

### Scenario 8: Integration con DisplayController

```gherkin
Feature: Display Integration
  As a CountdownController
  I want update DisplayController digit buffers quando values change
  So that display reflects current edited time

  Background:
    Given DisplayController initialized
    And CountdownController initialized con DisplayController reference

  Scenario: Digit change propagates to display
    Given Current time 10:00
    And Cursor @ D4 (minutes ones)
    When I press UP key (D4: 0 → 1)
    Then CountdownController calls displayTime(10, 1)
    And DisplayController updates digit buffer D4 = 1
    And Display shows "10:01" dopo next refresh()

  Scenario: Cursor blink via DisplayController API
    Given Cursor @ D2
    When CountdownController calls setColonBlink(true) @ 500ms
    Then DisplayController shows DP ON @ D2
    When CountdownController calls setColonBlink(false) @ 1000ms
    Then DisplayController shows DP OFF @ D2
```

---

## 🛠️ Test Plan

### Phase 1: Unit Testing - Cursor Logic (2 ore)

#### Test 1.1: Cursor Navigation

```cpp
// test/test_countdown_controller.cpp
#include <unity.h>
#include "CountdownController.h"
#include "MockDisplayController.h"  // Mock per isolated testing

MockDisplayController* mockDisplay;
CountdownController* countdown;

void setUp() {
    mockDisplay = new MockDisplayController();
    countdown = new CountdownController(mockDisplay);
}

void tearDown() {
    delete countdown;
    delete mockDisplay;
}

// Test: RIGHT navigation attraverso D1→D2→D3→D4
void test_cursor_right_navigation() {
    countdown->begin();  // Cursor @ D1 (initial)
    
    // Start @ D1
    TEST_ASSERT_EQUAL(0, countdown->getCursorPosition());  // D1 = position 0
    
    // RIGHT → D2
    countdown->moveCursorRight();
    TEST_ASSERT_EQUAL(1, countdown->getCursorPosition());
    
    // RIGHT → D3
    countdown->moveCursorRight();
    TEST_ASSERT_EQUAL(2, countdown->getCursorPosition());
    
    // RIGHT → D4
    countdown->moveCursorRight();
    TEST_ASSERT_EQUAL(3, countdown->getCursorPosition());
    
    // RIGHT → wrap to D1
    countdown->moveCursorRight();
    TEST_ASSERT_EQUAL(0, countdown->getCursorPosition());
}

// Test: LEFT navigation con wrap
void test_cursor_left_wrap() {
    countdown->begin();  // Cursor @ D1
    
    // LEFT from D1 → wrap to D4
    countdown->moveCursorLeft();
    TEST_ASSERT_EQUAL(3, countdown->getCursorPosition());  // D4 = position 3
    
    // LEFT → D3
    countdown->moveCursorLeft();
    TEST_ASSERT_EQUAL(2, countdown->getCursorPosition());
}

// Run all cursor tests
void process() {
    UNITY_BEGIN();
    RUN_TEST(test_cursor_right_navigation);
    RUN_TEST(test_cursor_left_wrap);
    UNITY_END();
}
```

#### Test 1.2: Digit Increment/Decrement D1 (Hours Tens)

```cpp
// Test D1: max=2, wrap 0↔2
void test_d1_increment_to_max() {
    countdown->setCursorPosition(0);  // D1
    countdown->setTime(0, 0);  // Start 00:00
    
    // Increment: 0 → 1 → 2
    countdown->incrementCurrentDigit();
    TEST_ASSERT_EQUAL(1, countdown->getHoursTens());
    
    countdown->incrementCurrentDigit();
    TEST_ASSERT_EQUAL(2, countdown->getHoursTens());
    
    // Increment @ max → wrap to 0
    countdown->incrementCurrentDigit();
    TEST_ASSERT_EQUAL(0, countdown->getHoursTens());
}

void test_d1_decrement_wrap() {
    countdown->setCursorPosition(0);  // D1
    countdown->setTime(0, 0);  // D1 = 0
    
    // Decrement from 0 → wrap to 2
    countdown->decrementCurrentDigit();
    TEST_ASSERT_EQUAL(2, countdown->getHoursTens());
}
```

#### Test 1.3: Digit D2 (Hours Ones) con D1 Dependency

```cpp
// Test D2: max=9 se D1<2, max=3 se D1=2
void test_d2_max_depends_on_d1() {
    countdown->setCursorPosition(1);  // D2
    
    // Case 1: D1=1 → D2 max=9
    countdown->setTime(1, 0);  // 10:00
    countdown->setHoursOnes(9);  // D2 = 9
    countdown->incrementCurrentDigit();
    TEST_ASSERT_EQUAL(0, countdown->getHoursOnes());  // Wrap 9 → 0
    
    // Case 2: D1=2 → D2 max=3
    countdown->setTime(2, 0);  // 20:00
    countdown->setHoursOnes(3);  // D2 = 3
    countdown->incrementCurrentDigit();
    TEST_ASSERT_EQUAL(0, countdown->getHoursOnes());  // Wrap 3 → 0 (not 4!)
}

// Test D2 auto-clamp quando D1 changes
void test_d2_clamp_on_d1_change() {
    countdown->setTime(1, 9);  // 19:XX (valid)
    
    // Change D1: 1 → 2 (now max hours = 23, so D2 must be ≤ 3)
    countdown->setCursorPosition(0);  // D1
    countdown->incrementCurrentDigit();  // D1: 1 → 2
    
    // D2 should auto-clamp: 9 → 3
    TEST_ASSERT_EQUAL(3, countdown->getHoursOnes());
}
```

#### Test 1.4: Minutes Validation (D3, D4)

```cpp
// Test D3: max=5 (minutes tens)
void test_d3_max_5() {
    countdown->setCursorPosition(2);  // D3
    countdown->setTime(0, 50);  // XX:50, D3=5
    
    countdown->incrementCurrentDigit();
    TEST_ASSERT_EQUAL(0, countdown->getMinutesTens());  // Wrap 5 → 0
}

// Test D4: max=9 (minutes ones)
void test_d4_max_9() {
    countdown->setCursorPosition(3);  // D4
    countdown->setTime(0, 9);  // XX:X9, D4=9
    
    countdown->incrementCurrentDigit();
    TEST_ASSERT_EQUAL(0, countdown->getMinutesOnes());  // Wrap 9 → 0
}
```

**Run unit tests**: `pio test -e uno`

---

### Phase 2: Manual Integration Testing (2 ore)

#### Test 2.1: Edit Sequence "00:00 → 23:59"

```
Procedura manuale (Wokwi + Serial Monitor):

1. Start in CLOCK mode
   - Press KEY_A → enter COUNTDOWN_EDIT
   - Expected: Display "00:00", cursor blink @ D1

2. Set D1 = 2 (hours tens)
   - Cursor @ D1
   - Press UP 2 volte
   - Expected: Display "20:00"

3. Set D2 = 3 (hours ones)
   - Press RIGHT (cursor → D2)
   - Press UP 3 volte
   - Expected: Display "23:00"

4. Set D3 = 5 (minutes tens)
   - Press RIGHT (cursor → D3)
   - Press UP 5 volte
   - Expected: Display "23:50"

5. Set D4 = 9 (minutes ones)
   - Press RIGHT (cursor → D4)
   - Press UP 9 volte
   - Expected: Display "23:59"

6. Verify overflow wrap
   - Press UP 1 volta (D4: 9 → 0)
   - Expected: Display "23:50" (D4 wrapped, no carry)

7. Exit COUNTDOWN_EDIT
   - Press KEY_A
   - Expected: Mode → CLOCK, countdown value stored = 23:59

Pass/Fail: ☐ PASS  ☐ FAIL (note issues)
```

#### Test 2.2: Cursor Wrap-Around Visual

```
Procedura:

1. Enter COUNTDOWN_EDIT, cursor @ D1
2. Press LEFT key
   - Expected: Cursor wraps to D4, DP blinks @ rightmost digit
3. Press RIGHT key
   - Expected: Cursor wraps back to D1
4. Press RIGHT 4 volte rapidamente
   - Expected: Cursor cycles D1→D2→D3→D4→D1, no glitches

Pass/Fail: ☐ PASS  ☐ FAIL
```

#### Test 2.3: D2 Clamp Validation (Critical Edge Case)

```
Procedura:

1. Enter COUNTDOWN_EDIT
2. Set time to 19:XX (D1=1, D2=9)
   - Cursor @ D1, press UP 1 volta → D1=1
   - RIGHT, press UP 9 volte → D2=9
   - Expected: Display "19:XX"

3. Move back to D1
   - Press LEFT key (cursor → D2)
   - Press LEFT key (cursor → D1)

4. Increment D1: 1 → 2
   - Press UP key
   - Expected: D1=2, D2 auto-clamped to 3
   - Display should show "23:XX" (NOT "29:XX"!)

5. Verify serial output
   - Serial should print: "D2 clamped: 9 → 3 (D1=2 constraint)"

Pass/Fail: ☐ PASS  ☐ FAIL (CRITICAL test!)
```

---

### Phase 3: Cursor Blink Visual Verification (1 ora)

#### Test 3.1: Blink Timing Measurement

```cpp
// Add debug code in CountdownController:
void CountdownController::updateCursorBlink() {
    if (millis() - lastBlinkTime >= 500) {
        cursorVisible = !cursorVisible;
        display->setColonBlink(cursorVisible);
        
        #ifdef DEBUG_BLINK
        Serial.print(F("Cursor blink @ "));
        Serial.print(millis());
        Serial.print(F(", state: "));
        Serial.println(cursorVisible ? "ON" : "OFF");
        #endif
        
        lastBlinkTime = millis();
    }
}
```

**Procedura**:
1. Build con `#define DEBUG_BLINK` enabled
2. Enter COUNTDOWN_EDIT mode
3. Observe serial output per 10 secondi
4. Measure blink intervals (should be ~500ms ±10ms)

**Expected output**:
```
Cursor blink @ 1000, state: ON
Cursor blink @ 1500, state: OFF
Cursor blink @ 2000, state: ON
Cursor blink @ 2500, state: OFF
...
```

#### Test 3.2: Visual Inspection Wokwi

```
Manual observation:
1. Enter COUNTDOWN_EDIT
2. Stare at display per 30 secondi
3. Check:
   ☐ DP blinks smoothly (no stuttering)
   ☐ Blink rate feels like ~2 Hz (500ms period)
   ☐ Cursor position matches selected digit
   ☐ Moving cursor resets blink state immediately
```

---

### Phase 4: Stress Testing (1 ora)

#### Test 4.1: Rapid Input Sequence

```cpp
// Automated test: simulate 100 rapid key presses
void stressTestRapidInput() {
    countdown->begin();
    
    for (uint16_t i = 0; i < 100; i++) {
        // Alternate UP/RIGHT keys
        if (i % 2 == 0) {
            countdown->incrementCurrentDigit();
        } else {
            countdown->moveCursorRight();
        }
        
        delay(50);  // 20 keys/sec (fast typing)
        
        // Verify state consistency every 10 iterations
        if (i % 10 == 0) {
            uint8_t h = countdown->getHours();
            uint8_t m = countdown->getMinutes();
            
            // Validate time range
            TEST_ASSERT_LESS_OR_EQUAL(23, h);
            TEST_ASSERT_LESS_OR_EQUAL(59, m);
        }
    }
    
    Serial.println(F("Stress test PASS: 100 operations, state valid"));
}
```

#### Test 4.2: Edge Case Boundary Cycling

```cpp
// Test: cycle through all 1440 possible times (00:00 - 23:59)
void testAllValidTimes() {
    for (uint8_t h = 0; h <= 23; h++) {
        for (uint8_t m = 0; m <= 59; m++) {
            countdown->setTime(h, m);
            
            // Validate internal state matches
            TEST_ASSERT_EQUAL(h, countdown->getHours());
            TEST_ASSERT_EQUAL(m, countdown->getMinutes());
            
            // Validate digit decomposition
            uint8_t d1 = h / 10;
            uint8_t d2 = h % 10;
            uint8_t d3 = m / 10;
            uint8_t d4 = m % 10;
            
            TEST_ASSERT_EQUAL(d1, countdown->getHoursTens());
            TEST_ASSERT_EQUAL(d2, countdown->getHoursOnes());
            TEST_ASSERT_EQUAL(d3, countdown->getMinutesTens());
            TEST_ASSERT_EQUAL(d4, countdown->getMinutesOnes());
        }
    }
    
    Serial.println(F("Validated all 1440 times: PASS"));
}
```

---

## 📚 Teoria Hardware/Software

### 💻 Software: Time Representation & Digit Decomposition

#### BCD (Binary-Coded Decimal) vs Binary Encoding

```cpp
// Time storage options:

// Option 1: Binary (compact, requires division per display)
struct TimeCompact {
    uint8_t hours;    // 0-23 (5 bits sufficient, use 8 for simplicity)
    uint8_t minutes;  // 0-59 (6 bits sufficient, use 8)
    // Total: 2 bytes RAM
};

// Option 2: BCD (separate digits, faster display update)
struct TimeBCD {
    uint8_t hoursTens;    // 0-2 (D1)
    uint8_t hoursOnes;    // 0-9 (D2, max 3 se hoursTens=2)
    uint8_t minutesTens;  // 0-5 (D3)
    uint8_t minutesOnes;  // 0-9 (D4)
    // Total: 4 bytes RAM
};

// This project uses: Binary storage + on-demand conversion
// Why: Compact storage (2 bytes), conversion only when display updates
```

#### Time Decomposition Algorithm

```cpp
// Convert binary time → digit positions
class CountdownController {
private:
    uint8_t hours;    // 0-23
    uint8_t minutes;  // 0-59
    
public:
    uint8_t getHoursTens() const {
        return hours / 10;  // Integer division: 23 / 10 = 2
    }
    
    uint8_t getHoursOnes() const {
        return hours % 10;  // Modulo: 23 % 10 = 3
    }
    
    uint8_t getMinutesTens() const {
        return minutes / 10;  // 59 / 10 = 5
    }
    
    uint8_t getMinutesOnes() const {
        return minutes % 10;  // 59 % 10 = 9
    }
};

// Performance:
// - Division/modulo su AVR: ~20-30 CPU cycles (integer, 8-bit)
// - Called only quando display update needed (~1/sec max)
// - Overhead trascurabile vs RAM saved (2 bytes vs 4 bytes)
```

#### Time Recomposition (Digit → Binary)

```cpp
// Reconstruct binary time da digit values
void CountdownController::updateTimeFromDigits() {
    hours = (hoursTens * 10) + hoursOnes;     // 2*10 + 3 = 23
    minutes = (minutesTens * 10) + minutesOnes; // 5*10 + 9 = 59
    
    // Validation (defensive programming):
    if (hours > 23) hours = 23;   // Clamp to max
    if (minutes > 59) minutes = 59;
}
```

---

### 🧮 Software: Time Validation Constraints

#### Digit Constraints Table

| Digit | Position | Name | Min | Max | Max depends on | Notes |
|-------|----------|------|-----|-----|----------------|-------|
| D1 | 0 | Hours Tens | 0 | 2 | - | H1 ∈ {0,1,2} |
| D2 | 1 | Hours Ones | 0 | 9 (se D1<2)<br>3 (se D1=2) | D1 | H2 max = (D1==2) ? 3 : 9 |
| D3 | 2 | Minutes Tens | 0 | 5 | - | M1 ∈ {0,1,2,3,4,5} |
| D4 | 3 | Minutes Ones | 0 | 9 | - | M2 ∈ {0,1,2,3,4,5,6,7,8,9} |

#### Validation Logic Implementation

```cpp
// D1: Hours tens (0-2)
void CountdownController::incrementD1() {
    hoursTens = (hoursTens + 1) % 3;  // Wrap: 0→1→2→0
    
    // Cascade check: se D1=2, clamp D2 to max 3
    if (hoursTens == 2 && hoursOnes > 3) {
        hoursOnes = 3;  // Auto-clamp!
    }
    
    updateTimeFromDigits();
}

// D2: Hours ones (0-9 or 0-3 depending on D1)
void CountdownController::incrementD2() {
    uint8_t maxD2 = (hoursTens == 2) ? 3 : 9;  // Conditional max
    
    hoursOnes = (hoursOnes + 1) % (maxD2 + 1);  // Wrap 0→max→0
    
    updateTimeFromDigits();
}

// D3: Minutes tens (0-5)
void CountdownController::incrementD3() {
    minutesTens = (minutesTens + 1) % 6;  // Wrap: 0→5→0
    updateTimeFromDigits();
}

// D4: Minutes ones (0-9)
void CountdownController::incrementD4() {
    minutesOnes = (minutesOnes + 1) % 10;  // Standard decimal wrap
    updateTimeFromDigits();
}
```

#### Edge Case: D1 Change Triggers D2 Clamp

```
User workflow che richiede clamping:

Step 1: Set time to 19:XX
   D1 = 1, D2 = 9 → hours = 19 ✓ (valid)

Step 2: User moves cursor back to D1

Step 3: User presses UP → D1: 1 → 2
   Now D1 = 2, D2 = 9 → hours = 29 ✗ (INVALID!)
   
Auto-fix: Clamp D2 to max valid value
   D2: 9 → 3 → hours = 23 ✓ (valid)
   
Implementation:
   if (hoursTens == 2 && hoursOnes > 3) {
       hoursOnes = 3;  // Cascade clamp
       Serial.println(F("D2 auto-clamped to 3"));
   }
```

---

### 💻 Software: Cursor State Machine

#### Cursor Position Enum

```cpp
enum CursorPosition {
    CURSOR_D1 = 0,  // Hours tens
    CURSOR_D2 = 1,  // Hours ones
    CURSOR_D3 = 2,  // Minutes tens
    CURSOR_D4 = 3   // Minutes ones
};

class CountdownController {
private:
    CursorPosition cursorPos = CURSOR_D1;  // Default start D1
    
public:
    void moveCursorRight() {
        cursorPos = (CursorPosition)((cursorPos + 1) % 4);  // Wrap 0→3→0
        resetBlinkState();  // Cursor move → blink visible immediately
    }
    
    void moveCursorLeft() {
        // Reverse wrap: 0→3, 3→2, 2→1, 1→0
        if (cursorPos == CURSOR_D1) {
            cursorPos = CURSOR_D4;  // Wrap left from D1 → D4
        } else {
            cursorPos = (CursorPosition)(cursorPos - 1);
        }
        resetBlinkState();
    }
};
```

#### Blink State Machine

```
State diagram:

       ┌─────────┐
       │ HIDDEN  │ ◄──────┐
       └────┬────┘        │
            │ 500ms timer │ 500ms timer
            ▼             │
       ┌─────────┐        │
       │ VISIBLE │ ───────┘
       └─────────┘

Events:
- Timer elapsed (500ms) → toggle state
- Cursor move (LEFT/RIGHT) → reset to VISIBLE
- Mode exit → force HIDDEN
```

```cpp
class CountdownController {
private:
    bool cursorVisible = true;  // Blink state
    unsigned long lastBlinkTime = 0;
    
public:
    void updateCursorBlink() {
        if (millis() - lastBlinkTime >= 500) {
            cursorVisible = !cursorVisible;
            
            // Update display DP @ cursor position
            display->setColonBlink(cursorVisible);
            
            lastBlinkTime = millis();
        }
    }
    
    void resetBlinkState() {
        cursorVisible = true;  // Always start visible dopo move
        lastBlinkTime = millis();  // Reset timer
        display->setColonBlink(true);
    }
};
```

---

### 🔬 Software: Integration Pattern con DisplayController

#### Separation of Concerns

```
Architecture:

┌─────────────────────────┐
│ CountdownController     │ ← Business logic: time, cursor, validation
│  - hours, minutes       │
│  - cursorPos            │
│  - increment/decrement  │
└───────────┬─────────────┘
            │ calls displayTime()
            ▼
┌─────────────────────────┐
│ DisplayController       │ ← Hardware abstraction: 74HC595, multiplexing
│  - digitBuffers[4]      │
│  - refresh()            │
│  - setColonBlink()      │
└─────────────────────────┘
            │ GPIO ops
            ▼
       [Hardware: 74HC595 + LEDs]
```

#### API Contract

```cpp
// CountdownController → DisplayController communication:

class DisplayController {
public:
    // Update all digit buffers con time value
    void displayTime(uint8_t hours, uint8_t minutes);
    
    // Control DP (decimal point) blink per cursor indication
    void setColonBlink(bool visible);
    
    // Get current DP state (useful per toggle logic)
    bool getColonVisible() const;
};

// Usage in CountdownController:
void CountdownController::incrementCurrentDigit() {
    // 1. Update internal state (hours/minutes)
    switch (cursorPos) {
        case CURSOR_D1: incrementD1(); break;
        case CURSOR_D2: incrementD2(); break;
        case CURSOR_D3: incrementD3(); break;
        case CURSOR_D4: incrementD4(); break;
    }
    
    // 2. Propagate to display
    display->displayTime(hours, minutes);  // ← Single update call
}
```

**Why this pattern?**
- ✅ Single Responsibility: CountdownController non sa nulla di 74HC595
- ✅ Testability: Facile mockare DisplayController per unit tests
- ✅ Loose Coupling: Cambiare hardware display non richiede rewrite countdown logic

---

### 🔌 Hardware: Decimal Point (DP) per Cursor Indication

#### 7-Segment DP Bit

```
7-segment con DP:

       A
     ─────
    │     │
  F │  G  │ B
     ─────
    │     │
  E │     │ C
     ─────  • ← DP (decimal point, bit 7)
       D

Byte encoding:
Bit: 7  6  5  4  3  2  1  0
     DP G  F  E  D  C  B  A
     
Example: digit "5" con DP ON
  0b11101101 = 0xED
  (DP=1, G=1, F=1, E=0, D=1, C=1, B=0, A=1)
```

#### DP Control in DisplayController

```cpp
// Display "12:34" con cursor @ D2 (hours ones, "2")

uint8_t digitBuffers[4] = {
    encode('1') | 0x00,  // D1: "1" senza DP
    encode('2') | 0x80,  // D2: "2" con DP (bit 7 ON) ← CURSOR HERE
    encode('3') | 0x00,  // D3: "3" senza DP
    encode('4') | 0x00   // D4: "4" senza DP
};

// Toggle DP via setColonBlink():
void DisplayController::setColonBlink(bool visible) {
    uint8_t cursorDigit = countdown->getCursorPosition();
    
    if (visible) {
        digitBuffers[cursorDigit] |= 0x80;   // Set bit 7 (DP ON)
    } else {
        digitBuffers[cursorDigit] &= ~0x80;  // Clear bit 7 (DP OFF)
    }
}
```

**Alternative: Colon (:) between digits**
- Some displays have separate colon LEDs between D2:D3
- Controlled via additional GPIO pin (non-multiplexed)
- This project: Using DP for cursor indication (simpler wiring)

---

## 📝 Notes & Considerations

### Memory Impact
- CountdownController class: ~10 bytes RAM (hours, minutes, cursor state)
- Flash footprint: ~800 bytes (validation logic + digit operations)
- Total: +0.5% RAM, +2.5% Flash (acceptable)

### Known Issues / Edge Cases
1. **No "carry" logic between digits**:
   - Incrementing D4 from 9 → 0 does NOT increment D3
   - User must manually edit each digit
   - Rationale: Simpler logic, avoid unexpected time jumps

2. **D2 clamp può confondere utente**:
   - User sets 19:XX, increments D1 → suddenly display shows 23:XX
   - Mitigation: Serial debug message explains auto-clamp
   - Future: Beep sound feedback per clamp event

3. **Cursor blink può essere missed se display flickers** (Task 001):
   - Blink timing relies on stable display multiplexing
   - Test MUST wait per Task 001 resolution

### Design Decisions
- **Why 500ms blink rate?**
  - Human perception: 1-2 Hz comfortable per text cursor
  - Too fast (<200ms): Seizure risk, hard to track
  - Too slow (>1s): Appears frozen, confusing
  - 500ms (2 Hz): Standard keyboard cursor rate

- **Why circular wrap invece di limit?**
  - UP @ max → wrap to 0 (not stay @ max)
  - Consistent con clock behavior (23:59 → 00:00)
  - Faster navigation (can go "backwards" via forward wrap)

---

## 🔗 References

### Project Documentation
- `EPIC_03.md`: CountdownController specification
- `Utilities/CountdownController.{h,cpp}`: Implementation
- `Utilities/DisplayController.{h,cpp}`: Display API

### External Resources
- [BCD vs Binary Encoding](https://en.wikipedia.org/wiki/Binary-coded_decimal): Theory comparison
- [Modulo Operator AVR](https://www.avrfreaks.net/forum/modulo-operator-performance): Performance notes
- [Time Validation Algorithms](https://stackoverflow.com/questions/3224834/validating-time-format): Stack Overflow discussion

### Test Framework
- Unity: [Assertion macros](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityAssertionsReference.md)
- PlatformIO Testing: [Unit testing guide](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html)

---

## 📋 Checklist Completamento

### Phase 1: Unit Testing
- [ ] All cursor navigation tests pass
- [ ] Digit increment/decrement D1-D4 validated
- [ ] D2 clamp on D1 change tested (critical!)
- [ ] Edge cases (00:00, 23:59) validated
- [ ] Code coverage >90% (countdown logic)

### Phase 2: Manual Integration Testing
- [ ] Edit sequence 00:00 → 23:59 completed
- [ ] Cursor wrap-around visual verified
- [ ] D2 clamp scenario manually validated
- [ ] No glitches durante rapid input

### Phase 3: Cursor Blink Verification
- [ ] Blink timing measured (500ms ±10ms)
- [ ] Visual inspection: blink smooth, no stutter
- [ ] Cursor position change resets blink correctly

### Phase 4: Stress Testing
- [ ] Rapid input test (100 operations) passed
- [ ] All 1440 time values validated
- [ ] Memory stable (no leaks after stress)

### Documentation
- [ ] Test results logged (pass/fail per scenario)
- [ ] Known edge cases documented
- [ ] Integration notes updated

---

**Task Owner**: Antonio Cittadino  
**Reviewer**: Self + Integration validation  
**Blocked By**: Task 001 (Display Fix), Task 002 (Keypad Validation)  
**Last Updated**: 19 Dicembre 2025  
**Next Review**: Post Task 001+002 completion
