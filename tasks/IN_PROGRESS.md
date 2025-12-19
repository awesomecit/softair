# 🔧 IN PROGRESS — Task Attivi

**Ultimo aggiornamento**: 19 Dicembre 2025

Questo documento elenca i task attualmente in lavorazione o testing nel progetto Softair Arduino.

---

## 📊 Riepilogo

| ID | Task | Priorità | Stato | Assegnato | ETA |
|----|------|----------|-------|-----------|-----|
| 001 | Display Flickering Fix | 🔴 CRITICAL | INVESTIGATING | Team | 2-3 giorni |
| 002 | Keypad Controller Testing | 🟡 HIGH | READY TO TEST | Team | 1-2 giorni |
| 003 | Countdown Controller Testing | 🟡 HIGH | READY TO TEST | Team | 1-2 giorni |

**Blocco attuale**: Task 001 (Display Flickering) blocca task 002 e 003. Risoluzione prioritaria necessaria.

---

## 🐛 Task #001: Display Flickering Fix

**Tipo**: BUG (Critical)  
**Priorità**: 🔴 CRITICAL  
**Stima**: 8-12 ore  
**Dipendenze**: Nessuna (blocker per altri task)  
**File coinvolti**: 
- `src/main.cpp` (loop timing)
- `Utilities/KeypadController.cpp` (scan timing)
- `Utilities/DisplayController.cpp` (refresh logic)

### 🎯 Descrizione

Display 7-segmenti mostra flickering (sfarfallio) dopo integrazione EPIC_03 (Keypad + Countdown). Prima dell'integrazione, DisplayController funzionava perfettamente con multiplexing stabile a 50Hz.

### 🔍 Sintomi Riportati

```
User report: "ora i led sembra che sfarfallino e sono quasi sempre accesi"
```

- Segmenti appaiono sempre accesi invece di multiplexing pulito
- Flickering visibile in simulazione Wokwi
- Problema iniziato dopo aggiunta di:
  - `KeypadController` scan nel loop
  - `CountdownController` cursor blink logic
  - MODE selection screen con timeout

### 🔬 Analisi Root Cause (Ipotesi)

#### Ipotesi 1: Keypad Scan Blocking (Più Probabile)
```cpp
// KeypadController::scan() scansiona 4 righe sequenzialmente
// Stima: ~1-2ms per scan completo con debounce check
// Se chiamato troppo frequentemente, può interferire con display refresh (5ms target)
```

**Evidence**:
- Display funzionava prima di keypad
- Scan a matrice richiede cicli di GPIO reads (non istantaneo)
- Debounce logic aggiunge delay nel controllo

#### Ipotesi 2: displayTime() Called Too Often
```cpp
// In loop originale, displayTime() chiamato SOLO quando tempo cambia (1 sec)
// Con cursor blink (500ms toggle), displayTime() potenzialmente chiamato 2x/sec
// refresh() deve essere chiamato OGNI loop, displayTime() solo quando cambia contenuto
```

**Evidence**:
- Cursor blink implementato con display update ogni 500ms
- Mode selection timeout chiamava displayTime() direttamente

#### Ipotesi 3: Loop Timing Disruption
```cpp
// Display refresh richiede chiamata consistente ogni loop (~5ms target)
// Aggiunta di keypad scan, countdown logic, mode timeout aumenta loop time
// Se loop time > 5ms, multiplexing rate scende sotto 50Hz (flickering visibile)
```

### ✅ Acceptance Criteria (BDD)

```gherkin
Given: Display 7-segmenti connesso e DisplayController inizializzato
When: Keypad scan attivo nel loop
Then: Display mostra cifre stabili senza flickering visibile

Given: Loop time misurato con micros()
When: Tutti i subsystem attivi (display + keypad + battery)
Then: Loop time medio < 5ms (target 200Hz loop rate)

Given: DisplayController refresh() chiamato ogni loop
When: displayTime() chiamato solo quando contenuto cambia
Then: Multiplexing stabile a 50Hz (5ms per digit)

Given: Keypad scan eseguito
When: Timing profiled con serial debug
Then: Scan duration < 1ms (non blocca display refresh)

Given: Cursor blink attivo in COUNTDOWN_EDIT mode
When: 500ms timer scatta
Then: Solo colon state toggled, NO displayTime() call
```

