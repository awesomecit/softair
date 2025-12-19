# Task #011: Serial Commands Extended

**ID**: 011  
**Titolo**: Estendere Serial Commands con m/v/k/d  
**Tipo**: ✨ FEATURE  
**Priorità**: 🟢 MEDIUM  
**Stato**: 📋 BACKLOG  
**Milestone**: M2 — Enhancement  

**Stima**: 2-3 ore  
**Dipendenze**: Task 006  
**Files**: `src/main.cpp` (handleSerialCommands)  

---

## 📋 Descrizione

Aggiungere comandi serial debug: `m` (memory detailed), `v` (version/build info), `k` (keypad test mode), `d` (display test pattern).

---

## ✅ Acceptance Criteria

```
Command 'm': Show RAM breakdown per module
Command 'v': Show version, compile date, PlatformIO env
Command 'k': Enter keypad diagnostic (print all key presses)
Command 'd': Cycle display test patterns (8.8.8.8, segments ON/OFF)
```

---

## 🛠️ Implementation

```cpp
case 'm':
    printMemoryBreakdown();  // Per-module RAM usage
    break;
case 'v':
    Serial.println(F("Version: 1.0.0-M1"));
    Serial.println(F(__DATE__ " " __TIME__));
    break;
```

---

**Task Owner**: Antonio Cittadino  
**Last Updated**: 19 Dicembre 2025
