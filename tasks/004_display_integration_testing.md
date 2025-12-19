# Task #004: Display Integration Testing

**ID**: 004  
**Titolo**: Test Integrazione Display - Colon Blink & Rotation Visual  
**Tipo**: 🧪 TEST  
**Priorità**: 🟡 HIGH  
**Stato**: 📋 BACKLOG  
**Milestone**: M1 — MVP Stabile  

**Stima**: 4-6 ore  
**Dipendenze**: Task 001, 002, 003  
**Files**: `Utilities/DisplayController.cpp`, test suite  

---

## 📋 Descrizione

Test visuale completo DisplayController: colon blink timing (500ms), segment rotation (D3/D4), multiplexing stability (50Hz), brightness uniformity. Validazione in Wokwi + hardware reale.

---

## ✅ Acceptance Criteria (BDD)

```gherkin
Feature: Display Visual Quality
  Scenario: Colon blink visibile @ 500ms
    Given Display showing time
    When 500ms elapsed
    Then Colon DP toggles visibly
    
  Scenario: Segment rotation D3/D4
    Given Display showing "12:34"
    When Observe segments
    Then No ghosting between digits
```

---

## 🛠️ Test Plan

1. **Visual inspection** Wokwi: 5 min observation, no flickering
2. **Timing measurement**: micros() per colon blink period
3. **Multiplexing check**: 50Hz stable con oscilloscopio (se disponibile)
4. **Brightness uniformity**: Tutti 4 digits equal brightness

---

## 📚 Teoria (da completare)

- Hardware: Common cathode multiplexing timing
- Software: Blink state machine, refresh() timing
- MCU: Timer0 vs polling patterns

---

**Task Owner**: Antonio Cittadino  
**Last Updated**: 19 Dicembre 2025
