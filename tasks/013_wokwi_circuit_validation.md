# Task #013: Wokwi Circuit Validation

**ID**: 013  
**Titolo**: Validare Wokwi diagram.json vs Code Pinout  
**Tipo**: 🧪 TEST  
**Priorità**: 🟢 MEDIUM  
**Stato**: 📋 BACKLOG  
**Milestone**: M2 — Enhancement  

**Stima**: 2-3 ore  
**Dipendenze**: Task 006  
**Files**: `diagram.json`, schematic docs  

---

## 📋 Descrizione

Cross-check diagram.json wiring vs code pin definitions. Verificare: display pins 6/7/8, keypad rows/cols, battery LEDs, buzzer. Update schematic documentation se discrepanze.

---

## ✅ Acceptance Criteria

- [ ] All pin connections in diagram.json match code constants
- [ ] Schematic diagram updated (visual reference)
- [ ] Wokwi simulation test: all components functional
- [ ] Document any wiring changes needed per hardware build

---

## 🛠️ Validation Checklist

```
Display:
☐ DATA pin → Arduino 6 (diagram.json line XX)
☐ CLOCK pin → Arduino 7
☐ LATCH pin → Arduino 8
Keypad:
☐ ROW1 → Pin 13, ROW2 → 12, ROW3 → 11, ROW4 → 10
...
```

---

**Task Owner**: Antonio Cittadino  
**Last Updated**: 19 Dicembre 2025
