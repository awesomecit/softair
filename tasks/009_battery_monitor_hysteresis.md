# Task #009: Battery Monitor Hysteresis

**ID**: 009  
**Titolo**: Aggiungere Hysteresis a Battery Monitor  
**Tipo**: ✨ FEATURE (Refactor)  
**Priorità**: 🟢 MEDIUM  
**Stato**: 📋 BACKLOG  
**Milestone**: M2 — Enhancement  

**Stima**: 3-4 ore  
**Dipendenze**: Task 006 (M1 complete)  
**Files**: `Utilities/BatteryMonitor.cpp` (new class), main.cpp  

---

## 📋 Descrizione

Refactor battery monitor da procedural → class con state machine. Aggiungere hysteresis (±0.1V deadband) per evitare LED flapping vicino soglie. Entry actions: beep on LOW/CRITICAL transition.

---

## ✅ Acceptance Criteria (BDD)

```gherkin
Feature: Hysteresis Deadband
  Scenario: Voltage oscillating @ threshold
    Given Voltage @ 2.50V (GOOD threshold)
    When Voltage varies 2.48V ↔ 2.52V
    Then LED state stable (no flapping)
    And Transition only se voltage crosses ±0.1V band
```

---

## 🛠️ Implementation

- Create `BatteryMonitor` class con state: FULL/GOOD/LOW/CRITICAL
- Hysteresis: transition thresholds ±0.1V offset
- Entry actions: buzzer beep su LOW/CRITICAL enter

---

## 📚 Teoria (da completare)

- Software: State machine con hysteresis
- Hardware: ADC noise filtering, moving average

---

**Task Owner**: Antonio Cittadino  
**Last Updated**: 19 Dicembre 2025
