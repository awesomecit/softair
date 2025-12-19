# Task #001: Display Flickering Fix

**ID**: 001  
**Titolo**: Risoluzione Flickering Display 7-Segmenti dopo Integrazione Keypad  
**Tipo**: 🐛 BUG (Critical)  
**Priorità**: 🔴 CRITICAL (Blocker)  
**Stato**: 🔧 IN PROGRESS  
**Milestone**: M1 — MVP Stabile  

**Stima iniziale**: 8-12 ore  
**Tempo effettivo**: TBD  
**Assegnato a**: Team Core  
**Data creazione**: 19 Dicembre 2025  
**Data inizio**: 19 Dicembre 2025  
**Deadline**: 22 Dicembre 2025  

---

## 📋 Descrizione

Display 7-segmenti a 4 cifre mostra flickering (sfarfallio) visibile dopo integrazione EPIC_03 (KeypadController + CountdownController). Prima dell'integrazione, `DisplayController` standalone funzionava correttamente con multiplexing stabile a 50Hz.

**Context**: 
- DisplayController implementato in EPIC_02: multiplexing non-blocking, 74HC595 shift register, refresh() chiamato ogni loop
- EPIC_03 aggiunto: KeypadController (matrix scan), CountdownController (cursor logic), MODE selection logic
- **Regression**: Display stabile → Display flickering dopo integration

---

## 🎯 Obiettivi

### Obiettivo Primario
Eliminare flickering display garantendo multiplexing stabile a 50Hz (5ms per digit) anche con keypad scan e countdown logic attivi nel loop principale.

### Obiettivi Secondari
- [ ] Profilare timing loop principale (target <5ms loop time)
- [ ] Identificare root cause del flickering (keypad blocking vs display timing)
- [ ] Ottimizzare keypad scan se necessario (time-slicing o throttling)
- [ ] Validare fix in Wokwi simulation E hardware reale
- [ ] Documentare best practices per timing-critical integration

---

## 🔍 Analisi Problema

### Sintomi Osservati

```
User report (Conversation Summary):
"ora i led sembra che sfarfallino e sono quasi sempre accesi"
```

**Dettagli**:
1. Segmenti display appaiono sempre accesi invece di multiplexing pulito
2. Flickering visibile in simulazione Wokwi
3. Problema assente prima di EPIC_03 integration
4. Manifestazione: dopo aggiunta keypad scan, countdown cursor blink, MODE selection

### Timeline Regression

```mermaid
graph LR
    A[EPIC_02: Display OK] -->|Add KeypadController| B[Display + Keypad]
    B -->|Add CountdownController| C[Full Integration]
    C -->|Symptom Appears| D[FLICKERING VISIBLE]
    
    style A fill:#90EE90
    style D fill:#FF6B6B
```

### Root Cause Hypotheses

#### Hypothesis 1: Keypad Scan Blocking ⭐ (Most Likely)

**Theory**: `KeypadController::scan()` scansiona matrice 4×4 sequenzialmente (4 righe), ogni riga richiede GPIO operations. Tempo totale scan potenzialmente >1ms.

```cpp
// Pseudocodice current scan logic
KeypadKey KeypadController::scan() {
    for (uint8_t row = 0; row < 4; row++) {
        // 1. Set row LOW
        digitalWrite(rowPins[row], LOW);
        delayMicroseconds(10);  // ⚠️ Small delay per stabilize
        
        // 2. Read all 4 columns
        for (uint8_t col = 0; col < 4; col++) {
            if (digitalRead(colPins[col]) == LOW) {
                // Key detected, apply debounce...
            }
        }
        
        // 3. Reset row HIGH
        digitalWrite(rowPins[row], HIGH);
    }
    return detectedKey;
}
```

**Time estimate**: 
- 4 rows × (GPIO ops + 10µs delay + 4 column reads + debounce check)
- **Best case**: ~500µs
- **Worst case** (with debounce logic): 1-2ms

**Impact on display**:
- Display refresh() target: call every loop, ideally <5ms loop time
- If keypad scan takes 1-2ms, reduces available time for other operations
- Multiple scan() calls per digit refresh cycle → timing jitter → flickering