### 🛠️ Fix Strategy (Step-by-Step)

#### Step 1: Baseline Profiling
```cpp
// In loop(), aggiungere timing measurements:
unsigned long loopStart = micros();

// ... existing loop code ...

unsigned long loopEnd = micros();
unsigned long loopTime = loopEnd - loopStart;

// Report ogni 100 loop
static uint8_t loopCounter = 0;
if (++loopCounter >= 100) {
    Serial.print(F("Avg loop time: "));
    Serial.print(loopTime);
    Serial.println(F(" us"));
    loopCounter = 0;
}
```

**Target**: Loop time < 5000µs (5ms) per mantenere 200Hz rate

#### Step 2: Profile Keypad Scan
```cpp
// In KeypadController::scan():
unsigned long scanStart = micros();
// ... scan logic ...
unsigned long scanDuration = micros() - scanStart;

#ifdef DEBUG
Serial.print(F("Keypad scan: "));
Serial.print(scanDuration);
Serial.println(F(" us"));
#endif
```

**Target**: Scan time < 1000µs (1ms)

#### Step 3: Optimize Keypad Scan (se necessario)
```cpp
// Opzione A: Time-slicing (scan 1 riga per loop invece di 4)
static uint8_t currentRow = 0;
KeypadKey key = keypad->scanRow(currentRow);
currentRow = (currentRow + 1) % 4;

// Opzione B: Scan throttling (scan ogni N loop)
static uint8_t scanThrottle = 0;
if (++scanThrottle >= 5) { // Scan ogni 5 loop
    keypad->scan();
    scanThrottle = 0;
}
```

#### Step 4: Ensure displayTime() Only When Needed
```cpp
// WRONG (chiamato troppo spesso):
if (colonBlinkTimer - lastColonBlink >= 500) {
    displayTime(hours, minutes); // ❌ Ricalcola tutto
}

// CORRECT (solo toggle state):
if (colonBlinkTimer - lastColonBlink >= 500) {
    display->setColonBlink(!display->getColonVisible()); // ✅ Solo bitmask
    lastColonBlink = colonBlinkTimer;
}
```

#### Step 5: Verify Display refresh() Priority
```cpp
void loop() {
    // STEP 1: Display refresh SEMPRE FIRST (non-blocking)
    display->refresh(); // ← Chiamata ogni loop, massima priorità
    
    // STEP 2: Keypad scan (potenzialmente slow)
    if (shouldScanKeypad()) {
        keypad->scan();
    }
    
    // STEP 3: Altri subsystems (battery, serial, etc.)
    // ...
}
```

### 📋 Test Plan

1. **Test Baseline**: Compilare versione solo display (no keypad)
   - Verificare: No flickering, loop time ~2ms
   
2. **Test Keypad Isolated**: Aggiungere solo keypad scan
   - Verificare: Loop time < 5ms, scan time < 1ms
   
3. **Test Integration**: Aggiungere countdown + mode logic
   - Verificare: No flickering, tutti i subsystems funzionanti
   
4. **Test Stress**: Premere tasti continuamente
   - Verificare: Display stabile anche con input frequente

### 📝 Note

- **Blocco**: Questo bug blocca testing di task 002 e 003
- **Workaround Attuale**: Keypad scan disabilitato, MODE selection disabilitata
- **Commit di Riferimento**: Cercare ultimo commit pre-EPIC_03 per stato working
- **Alternative**: Se profiling mostra scan non è problema, investigate display refresh timing

### 🔗 Riferimenti

- Conversation Summary: "Display flickering after EPIC_03 integration"
- `EPIC_03.md`: Keypad + Countdown spec
- `DisplayController.cpp`: refresh() implementation (linee 138-178)
- `KeypadController.cpp`: scan() implementation

---

## 🧪 Task #002: Keypad Controller Testing

**Tipo**: TEST  
**Priorità**: 🟡 HIGH  
**Stima**: 4-6 ore  
**Dipendenze**: Task 001 (Display Flickering Fix)  
**File coinvolti**: 
- `Utilities/KeypadController.{h,cpp}`
- `src/main.cpp` (integration code)

### 🎯 Descrizione

Validazione completa del KeypadController dopo fix display flickering. Codice compilato ma mai testato su hardware/simulazione.

