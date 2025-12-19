# Task #007: Countdown Running Mode

**ID**: 007  
**Titolo**: Implementare Countdown Auto-Decrement Timer  
**Tipo**: ✨ FEATURE  
**Priorità**: 🟡 HIGH  
**Stato**: 📋 BACKLOG  
**Milestone**: M1 — MVP Stabile  

**Stima**: 4-6 ore  
**Dipendenze**: Task 005  
**Files**: `Utilities/CountdownController.cpp`, main.cpp  

---

## 📋 Descrizione

Aggiungere modalità RUNNING al countdown: auto-decrement ogni secondo da time impostato → 00:00. State machine: STOPPED → RUNNING (KEY_B start) → PAUSED (KEY_B pause) → STOPPED (reach 00:00).

---

## ✅ Acceptance Criteria (BDD)

```gherkin
Feature: Countdown Timer Decrement
  Scenario: Start countdown da 01:00
    Given Countdown set to 01:00
    When Press KEY_B (start)
    Then Countdown decrements ogni 1 sec
    And Display shows 00:59, 00:58, ...
    When Reach 00:00
    Then Buzzer beep, mode → STOPPED
```

---

## 🛠️ Implementation Plan

1. State machine: enum { STOPPED, RUNNING, PAUSED }
2. Timer logic: millis() every 1 sec → decrement
3. Zero detection → buzzer alert
4. KEY_B toggle RUNNING ↔ PAUSED

---

## 📚 Teoria (da completare)

- Software: State machine patterns, timer precision
- Hardware: Buzzer patterns for countdown end

---

**Task Owner**: Antonio Cittadino  
**Last Updated**: 19 Dicembre 2025