**Evidence supporting this hypothesis**:
- Display worked perfectly before keypad integration
- Flickering appeared immediately after `keypad->scan()` added to loop
- Disabling keypad scan (temporary workaround) eliminates flickering

#### Hypothesis 2: displayTime() Called Too Frequently

**Theory**: `displayTime()` recalcola digit buffers (division/modulo operations), dovrebbe essere chiamato SOLO quando time change, ma cursor blink logic potrebbe chiamarlo ogni 500ms.

```cpp
// WRONG pattern (if present):
if (millis() - lastCursorBlink >= 500) {
    displayTime(hours, minutes);  // ❌ Ricalcola TUTTO
    lastCursorBlink = millis();
}

// CORRECT pattern:
if (millis() - lastCursorBlink >= 500) {
    display->setColonBlink(!display->getColonVisible());  // ✅ Solo toggle state
    lastCursorBlink = millis();
}
```

**Impact**: CPU cycles sprecati su calculations, meno tempo per refresh() multiplexing.

**Evidence**:
- Conversation summary: "displayTime() chiamato troppo spesso durante cursor blink"
- Mode selection timeout chiamava displayTime() direttamente (fixed in iteration)

#### Hypothesis 3: Loop Timing Disruption

**Theory**: Combinazione keypad + countdown + mode logic aumenta loop time oltre 5ms, refresh() non chiamato abbastanza frequentemente.

**Target**: Loop rate 200Hz (5ms per loop) per multiplexing stabile (4 digit × 5ms = 20ms full cycle → 50Hz)

**Measurement needed**: Profiling con micros() per confermare loop time actual.

---

## ✅ Acceptance Criteria (BDD)

### Scenario 1: Display Stabile con Keypad Scan

```gherkin
Feature: Display Multiplexing con Keypad Attivo
  As a system integrator
  I want display refresh stable anche con keypad scan
  So that utente vede cifre nitide senza flickering

  Background:
    Given Display 7-segmenti connesso con 74HC595
    And KeypadController initialized con matrix 4×4
    And loop() calls refresh() as first operation

  Scenario: Keypad scan non causa flickering
    Given Loop attivo con keypad scan enabled
    When 100 loop cycles eseguiti
    Then Display mostra cifre stabili senza flickering
    And Loop time medio < 5ms
    And Keypad scan time < 1ms

  Scenario: Keypad scan con key press
    Given Tasto "5" premuto sul keypad
    When scan() rileva key press
    Then Display rimane stabile durante event handling
    And No visible flicker su digit attivi

  Scenario: Long-term stability test
    Given Sistema running con display + keypad
    When 1 ora elapsed time (720,000 loop cycles @ 200Hz)
    Then Display ancora stabile senza glitch
    And Nessun memory leak (RAM usage stable)
```

### Scenario 2: Loop Timing Profiled

```gherkin
Feature: Loop Timing Measurement
  As a developer
  I want accurate loop timing data
  So that posso identify performance bottlenecks

  Scenario: Baseline loop time without keypad
    Given Keypad scan disabled
    When 1000 loop iterations misurate
    Then Avg loop time < 2ms
    And Max loop time < 3ms

  Scenario: Loop time con keypad scan
    Given Keypad scan enabled
    When 1000 loop iterations misurate
    Then Avg loop time < 5ms
    And Max loop time < 8ms (con key press event)

  Scenario: Keypad scan profiling isolato
    Given Only scan() chiamato in test bench
    When 100 scan operations misurate
    Then Avg scan time < 800µs
    And Max scan time < 1.5ms
```

### Scenario 3: displayTime() Call Frequency