### 📦 Cosa Testare

#### 1. Matrix Scanning Basics
```gherkin
Given: Keypad 4×4 connesso su pins 13-10 (rows), A5,9,8,7 (cols)
When: Nessun tasto premuto
Then: scan() ritorna KEY_NONE

Given: Tasto "5" premuto
When: scan() chiamato
Then: Ritorna KEY_5

Given: Tutti i 16 tasti testati uno alla volta
When: Ogni tasto premuto e scan() chiamato
Then: Valore corretto ritornato (vedi KEY_MAP)
```

#### 2. Debouncing
```gherkin
Given: Tasto premuto con bounce meccanico
When: Scan chiamato ripetutamente entro 50ms
Then: Solo un evento KEY_PRESS generato

Given: Tasto tenuto premuto > 50ms
When: Scan continua
Then: Nessun repeat event (no auto-repeat)

Given: Tasto rilasciato
When: Successiva pressione dopo > 50ms
Then: Nuovo evento KEY_PRESS generato
```

#### 3. D-Pad Navigation
```gherkin
Given: Mapping D-pad attivo (4=LEFT, 6=RIGHT, 2=UP, 8=DOWN)
When: KEY_4 premuto
Then: handleKeyPress() riceve KEY_LEFT

Given: Cursor in COUNTDOWN_EDIT mode
When: KEY_4 (LEFT) premuto
Then: Cursor si sposta verso sinistra (3→2→1→0→3 wrap)
```

#### 4. Mode Toggle
```gherkin
Given: Display in CLOCK mode
When: KEY_A premuto
Then: Mode passa a COUNTDOWN_EDIT

Given: Display in COUNTDOWN_EDIT mode
When: KEY_A premuto
Then: Mode passa a CLOCK
```

### ✅ Acceptance Criteria (BDD)

```gherkin
Given: Display flickering fix applicato (Task 001 completato)
When: Keypad scan integrato nel loop principale
Then: Nessun flickering display durante scan

Given: Serial monitor aperto
When: Tutti i 16 tasti premuti in sequenza
Then: Output seriale mostra key press events corretti

Given: Stress test (100 key presses rapide)
When: Test eseguito
Then: Sistema stabile, no crash, no memory leak, display OK
```

### 🛠️ Test Procedure

#### Setup
1. Applicare fix Task 001 (display stabile)
2. Abilitare keypad scan in `main.cpp`
3. Aggiungere debug output:
```cpp
void handleKeyPress(KeypadKey key) {
    Serial.print(F("Key pressed: "));
    Serial.println(keypad->keyToChar(key));
    // ... rest of logic
}
```

#### Manual Test Matrix
| Tasto | Expected Key | Expected Action | Pass/Fail |
|-------|--------------|-----------------|-----------|
| 1 | KEY_1 | Digit input | ⬜ |
| 2 | KEY_2 | UP navigation | ⬜ |
| 3 | KEY_3 | Digit input | ⬜ |
| A | KEY_A | MODE toggle | ⬜ |
| 4 | KEY_4 | LEFT navigation | ⬜ |
| 5 | KEY_5 | Digit input | ⬜ |
| 6 | KEY_6 | RIGHT navigation | ⬜ |
| B | KEY_B | (reserved) | ⬜ |
| 7 | KEY_7 | Digit input | ⬜ |
| 8 | KEY_8 | DOWN navigation | ⬜ |
| 9 | KEY_9 | Digit input | ⬜ |
| C | KEY_C | (reserved) | ⬜ |
| * | KEY_STAR | (reserved) | ⬜ |
| 0 | KEY_0 | Digit input | ⬜ |
| # | KEY_HASH | (reserved) | ⬜ |
| D | KEY_D | (reserved) | ⬜ |

#### Automated Test (Unity)
```cpp
// In test/test_embedded/test_keypad.cpp
void test_keypad_scan_returns_none_when_no_press() {
    KeypadController keypad(rowPins, colPins);
    keypad.begin();
    KeypadKey key = keypad.scan();
    TEST_ASSERT_EQUAL(KEY_NONE, key);
}

void test_keypad_debounce_blocks_repeat() {
    // Simulate key bounce (multiple edges within 50ms)
    // Assert: only one event generated
}
```

### 📝 Note

