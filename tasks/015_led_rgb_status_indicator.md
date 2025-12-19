# Task #015: LED RGB Status Indicator

**ID**: 015  
**Titolo**: Aggiungere LED RGB per Game State Indicator  
**Tipo**: ✨ FEATURE (TacBomb)  
**Priorità**: 🔵 LOW (OPTIONAL)  
**Stato**: 📋 BACKLOG  
**Milestone**: M3 — TacBomb Parity  

**Stima**: 3-4 ore  
**Dipendenze**: Task 017 (Game State Machine)  
**Files**: `Utilities/RGBLed.cpp` (new), hardware wiring  

---

## 📋 Descrizione

LED RGB common cathode su pins PWM (9,10,11) per indicare game state: STANDBY=blue, ARMED=yellow, COUNTDOWN=red blink, DEFUSED=green, EXPLODED=red solid.

---

## ✅ Acceptance Criteria

```gherkin
Feature: RGB State Indication
  Scenario: ARMED state
    Given Game state = ARMED
    Then RGB shows yellow (R=255, G=255, B=0)
```

---

## 🛠️ Hardware

- RGB LED common cathode
- Resistors: 3× 220Ω (per channel)
- Pins: 9 (R), 10 (G), 11 (B) - PWM capable

---

## 📚 Teoria (da completare)

- Hardware: RGB LED types, PWM color mixing
- Software: analogWrite() per color control

---

**Task Owner**: Antonio Cittadino  
**Last Updated**: 19 Dicembre 2025