```gherkin
Feature: Display Update Optimization
  As a system
  I want displayTime() chiamato solo quando necessario
  So that CPU cycles non sprecati su redundant calculations

  Scenario: displayTime() solo su time change
    Given Time 10:00 displayed
    When 1 secondo passa
    Then displayTime(10, 0) chiamato 1 volta
    And Non chiamato durante 999ms intermedie

  Scenario: Cursor blink non chiama displayTime()
    Given COUNTDOWN_EDIT mode con cursor blink attivo
    When 500ms timer scatta (cursor toggle)
    Then setColonBlink() chiamato
    And displayTime() NON chiamato

  Scenario: Mode change aggiorna display
    Given CLOCK mode active
    When KEY_A premuto (MODE toggle)
    Then displayTime() chiamato 1 volta per update
    And Successive refreshes solo refresh() calls
```

---

## 🛠️ Implementation Plan

### Phase 1: Profiling & Measurement (4 ore)

#### Step 1.1: Baseline Loop Timing
```cpp
// In loop(), add timing measurement:
void loop() {
    unsigned long loopStart = micros();
    
    // === EXISTING LOOP CODE ===
    display->refresh();
    handleSerialCommands();
    // ... etc
    
    unsigned long loopEnd = micros();
    unsigned long loopTime = loopEnd - loopStart;
    
    // Moving average (100 samples)
    static unsigned long loopSum = 0;
    static uint8_t loopCount = 0;
    loopSum += loopTime;
    if (++loopCount >= 100) {
        #ifdef DEBUG
        Serial.print(F("Loop avg: "));
        Serial.print(loopSum / 100);
        Serial.println(F(" us"));
        #endif
        loopSum = 0;
        loopCount = 0;
    }
}
```

**Deliverable**: CSV data con loop time average/max/min per 1000 iterations

#### Step 1.2: Keypad Scan Profiling
```cpp
// In KeypadController::scan():
KeypadKey KeypadController::scan() {
    #ifdef DEBUG_KEYPAD_TIMING
    unsigned long scanStart = micros();
    #endif
    
    // ... existing scan logic ...
    
    #ifdef DEBUG_KEYPAD_TIMING
    unsigned long scanDuration = micros() - scanStart;
    Serial.print(F("Scan: "));
    Serial.print(scanDuration);
    Serial.println(F(" us"));
    #endif
    
    return key;
}
```

**Deliverable**: Scan time min/max/avg, comparison no-press vs key-press

#### Step 1.3: displayTime() Call Frequency Audit
```cpp
// In DisplayController::displayTime():
void DisplayController::displayTime(uint8_t hours, uint8_t minutes) {
    #ifdef DEBUG_DISPLAY_CALLS
    static unsigned long lastCallTime = 0;
    static uint16_t callCount = 0;
    unsigned long now = millis();
    
    Serial.print(F("displayTime() call #"));
    Serial.print(++callCount);
    Serial.print(F(", delta: "));
    Serial.println(now - lastCallTime);
    
    lastCallTime = now;
    #endif
    
    // ... existing logic ...
}
```

**Deliverable**: Call frequency report (expected: 1/sec max per time display)

---

### Phase 2: Root Cause Identification (2 ore)

Analizzare dati profiling da Phase 1:

#### Decision Tree
```
Loop time > 5ms?
├─ YES: Identify slowest component
│   ├─ Keypad scan > 1ms? → Optimize scan (goto Phase 3.1)
│   ├─ Display refresh > 2ms? → Investigate refresh logic (unlikely)
│   └─ Other operations > 2ms? → Refactor culprit code
│
└─ NO: Loop time OK, investigate display timing
    ├─ displayTime() called > 1/sec? → Fix call frequency (goto Phase 3.2)
    ├─ refresh() chiamato inconsistently? → Fix call priority
    └─ Hardware issue (unlikely)? → Test su Arduino reale
```

**Deliverable**: Root cause report con evidence data e proposed fix

---

### Phase 3: Fix Implementation (4-6 ore)

#### Fix Option 3.1: Time-Sliced Keypad Scan

**When**: Se scan time > 1ms è identificato come bottleneck

