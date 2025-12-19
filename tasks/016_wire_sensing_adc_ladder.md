# Task #016: Wire Sensing ADC Ladder

**ID**: 016  
**Titolo**: Implementare 4-Wire Sensing con Resistor Ladder  
**Tipo**: ✨ FEATURE (TacBomb)  
**Priorità**: 🔵 LOW (OPTIONAL)  
**Stato**: 📋 BACKLOG  
**Milestone**: M3 — TacBomb Parity  

**Stima**: 6-8 ore  
**Dipendenze**: Task 017  
**Files**: `Utilities/WireSensor.cpp` (new), ADC A1  

---

## 📋 Descrizione

4 wire buttons con resistor ladder su ADC A1. Valori ADC: wire1=~200, wire2=~400, wire3=~600, wire4=~800 (con tolerance ±50). Detect quale wire tagliato per defusal logic.

---

## ✅ Acceptance Criteria

```gherkin
Feature: Wire Detection
  Scenario: Wire 3 cut (correct defusal wire)
    Given Game state = ARMED
    When Wire 3 cut (ADC reads ~600)
    Then State → DEFUSED, buzzer success beep
```

---

## 🛠️ Hardware

```
VCC ─┬─ R1 (1kΩ) ─┬─ Wire1 ─┬─ R2 (1kΩ) ─┬─ Wire2 ─ ...
     │            └─ A1       │            └─ A1
                              ...
```

---

## 📚 Teoria (da completare)

- Hardware: Resistor ladder voltage divider
- Software: ADC tolerance bands, debouncing

---

**Task Owner**: Antonio Cittadino  
**Last Updated**: 19 Dicembre 2025