- **Wokwi**: Keypad già cablato in `diagram.json` (8 wires)
- **Hardware Real**: Matrice 4×4 membrane (ICS parte #xxxxxxx)
- **Timing**: Dopo fix display, profilare keypad scan con micros()

### 🔗 Riferimenti

- `EPIC_03.md`: Keypad specification completa
- `KeypadController.h`: API reference (scan, keyToChar, begin)
- `diagram.json`: Wiring (rows 13-10, cols A5,9,8,7)

---

## 🧪 Task #003: Countdown Controller Testing

**Tipo**: TEST  
**Priorità**: 🟡 HIGH  
**Stima**: 4-6 ore  
**Dipendenze**: Task 001 (Display Flickering Fix), Task 002 (Keypad Testing)  
**File coinvolti**: 
- `Utilities/CountdownController.{h,cpp}`
- `src/main.cpp` (integration code)

### 🎯 Descrizione

Validazione completa del CountdownController per editing time format HH:MM con cursor navigation e digit validation. Codice compilato ma mai testato.

### 📦 Cosa Testare

#### 1. Cursor Navigation
```gherkin
Given: Countdown in EDIT mode, cursor su posizione 0 (H1)
When: moveCursorRight() chiamato
Then: Cursor passa a posizione 1 (H2)

Given: Cursor su posizione 3 (M2, ultimo digit)
When: moveCursorRight() chiamato
Then: Cursor wrappa a posizione 0 (H1)

Given: Cursor su posizione 0 (H1)
When: moveCursorLeft() chiamato
Then: Cursor wrappa a posizione 3 (M2)
```

#### 2. Digit Increment con Validation
```gherkin
Given: Time 12:34, cursor su H1 (valore 1)
When: incrementDigit() chiamato
Then: H1 diventa 2, time è 22:34

Given: Time 22:34, cursor su H1 (valore 2, max per H1)
When: incrementDigit() chiamato
Then: H1 wrappa a 0, time è 02:34

Given: Time 20:00, cursor su H2 (valore 0)
When: incrementDigit() ripetuto 4 volte
Then: H2 si ferma a 3 (max 23:59), time è 23:00

Given: Time 12:55, cursor su M1 (valore 5, max per M1)
When: incrementDigit() chiamato
Then: M1 wrappa a 0, time è 12:05
```

#### 3. Digit Decrement con Validation
```gherkin
Given: Time 12:34, cursor su H1 (valore 1)
When: decrementDigit() chiamato
Then: H1 diventa 0, time è 02:34

Given: Time 02:34, cursor su H1 (valore 0)
When: decrementDigit() chiamato
Then: H1 wrappa a 2 (max), time è 22:34

Given: Time 00:00, cursor su M2 (valore 0)
When: decrementDigit() chiamato
Then: M2 wrappa a 9, time è 00:09
```

#### 4. Edge Cases Validation
```gherkin
Given: Time 23:59 (maximum valid)
When: incrementDigit() su qualsiasi posizione
Then: Wrap appropriato, time rimane valido (<= 23:59)

Given: Time 00:00 (minimum valid)
When: decrementDigit() su qualsiasi posizione
Then: Wrap appropriato, time rimane valido

Given: Time 19:59, cursor su H1 (valore 1)
When: incrementDigit() → H1 diventa 2
Then: Sistema valida: H2 deve essere <= 3, quindi H2 non cambia (29:59 invalido)
```

### ✅ Acceptance Criteria (BDD)

```gherkin
Given: CountdownController inizializzato con default 10:00
When: getTime() chiamato
Then: Ritorna hours=10, minutes=0

Given: Countdown editing attivo
When: Tutte le combinazioni di cursor pos + increment/decrement testate
Then: Nessun valore invalido generato (sempre <= 23:59)

Given: Display integration attivo
When: Cursor blink ogni 500ms
Then: Digit corrente lampeggia, altri digit stabili, display no flickering

Given: Keypad integration attivo (Task 002 completato)
When: KEY_4/KEY_6 per LEFT/RIGHT, KEY_2/KEY_8 per UP/DOWN
Then: Cursor navigation e digit edit funzionano correttamente
```

### 🛠️ Test Procedure

#### Setup
1. Completare Task 001 (display stable) e Task 002 (keypad working)
2. Abilitare COUNTDOWN_EDIT mode in main.cpp
3. Aggiungere debug output:
```cpp
void updateCountdownDisplay() {
    uint8_t h, m;
    countdown->getTime(h, m);
    uint8_t cursorPos = countdown->getCursorPosition();
    
    Serial.print(F("Countdown: "));
    Serial.print(h);
    Serial.print(F(":"));
    Serial.print(m);
    Serial.print(F(" [Cursor@"));
    Serial.print(cursorPos);
    Serial.println(F("]"));
}
```

#### Manual Test Matrix
| Test Case | Initial Time | Action | Expected Result | Pass/Fail |
|-----------|--------------|--------|-----------------|-----------|
| Cursor wrap right | 10:00 @ pos 3 | RIGHT | 10:00 @ pos 0 | ⬜ |
| Cursor wrap left | 10:00 @ pos 0 | LEFT | 10:00 @ pos 3 | ⬜ |
| Increment H1 normal | 10:00 @ pos 0 | UP | 20:00 @ pos 0 | ⬜ |
| Increment H1 max wrap | 20:00 @ pos 0 | UP | 00:00 @ pos 0 | ⬜ |
| Increment H2 validation | 20:00 @ pos 1 | UP (×4) | 23:00 @ pos 1 | ⬜ |
| Increment M1 normal | 12:30 @ pos 2 | UP | 12:40 @ pos 2 | ⬜ |
| Increment M1 max wrap | 12:50 @ pos 2 | UP | 12:00 @ pos 2 | ⬜ |
| Decrement H1 min wrap | 00:00 @ pos 0 | DOWN | 20:00 @ pos 0 | ⬜ |
| Edge case 23:59 | 23:59 @ any | UP/DOWN | Valid time always | ⬜ |

#### Automated Test (Unity)
```cpp
// In test/test_embedded/test_countdown.cpp
void test_countdown_increment_h1_wraps_at_max() {
    CountdownController cd;
    cd.setTime(2, 0); // 20:00
    cd.incrementDigit(); // H1: 2 → 0
    uint8_t h, m;
    cd.getTime(h, m);
    TEST_ASSERT_EQUAL(0, h);
}

void test_countdown_h2_max_is_3_when_h1_is_2() {
    CountdownController cd;
    cd.setTime(23, 0);
    cd.moveCursorRight(); // Cursor on H2
    cd.incrementDigit(); // H2: 3 → 3 (blocked, max reached)
    uint8_t h, m;
    cd.getTime(h, m);
    TEST_ASSERT_EQUAL(23, h);
}
```

### 📝 Note

- **Display Integration**: Richiede `DisplayController` estensione per cursor blink
- **Validation Logic**: Vedere `CountdownController.cpp` linee 80-120 per logic dettagliata
- **User Experience**: Cursor wrap naturale per navigation rapida

### 🔗 Riferimenti

- `EPIC_03.md`: Countdown editing specification
- `CountdownController.h`: API reference (getCursorPosition, incrementDigit, etc.)
- `TacBomb_Pro_Development_Complete.md`: Time format validation rules (HH:MM max 23:59)

---

## 📊 Progress Tracking

### Overall Status
```
Task 001: [████░░░░░░] 40% (investigating root cause)
Task 002: [░░░░░░░░░░]  0% (blocked by 001)
Task 003: [░░░░░░░░░░]  0% (blocked by 001 and 002)
```

### Timeline Estimate
- **Sprint Current**: Week 1 (Task 001 investigation)
- **Sprint Next**: Week 2 (Task 002 + 003 testing)
- **Target Completion**: 10-15 giorni da oggi

### Risk Assessment
- 🔴 **HIGH**: Task 001 root cause incerto - potrebbe richiedere refactor display logic
- 🟡 **MEDIUM**: Keypad timing optimization potrebbe essere necessaria
- 🟢 **LOW**: Countdown logic già complete, solo validation tests needed

---

## 🔄 Next Actions

1. **Immediate** (oggi): Profiling display loop timing (Task 001 Step 1)
2. **Short-term** (2-3 giorni): Fix display flickering, validate fix
3. **Medium-term** (5-7 giorni): Complete keypad and countdown testing
4. **Long-term** (10-15 giorni): Full system integration test, ready for BACKLOG tasks

Per feature future, consultare `BACKLOG.md`.