```cpp
// Refactor scan() per scanRow() (scan 1 riga alla volta)
class KeypadController {
private:
    uint8_t currentRow = 0;
    
public:
    KeypadKey scan() {
        // Scan SOLO current row invece di tutte e 4
        KeypadKey key = scanRow(currentRow);
        
        // Avanza al next row per prossimo loop
        currentRow = (currentRow + 1) % 4;
        
        return key;  // KEY_NONE se nessun key su questa riga
    }
    
private:
    KeypadKey scanRow(uint8_t row) {
        // Scan solo 1 riga: ~200µs invece di 800µs
        // ... logic ...
    }
};
```

**Tradeoff**: 
- ✅ Scan time ridotto 4× (800µs → 200µs)
- ⚠️ Key detection latency aumentata (max 4 loop cycles invece di 1)
- ✅ Display refresh prioritized

**Alternative: Scan Throttling**
```cpp
// Scan keypad ogni N loop invece di ogni loop
static uint8_t scanThrottle = 0;
if (++scanThrottle >= 5) {  // Scan ogni 5 loop (25ms @ 200Hz)
    keypad->scan();
    scanThrottle = 0;
}
```

**Tradeoff**:
- ✅ No refactor KeypadController code needed
- ✅ Display timing unaffected
- ⚠️ Key response slower (max 25ms latency invece di 5ms)

#### Fix Option 3.2: Eliminate Redundant displayTime() Calls

**When**: Se displayTime() chiamato > 1/sec

```cpp
// Pattern CORRECTED in main.cpp:

// WRONG (se trovato):
if (millis() - lastUpdate >= 1000) {
    displayTime(currentHours, currentMinutes);  // ❌ Sempre chiamato
    lastUpdate = millis();
}

// CORRECT:
if (millis() - lastUpdate >= 1000) {
    currentSeconds++;
    if (currentSeconds >= 60) {
        currentSeconds = 0;
        currentMinutes++;
        if (currentMinutes >= 60) {
            currentMinutes = 0;
            currentHours++;
        }
        displayTime(currentHours, currentMinutes);  // ✅ Solo se cambiato
    }
    lastUpdate = millis();
}

// Cursor blink:
if (millis() - lastCursorBlink >= 500) {
    display->setColonBlink(!display->getColonVisible());  // ✅ Solo state
    lastCursorBlink = millis();
    // NO displayTime() call here!
}
```

#### Fix Option 3.3: Ensure refresh() Priority

```cpp
void loop() {
    // === STEP 1: DISPLAY REFRESH (MASSIMA PRIORITÀ) ===
    display->refresh();  // ← Must be FIRST, chiamato OGNI loop
    
    // === STEP 2: INPUT HANDLING (può essere più lento) ===
    if (Serial.available()) {
        handleSerialCommands();  // OK se prende tempo
    }
    
    // Keypad scan con time-slicing o throttling
    keypad->scan();  // Optimized version
    
    // === STEP 3: BUSINESS LOGIC (non-time-critical) ===
    updateCountdown();
    updateBattery();
    // ... etc
}
```

---

### Phase 4: Validation & Testing (2-4 ore)

#### Test 4.1: Visual Inspection (Wokwi)
- [ ] Start simulator con fix applicato
- [ ] Osservare display per 5 minuti continuous
- [ ] Verificare: cifre nitide, no flickering, colon blink smooth
- [ ] Screenshot display stable per documentation

#### Test 4.2: Stress Test
```cpp
// Test bench: rapid key presses + time updates
void stressTest() {
    for (uint16_t i = 0; i < 1000; i++) {
        display->refresh();
        keypad->scan();
        if (i % 100 == 0) {
            displayTime(i / 100, i % 60);  // Frequent updates
        }
        delay(5);  // Simulate 200Hz loop
    }
    // Check: no flickering, no crash, RAM stable
}
```

#### Test 4.3: Timing Re-measurement
Re-run Phase 1 profiling con fix applicato:
- [ ] Loop time avg < 5ms ✅
- [ ] Keypad scan time < 1ms (or <200µs se time-sliced) ✅
- [ ] displayTime() call frequency ≤ 1/sec ✅
- [ ] No memory leak dopo 1 ora runtime ✅

#### Test 4.4: Hardware Real Arduino (se disponibile)
- Flash firmware su Arduino Uno reale
- Visual inspection su display fisico
- Conferma fix funziona anche su hardware (non solo Wokwi)

