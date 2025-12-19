# Task #014: Code Documentation Pass

**ID**: 014  
**Titolo**: Aggiungere Doxygen Comments + README Update  
**Tipo**: 📄 DOCS  
**Priorità**: 🟢 MEDIUM  
**Stato**: 📋 BACKLOG  
**Milestone**: M2 — Enhancement  

**Stima**: 3-4 ore  
**Dipendenze**: Task 006 (M1 code stable)  
**Files**: All .h/.cpp files, README.md  

---

## 📋 Descrizione

Aggiungere Doxygen comments a tutte le classi/metodi pubblici. Update README con architecture diagram, API reference links, build instructions dettagliate. Generate HTML docs con Doxygen.

---

## ✅ Acceptance Criteria

- [ ] All public methods hanno `@brief`, `@param`, `@return` comments
- [ ] README updated: architecture diagram, modules overview
- [ ] Doxygen config file (`Doxyfile`) presente
- [ ] `doxygen` generates HTML docs successfully

---

## 🛠️ Example Doxygen Comment

```cpp
/**
 * @brief Increment digit at current cursor position
 * 
 * Applies time validation constraints (e.g., hours max 23).
 * Auto-clamps dependent digits if needed (D2 when D1=2).
 * 
 * @return void
 */
void CountdownController::incrementCurrentDigit();
```

---

**Task Owner**: Antonio Cittadino  
**Last Updated**: 19 Dicembre 2025
