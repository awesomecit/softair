# Task #018: Audio Feedback System

**ID**: 018  
**Titolo**: Sistema Audio Feedback per Game Events  
**Tipo**: ✨ FEATURE (TacBomb)  
**Priorità**: 🔵 LOW (OPTIONAL)  
**Stato**: 📋 BACKLOG  
**Milestone**: M3 — TacBomb Parity  

**Stima**: 3-4 ore  
**Dipendenze**: Task 017  
**Files**: `Utilities/BuzzerController.cpp` (extend)  

---

## 📋 Descrizione

Estendere BuzzerController con pattern audio per game events: armed beep (3×short), countdown tick (ogni 10s), defused melody (victory), exploded (alarm). Use tone() sequences.

---

## ✅ Acceptance Criteria

```
Event: ARMED → 3 short beeps (500Hz, 100ms each)
Event: COUNTDOWN tick → 1 beep every 10s (increasing frequency when <10s)
Event: DEFUSED → victory melody (C-E-G-C notes)
Event: EXPLODED → alarm (alternating 800Hz/400Hz)
```

---

## 🛠️ Implementation

```cpp
void BuzzerController::playArmedBeep() {
    for (int i = 0; i < 3; i++) {
        tone(buzzerPin, 500, 100);
        delay(150);
    }
}
```

---

## 📚 Teoria (da completare)

- Hardware: Piezo buzzer resonant frequency
- Software: Non-blocking tone sequences

---

**Task Owner**: Antonio Cittadino  
**Last Updated**: 19 Dicembre 2025