---

## 📚 Teoria Hardware/Software

### 🔌 Hardware: Display Multiplexing Principles

#### Common Cathode 7-Segment Display

```
Anatomia display singolo:
       A
     ─────
    │     │
  F │  G  │ B
     ─────
    │     │
  E │     │ C
     ─────
       D      • DP

Segment Encoding (1 byte):
Bit: 7  6  5  4  3  2  1  0
     DP G  F  E  D  C  B  A
     
Example: digit "5" = 0b01101101 = 0x6D
         A=ON, B=OFF, C=ON, D=ON, E=OFF, F=ON, G=ON, DP=OFF
```

#### Multiplexing Tecnica

**Problema**: 4 digit × 8 segmenti = 32 pins necessari (troppi per ATmega328P)

**Soluzione**: Multiplexing temporale
- Condividere segment lines tra tutti i 4 digit (8 pins)
- Digit select lines individuali (4 pins via transistor)
- **Attivare 1 solo digit alla volta**, ciclare rapidamente

```
Timing diagram (target 50Hz full cycle):

Digit 1 ON  ▂▂▂▂▂▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔ OFF
             ▲─────▶ 5ms
Digit 2 ON  ▔▔▔▔▔▔▔▔▔▂▂▂▂▂▔▔▔▔▔ OFF
                     ▲─────▶ 5ms
Digit 3 ON  ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▂▂▂▂▂ OFF
                           ▲─────▶ 5ms
Digit 4 ON  ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▂▂▂▂▂
                                 ▲─────▶ 5ms
            ◀──────────────────────────▶
                  20ms = 50Hz

Persistenza visione umana: >50Hz appare "solido" (no flicker perceivable)
```

**Critical constraint**: Se digit non refreshed entro ~20ms, flicker visibile!

#### 74HC595 Shift Register

```
74HC595 DIP-16 pinout:
   ┌────────────┐
QB │1  ╱╲    16│ VCC (+5V)
QC │2       15│ QA (segment A output)
QD │3       14│ SER (serial data input) ◄─ Arduino DATA pin
QE │4  595  13│ OE (output enable, LOW=active)
QF │5       12│ RCLK (latch, rising edge) ◄─ Arduino LATCH pin
QG │6       11│ SRCLK (shift clock) ◄─ Arduino CLOCK pin
QH │7       10│ SRCLR (clear, HIGH=normal)
GND│8        9│ QH' (serial out for daisy-chain)
   └────────────┘

Protocol (bit-banging):
1. Set DATA pin (HIGH or LOW)
2. Pulse CLOCK pin (LOW→HIGH) — shifts bit into register
3. Repeat 8 times per byte
4. Pulse LATCH pin (LOW→HIGH) — output register updates, segments change

Timing requirements (HC595):
- Setup time: 25ns (data stable before clock rise)
- Pulse width: 40ns (clock HIGH duration)
- Propagation delay: 31ns (output change after latch)

Arduino @ 16MHz = 62.5ns per cycle → ampio margine timing!
```

---

### 💻 Software: Non-Blocking Timing Patterns

#### millis() vs delay()

```cpp
// ❌ BLOCKING (don't use in loop!):
void badPattern() {
    display->show("12:34");
    delay(1000);  // CPU blocked, no multiplexing!
    display->show("12:35");
}

// ✅ NON-BLOCKING (correct):
void goodPattern() {
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate >= 1000) {
        // Update SOLO quando necessario
        lastUpdate = millis();
    }
    
    // refresh() chiamato OGNI loop
    display->refresh();
}
```

**Why millis()?**
- Returns milliseconds since boot (unsigned long, overflow @ 49 giorni)
- Incrementato automaticamente da Timer0 interrupt (ogni 1.024ms)
- No blocking: check è istantaneo (1-2 CPU cycles)

#### Interrupt-Driven Timing (Alternative)

