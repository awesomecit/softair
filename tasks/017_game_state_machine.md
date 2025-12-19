# Task #017: Game State Machine

**ID**: 017  
**Titolo**: Implementare TacBomb Game State Machine  
**Tipo**: ✨ FEATURE (TacBomb)  
**Priorità**: 🔵 LOW (OPTIONAL)  
**Stato**: 📋 BACKLOG  
**Milestone**: M3 — TacBomb Parity  

**Stima**: 8-10 ore  
**Dipendenze**: Task 007 (Countdown Running)  
**Files**: `Utilities/GameStateMachine.cpp` (new)  

---

## 📋 Descrizione

State machine completa TacBomb: STANDBY → ARMED (keycode entry) → COUNTDOWN (running) → DEFUSED (correct wire) | EXPLODED (timeout/wrong wire). Entry/exit actions per state, RGB LED integration.

---

## ✅ Acceptance Criteria

```gherkin
Feature: Game Flow
  Scenario: Complete game cycle
    Given State = STANDBY
    When Enter keycode "1234"
    Then State → ARMED
    When Start countdown
    Then State → COUNTDOWN (decrement active)
    When Cut correct wire
    Then State → DEFUSED, buzzer victory sound
```

---

## 🛠️ State Diagram

```
STANDBY ──keycode──> ARMED ──start──> COUNTDOWN
                                  │
                        correct wire│  timeout/wrong wire
                                  ▼        ▼
                              DEFUSED  EXPLODED
```

---

## 📚 Teoria (da completare)

- Software: State machine patterns, event handling
- Game design: Balancing difficulty, feedback

---

**Task Owner**: Antonio Cittadino  
**Last Updated**: 19 Dicembre 2025
