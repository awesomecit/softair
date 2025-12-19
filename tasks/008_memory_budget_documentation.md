# Task #008: Memory Budget Documentation

**ID**: 008  
**Titolo**: Documentare Memory Budget per Modulo  
**Tipo**: 📄 DOCS  
**Priorità**: 🟢 MEDIUM  
**Stato**: 📋 BACKLOG  
**Milestone**: M1 — MVP Stabile  

**Stima**: 2-3 ore  
**Dipendenze**: Task 006  
**Files**: `docs/MEMORY_BUDGET.md` (new)  

---

## 📋 Descrizione

Creare documento MEMORY_BUDGET.md con breakdown RAM/Flash per ogni modulo. Target: <80% RAM (1640/2048), <90% Flash (29030/32256). Include: current usage, projections per M2/M3, optimization opportunities.

---

## ✅ Acceptance Criteria

- [ ] Tabella RAM: DisplayController (50B), KeypadController (20B), etc.
- [ ] Tabella Flash: per module con code size
- [ ] Grafico utilizzo attuale vs target
- [ ] Sezione "Optimization Opportunities" (String → F(), etc.)

---

## 🛠️ Deliverable

```markdown
# Memory Budget

## Current Usage (M1 MVP)
| Module | RAM (bytes) | Flash (bytes) |
|--------|-------------|---------------|
| Display | 50 | 1200 |
| Keypad | 20 | 600 |
...

## Projections
- M2 Enhancement: +150 RAM, +3KB Flash
- M3 TacBomb: +200 RAM, +5KB Flash
```

---

**Task Owner**: Antonio Cittadino  
**Last Updated**: 19 Dicembre 2025