```cpp
// Timer0 già usato da millis(), Timer1 disponibile per custom timing

// Setup Timer1 per 200Hz interrupt (5ms period):
void setupTimer1() {
    cli();  // Disable interrupts durante setup
    
    TCCR1A = 0;  // Normal mode
    TCCR1B = 0;
    TCNT1 = 0;   // Counter reset
    
    // Compare match: 16MHz / (prescaler × (1 + OCR1A)) = 200Hz
    // Prescaler 64: 16MHz / 64 = 250kHz
    // OCR1A = 250000 / 200 - 1 = 1249
    OCR1A = 1249;
    
    TCCR1B |= (1 << WGM12);  // CTC mode
    TCCR1B |= (1 << CS11) | (1 << CS10);  // Prescaler 64
    TIMSK1 |= (1 << OCIE1A);  // Enable interrupt
    
    sei();  // Re-enable interrupts
}

ISR(TIMER1_COMPA_vect) {
    // Chiamato automaticamente ogni 5ms
    display->refresh();  // Multiplexing timing perfetto!
}
```

**Tradeoff**:
- ✅ Timing preciso (jitter <1µs)
- ✅ No need to call refresh() in loop
- ⚠️ ISR overhead (~10-20µs per call)
- ⚠️ More complex code

**For this project**: millis() pattern sufficient, Timer1 interrupt overkill.

---

### 🧮 Software: Matrix Keypad Scanning

#### Matrix Principle

4×4 keypad = 16 keys con solo 8 pins (4 row + 4 col):

```
        COL1   COL2   COL3   COL4
         │      │      │      │
ROW1 ────┼──●───┼──●───┼──●───┼──●──  (keys: 1,2,3,A)
         │      │      │      │
ROW2 ────┼──●───┼──●───┼──●───┼──●──  (keys: 4,5,6,B)
         │      │      │      │
ROW3 ────┼──●───┼──●───┼──●───┼──●──  (keys: 7,8,9,C)
         │      │      │      │
ROW4 ────┼──●───┼──●───┼──●───┼──●──  (keys: *,0,#,D)

Rows: OUTPUT pins (set HIGH o LOW per scan)
Cols: INPUT_PULLUP pins (read HIGH quando no key, LOW quando key pressed)

Scanning algorithm:
1. Set all rows HIGH
2. For each row:
   a. Set row i LOW (activate scan)
   b. Read all columns (se COL j = LOW → key [i][j] pressed)
   c. Set row i HIGH (deactivate)
3. Return detected key (or KEY_NONE)
```

#### Scan Time Analysis

```cpp
// Worst case timing:
for (row = 0; row < 4; row++) {           // 4 iterations
    digitalWrite(rowPins[row], LOW);      // ~5µs per call
    delayMicroseconds(10);                // 10µs stabilization
    
    for (col = 0; col < 4; col++) {       // 4 iterations
        digitalRead(colPins[col]);        // ~3µs per call
        // Debounce check: ~2µs
    }
    
    digitalWrite(rowPins[row], HIGH);     // ~5µs
}

Total per scan: 4 × (5 + 10 + 4×(3+2) + 5) = 4 × 40 = 160µs (best case, no key)
With key detected + debounce: +50µs → ~200-250µs
```

**Conclusione**: Keypad scan ~250µs non è bottleneck (target <1ms)!  
Hypothesis 1 potrebbe essere **falsa** → investigate other causes.

#### Debouncing Software

Mechanical keys "bounce" per 5-20ms after press:

```
Voltage on COL pin:
5V ┐           ┌─────────────────────
   │  ╱╲  ╱╲  │
   │ ╱  ╲╱  ╲ │  (bounce: multiple edges)
0V └─────────────────
   ▲         ▲
   Press    Stable (after ~20ms)

Debounce algorithm:
if (key detected) {
    if (currentTime - lastDebounceTime > DEBOUNCE_DELAY) {
        // Key confirmed stable
        lastDebounceTime = currentTime;
        return key;
    }
}
return KEY_NONE;  // Still bouncing, ignore
```

---

### 🔬 Software: Profiling Techniques

#### micros() High-Resolution Timing

