# Task #019: Ghost Wire Easter Egg

**ID**: 019  
**Titolo**: Implementare Ghost Wire Hidden Defusal Sequence  
**Tipo**: 🎁 FEATURE (Easter Egg)  
**Priorità**: 🔵 LOW (OPTIONAL)  
**Stato**: 📋 BACKLOG  
**Milestone**: M3 — TacBomb Parity  

**Stima**: 4-6 ore  
**Dipendenze**: Task 016, 017  
**Files**: GameStateMachine.cpp  

---

## 📋 Descrizione

Easter egg: sequenza nascosta "ghost wire" (tagliare wire1→wire3→wire2 in ordine rapido <2s) → instant defusal bypass. Discovery rate target ~5%. Secret victory sound diverso.

---

## ✅ Acceptance Criteria

```gherkin
Feature: Hidden Defusal
  Scenario: Ghost wire sequence
    Given Game state = COUNTDOWN
    When Cut wire1, then wire3, then wire2 (within 2 sec)
    Then Instant DEFUSED, special sound
    And Display flash "GHOST!"
```

---

## 🛠️ Implementation

- Sequence detector: track last 3 wire cuts con timestamps
- Pattern match: [1,3,2] with all intervals <2s
- Secret sound: different melody da normal defusal

---

## 📚 Teoria (da completare)

- Software: Sequence detection algorithms
- Game design: Easter egg discovery psychology

---

**Task Owner**: Antonio Cittadino  
**Last Updated**: 19 Dicembre 2025
