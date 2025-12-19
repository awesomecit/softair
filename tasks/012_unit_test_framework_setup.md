# Task #012: Unit Test Framework Setup

**ID**: 012  
**Titolo**: Setup Unity Test Framework + CI/CD  
**Tipo**: 🔧 INFRASTRUCTURE  
**Priorità**: 🟢 MEDIUM  
**Stato**: 📋 BACKLOG  
**Milestone**: M2 — Enhancement  

**Stima**: 4-6 ore  
**Dipendenze**: Nessuna (può essere fatto in parallelo)  
**Files**: `test/test_embedded/`, `.github/workflows/ci.yml`  

---

## 📋 Descrizione

Setup Unity test framework in PlatformIO. Creare test directory structure, scrivere sample tests per CountdownController, configurare GitHub Actions CI/CD per auto-run tests on push.

---

## ✅ Acceptance Criteria

- [ ] `test/test_embedded/` directory con sample tests
- [ ] `pio test` runs successfully (esempio: 5+ tests pass)
- [ ] GitHub Actions workflow: build + test on push/PR
- [ ] README updated con "Running Tests" section

---

## 🛠️ Deliverables

```
test/
├── test_embedded/
│   ├── test_countdown_controller.cpp
│   ├── test_display_controller.cpp
│   └── test_battery_monitor.cpp
.github/
└── workflows/
    └── ci.yml  (PlatformIO CI config)
```

---

**Task Owner**: Antonio Cittadino  
**Last Updated**: 19 Dicembre 2025
