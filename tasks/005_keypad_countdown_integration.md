# Task #005: Keypad + Countdown Integration

**ID**: 005  
**Titolo**: Integrazione Completa Keypad ↔ Countdown Edit Workflow  
**Tipo**: ⚙️ INTEGRATION  
**Priorità**: 🔴 CRITICAL  
**Stato**: 📋 BACKLOG  
**Milestone**: M1 — MVP Stabile  

**Stima**: 6-8 ore  
**Dipendenze**: Task 001, 002, 003, 004  
**Files**: `src/main.cpp`, KeypadController, CountdownController  

---

## 📋 Descrizione

Connettere KeypadController + CountdownController in main.cpp: keypad input → countdown logic → display output. Workflow completo: MODE toggle → cursor navigation → digit edit → store value.

---

## ✅ Acceptance Criteria (BDD)

```gherkin
Feature: End-to-End Edit Workflow
  Scenario: Edit countdown da keypad
    Given CLOCK mode
    When Press KEY_A
    Then Enter COUNTDOWN_EDIT mode
    When Press UP/DOWN/LEFT/RIGHT keys
    Then Countdown updates in real-time
    When Press KEY_A again
    Then Exit to CLOCK, value stored
```

---

## 🛠️ Implementation Plan

1. Wire keypad events → countdown methods in main loop
2. Mode state machine (CLOCK ↔ COUNTDOWN_EDIT)
3. Timeout logic (30s inactivity → auto-revert CLOCK)
4. Integration test: edit sequence 00:00 → 23:59

---

## 📚 Teoria (da completare)

- Software: Event-driven architecture, state machines
- Integration patterns: Observer/callback vs polling

---

**Task Owner**: Antonio Cittadino  
**Last Updated**: 19 Dicembre 2025