```cpp
// Timer0 overflow interrupt ogni 1.024ms → millis() resolution 1ms
// Timer0 actual count ogni 4µs (16MHz / prescaler 64)
// micros() interpolates → resolution 4µs

unsigned long start = micros();  // Timestamp start
doExpensiveOperation();
unsigned long duration = micros() - start;  // Elapsed microseconds

// Overhead: micros() call ~4-6µs, trascurabile per misurazioni >50µs
```

#### Moving Average Filter

```cpp
// Smoothing noisy timing measurements:
class MovingAverage {
private:
    uint32_t buffer[N];
    uint8_t index = 0;
    uint32_t sum = 0;
    
public:
    void add(uint32_t value) {
        sum -= buffer[index];        // Subtract oldest
        buffer[index] = value;       // Store new
        sum += value;                // Add new
        index = (index + 1) % N;     // Circular buffer
    }
    
    uint32_t get() { return sum / N; }
};

// Usage:
MovingAverage<100> loopTiming;
loopTiming.add(micros() - loopStart);
Serial.println(loopTiming.get());  // Smooth average
```

---

## 📝 Notes & Considerations

### Memory Impact
- Profiling code adds ~100 bytes Flash (conditional compilation)
- Timing variables add ~20 bytes RAM (uint32_t accumulators)
- **Net impact**: +0.5% RAM, +0.3% Flash (negligible)

### Alternative Approaches (Rejected)
1. **Async keypad library** (e.g., Keypad.h with interrupt): Overkill complexity
2. **Hardware anti-bounce RC filter**: Già presente pull-up resistors, SW debounce sufficient
3. **Use Timer2 for display refresh**: Timer2 needed for buzzer PWM (tone generation)

### Rollback Plan
Se fix non risolve flickering entro deadline:
1. Revert to last known good commit (pre-EPIC_03)
2. Disable keypad + countdown features temporaneamente
3. Mark task as BLOCKED, escalate for architecture review

---

## 🔗 References

### Documentation
- `EPIC_02.md`: DisplayController specification (multiplexing design)
- `EPIC_03.md`: KeypadController + CountdownController spec
- `Utilities/DisplayController.cpp`: refresh() implementation (linee 138-178)
- `Utilities/KeypadController.cpp`: scan() implementation

### External Resources
- ATmega328P Datasheet: Timer/Counter chapters (timing internals)
- [74HC595 Datasheet](https://www.ti.com/lit/ds/symlink/sn74hc595.pdf): Shift register timing diagrams
- [Multiplexing Tutorial](https://www.electronics-tutorials.ws/blog/7-segment-display-tutorial.html): LED multiplexing theory

### Tools
- Wokwi Simulator: [https://wokwi.com](https://wokwi.com) (circuit `diagram.json`)
- Arduino micros() reference: [https://www.arduino.cc/reference/en/language/functions/time/micros/](https://www.arduino.cc/reference/en/language/functions/time/micros/)

---

## 📋 Checklist Completamento

### Analysis Phase
- [ ] Loop timing profiled (avg/max/min documented)
- [ ] Keypad scan timing profiled
- [ ] displayTime() call frequency audited
- [ ] Root cause identified con evidence data

### Implementation Phase
- [ ] Fix applicato (specify which option: 3.1, 3.2, or 3.3)
- [ ] Code compiled senza errori
- [ ] Code review eseguito (self-review OK)
- [ ] Memory usage verificato (<+5% RAM)

### Testing Phase
- [ ] Visual inspection Wokwi (5 min stable)
- [ ] Stress test passed (1000 iterations no glitch)
- [ ] Timing re-measurement confirms fix
- [ ] Hardware test Arduino reale (se disponibile)

### Documentation Phase
- [ ] Root cause documented in task file
- [ ] Fix rationale explained
- [ ] Before/after timing data included
- [ ] Lessons learned section updated

---

**Task Owner**: Antonio Cittadino  
**Reviewer**: Self (code review), Team (post-fix validation)  
**Last Updated**: 19 Dicembre 2025  
**Next Review**: 22 Dicembre 2025 (post-fix validation)
