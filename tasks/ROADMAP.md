# 🗺️ ROADMAP — Timeline Progetto Softair Arduino

**Versione**: 1.0.0  
**Ultimo aggiornamento**: 19 Dicembre 2025  
**Orizzonte temporale**: 10-12 settimane (Q1 2026)

---

## 📊 Executive Summary

Il progetto Softair Arduino è attualmente al **40% di completamento** con 8 feature implementate e funzionanti. L'obiettivo primario è raggiungere **MVP Stabile** entro 3-4 settimane, seguito da enhancement opzionali e integrazione completa features TacBomb Pro.

```
Progress: [████████░░░░░░░░░░] 40% Complete

┌─────────────────────────────────────────────────────────────┐
│ Phase          │ Status      │ Duration │ Completion    │
├─────────────────────────────────────────────────────────────┤
│ Foundation     │ ✅ DONE     │ 4 weeks  │ 100%          │
│ MVP Stabile    │ 🔧 IN PROGRESS │ 3-4 wks │ 30% → 100%  │
│ Enhancement    │ 📋 PLANNED  │ 1-2 wks  │ 0%            │
│ TacBomb Parity │ 📋 OPTIONAL │ 4-6 wks  │ 0%            │
└─────────────────────────────────────────────────────────────┘
```

---

## 🎯 Milestones Overview

### Milestone 0: Foundation ✅ **COMPLETED**
**Data completamento**: Novembre 2025  
**Durata effettiva**: 4 settimane

#### Obiettivi Raggiunti
- [x] Setup PlatformIO + Wokwi simulation environment
- [x] Boot sequence visuale con SystemInitializer
- [x] Battery monitor (implementazione minimal)
- [x] Display controller 4-digit 7-segmenti (multiplexing)
- [x] Buzzer controller con pattern audio
- [x] Config loader con struct-based config
- [x] Button handling interrupt-driven con debouncing
- [x] Serial debug interface con comandi interattivi

#### Deliverables
- 8 feature completate e funzionanti (vedere `DONE.md`)
- Memoria: 60.5% RAM (1240/2048), 34.2% Flash (11020/32256)
- Documentazione: README.md completo, EPIC_01/02 documentati
- Circuit: `diagram.json` con display + battery LEDs + boot LEDs

#### Lessons Learned
- ✅ **F() macro essenziale**: Risparmiati ~400 bytes RAM con Flash string storage
- ✅ **Modular structure**: Utilities/ folder permette riuso componenti
- ⚠️ **Display timing critico**: Multiplexing richiede refresh() call consistente
- ⚠️ **Integration testing needed**: Feature standalone funzionano, integration da validare

---

### Milestone 1: MVP Stabile 🔧 **IN PROGRESS**
**Target data**: Fine Gennaio 2026 (3-4 settimane da oggi)  
**Priorità**: 🔴 CRITICAL

#### Obiettivi
Sistema completo e stabile con display + keypad + countdown editing funzionante senza flickering, pronto per uso reale in scenario airsoft.

#### Features Incluse
```
┌───────────────────────────────────────────────────────────────┐
│ ID  │ Feature                      │ Status    │ ETA        │
├───────────────────────────────────────────────────────────────┤
│ 001 │ Display Flickering Fix       │ 🔧 Active │ 2-3 giorni │
│ 002 │ Keypad Controller Testing    │ 📋 Blocked│ +1-2 giorni│
│ 003 │ Countdown Controller Testing │ 📋 Blocked│ +1-2 giorni│
│ 004 │ Display Integration Test     │ 📋 Planned│ +4-6 ore   │
│ 005 │ Keypad+Countdown Integration │ 📋 Planned│ +6-8 ore   │
│ 006 │ Full System Integration Test │ 📋 Planned│ +4-6 ore   │
│ 007 │ Countdown Running Mode       │ 📋 Planned│ +4-6 ore   │
│ 008 │ Memory Budget Documentation  │ 📋 Planned│ +2-3 ore   │
└───────────────────────────────────────────────────────────────┘

Total Estimate: 32-42 ore (4-5 giorni lavorativi)
```

#### Success Criteria (Go/No-Go Decision)
```gherkin
✅ MUST HAVE (Blockers):
- Display 7-segmenti mostra time stabile senza flickering
- Keypad 4×4 rileva tutti i 16 tasti correttamente
- Countdown editing funziona con cursor navigation e digit increment/decrement
- Validazione time (max 23:59) funziona correttamente
- Sistema stabile per 1 ora continuous operation senza crash
- RAM usage < 80% (1638/2048 bytes)
- Nessun memory leak (stats ogni 10min stabili)

🟡 SHOULD HAVE (Desiderabili ma non blockers):
- Colon blink visibile e corretto timing (500ms)
- Countdown timer decrementa automaticamente ogni 1 sec
- Audio beep ogni secondo durante countdown
- Battery monitor con hysteresis (no LED flapping)

⭕ NICE TO HAVE (Opzionali per MVP):
- Mode selection screen con timeout
- Serial commands estesi (m/v/k/d)
- Unit tests con Unity framework
```

#### Milestone Gate Review
**Data prevista**: 15 Gennaio 2026

Decision Matrix:
- **GO (continue to M2)**: Tutti i MUST HAVE completati + 50% SHOULD HAVE
- **ITERATE (stay on M1)**: < 80% MUST HAVE completati
- **PIVOT**: >3 settimane oltre ETA e MUST HAVE < 60%

#### Risks & Mitigation

| Risk | Probabilità | Impatto | Mitigazione |
|------|-------------|---------|-------------|
| Display flickering non risolvibile con keypad | Media | Alto | Profiling dettagliato, time-slicing keypad scan, worst case: scan throttling |
| Countdown validation logic ha edge cases | Bassa | Medio | Unit tests per tutti i boundary cases, peer review logic |
| RAM overflow durante integration | Bassa | Alto | Memory profiling per modulo, ottimizzare prima di integrare |
| Wokwi simulation diverge da hardware reale | Media | Medio | Test su Arduino fisico in parallelo, accept Wokwi limitations |

---

### Milestone 2: Enhancement 📋 **PLANNED**
**Target data**: Metà Febbraio 2026 (1-2 settimane dopo M1)  
**Priorità**: 🟡 MEDIUM

#### Obiettivi
Miglioramenti qualità, stabilità, test automation e polish user experience.

#### Features Incluse
```
┌───────────────────────────────────────────────────────────────┐
│ ID  │ Feature                      │ Type       │ Estimate   │
├───────────────────────────────────────────────────────────────┤
│ 009 │ Battery Monitor Hysteresis   │ Refactor   │ 3-4 ore    │
│ 010 │ Colon Visual Verification    │ Test       │ 2-3 ore    │
│ 011 │ Serial Commands Extended     │ Feature    │ 2-3 ore    │
│ 012 │ Unit Test Framework Setup    │ Infra      │ 4-6 ore    │
│ 013 │ Wokwi Circuit Validation     │ Test       │ 2-3 ore    │
│ 014 │ Code Documentation Pass      │ Docs       │ 3-4 ore    │
└───────────────────────────────────────────────────────────────┘

Total Estimate: 16-23 ore (2-3 giorni lavorativi)
```

#### Success Criteria
- Battery monitor non mostra LED flapping vicino thresholds
- Unit test coverage > 50% per core logic (Countdown, Battery)
- CI/CD pipeline con GitHub Actions esegue `pio test` automaticamente
- README.md aggiornato con sezioni keypad + countdown + examples
- Colon blink confermato visibile su hardware reale (non solo Wokwi)

#### Deliverables
- `test/test_embedded/` directory con test cases Unity
- `docs/MEMORY_BUDGET.md` con breakdown dettagliato
- README.md v2.0 con tutorial completo
- GitHub badge con test pass rate

---

### Milestone 3: TacBomb Feature Parity 📋 **OPTIONAL**
**Target data**: Fine Marzo 2026 (4-6 settimane dopo M2)  
**Priorità**: 🟢 LOW (opzionale, dipende da decisione product)

#### Obiettivi
Implementazione completa features TacBomb Pro per trasformare progetto da "display + keypad" a "bomb defusal game completo".

#### Features Incluse
```
┌───────────────────────────────────────────────────────────────┐
│ ID  │ Feature                      │ Complexity │ Estimate   │
├───────────────────────────────────────────────────────────────┤
│ 015 │ LED RGB Status Indicator     │ Simple     │ 3-4 ore    │
│ 016 │ Wire Sensing (ADC Ladder)    │ Medium     │ 6-8 ore    │
│ 017 │ Game State Machine           │ Complex    │ 8-10 ore   │
│ 018 │ Audio Feedback System        │ Medium     │ 3-4 ore    │
│ 019 │ Ghost Wire Easter Egg        │ Medium     │ 4-6 ore    │
│ 020 │ Hardware PCB Design          │ Complex    │ 16-20 ore  │
└───────────────────────────────────────────────────────────────┘

Total Estimate: 40-52 ore (5-7 giorni lavorativi)
```

#### Success Criteria
- State machine completa: STANDBY → ARMED → COUNTDOWN → DEFUSED/EXPLODED
- Wire cut detection funzionante con sequenza validation
- LED RGB mostra stato corrente con color mapping
- Audio patterns per tutti gli eventi game (tick, warning, success, error)
- Ghost wire easter egg discovery rate ~5% (verificato in 100 test games)
- PCB prototipo assemblato e testato (equivalenza funzionale a breadboard)

#### Go/No-Go Decision Point
**Data decisione**: Post M1 Gate Review (15 Gennaio 2026)

Fattori decisione:
- ✅ **GO se**: M1 completato on-time, team ha capacity, interest di campo airsoft per full game
- ❌ **NO-GO se**: M1 completato con delay >2 settimane, RAM constraints impediscono features aggiuntive
- ⏸️ **DEFER se**: M1 OK ma priorità shift ad altri progetti

#### Hardware Investment
Task 015-019 richiedono componenti aggiuntivi:
- LED RGB common cathode: ~€0.35
- 4× morsettiere + fili colorati: ~€5.00
- Resistor ladder (4× valori specifici): ~€0.50

Task 020 (PCB) richiede budget dedicato:
- PCB prototype 5pz JLCPCB: ~€50 (inclusa spedizione)
- Componenti SMD per 5 board: ~€80-100
- Case IP65 professionale: ~€60 (5× €12)
- **Total investment**: ~€190-210 per 5 prototipi funzionali

---

## 📅 Timeline Gantt (High-Level)

```
2025 Dec  │ 2026 Jan    │ 2026 Feb   │ 2026 Mar   │ 2026 Apr
──────────┼─────────────┼────────────┼────────────┼──────────
          │             │            │            │
M0 ████   │             │            │            │  Foundation
          │             │            │            │  (DONE)
          │             │            │            │
     █████│███████████  │            │            │  M1: MVP Stabile
          │ (IN PROGRESS│)           │            │  (3-4 weeks)
          │             │            │            │
          │         ████│████████    │            │  M2: Enhancement
          │             │ (PLANNED)  │            │  (1-2 weeks)
          │             │            │            │
          │             │       █████│████████████│  M3: TacBomb
          │             │            │ (OPTIONAL) │  (4-6 weeks)
          │             │            │            │
──────────┴─────────────┴────────────┴────────────┴──────────
   Week 1      Week 5        Week 9      Week 13      Week 17
```

---

## 🎯 Q1 2026 Objectives (Gennaio-Marzo)

### January 2026
- **Week 1-2**: Display Flickering Fix (Task 001) + Keypad/Countdown testing (002-003)
- **Week 3**: Display + Keypad integration (004-005), System integration test (006)
- **Week 4**: Countdown running mode (007), Memory documentation (008), **M1 GATE REVIEW**

### February 2026
- **Week 1**: Enhancement batch (Battery hysteresis, Colon verification, Serial commands)
- **Week 2**: Unit test framework setup, Documentation pass, **M2 COMPLETION**
- **Week 3**: Decision checkpoint per M3 (GO/NO-GO)
- **Week 4**: (Se GO) LED RGB + Wire sensing start (015-016)

### March 2026
- **Week 1-2**: Game State Machine implementation (017)
- **Week 3**: Audio system + Ghost wire (018-019)
- **Week 4**: PCB design start (020) se budget approvato

---

## 🚀 Release Strategy

### v1.0.0 — MVP Release (Post M1)
**Target**: Fine Gennaio 2026

```markdown
## What's New in v1.0.0

### ✨ New Features
- 4-digit 7-segment display con multiplexing stabile
- Keypad 4×4 per input utente con debouncing
- Countdown editing (HH:MM format, max 23:59)
- Cursor navigation con wrap (LEFT/RIGHT/UP/DOWN)
- Battery monitor 4-LED con thresholds
- Boot sequence visuale (Red → Orange → Green LEDs)
- Serial debug interface con comandi interattivi

### 🛠️ Technical Improvements
- Display flickering fix (Issue #001)
- Memory optimization (60% RAM, 34% Flash)
- Non-blocking architecture con millis() timers
- Interrupt-driven button handling

### 📚 Documentation
- README.md completo con wiring diagrams
- EPIC_01/02/03 specifications
- API reference per tutti i controllers

### ⚠️ Known Limitations
- Countdown non decrementa automaticamente (static display only)
- No hysteresis in battery monitor (LED flapping possible)
- Colon blink implementato ma non visually verified
```

### v1.1.0 — Enhancement Release (Post M2)
**Target**: Metà Febbraio 2026

```markdown
## What's New in v1.1.0

### ✨ New Features
- Countdown timer running mode (auto-decrement ogni 1s)
- Battery monitor con hysteresis (no LED flapping)
- Extended serial commands (m/v/k/d)

### 🧪 Testing
- Unit test framework (Unity) con >50% coverage
- CI/CD pipeline con GitHub Actions
- Automated testing per ogni commit

### 📚 Documentation
- Memory budget breakdown dettagliato
- Doxygen API documentation
- README v2.0 con esempi completi
```

### v2.0.0 — TacBomb Release (Post M3, Opzionale)
**Target**: Fine Marzo 2026

```markdown
## What's New in v2.0.0

### 🎮 Game Features
- Game state machine completa (ARMED → COUNTDOWN → DEFUSED/EXPLODED)
- Wire cut detection con ADC ladder (4 fili)
- LED RGB status indicator con color mapping
- Audio patterns per eventi game
- Ghost wire easter egg (hidden defusal sequence)

### 🔧 Hardware
- PCB professionale design (KiCad)
- BOM per produzione batch (100 units)
- Assembly guide per PCB
- Case IP65 integration

### 📦 Production Ready
- Netlist KiCad validato
- Cost analysis (€16.73/unit @ 100pz)
- Quality control procedures
```

---

## 📊 Resource Planning

### Team Composition (Current)
- **Developer**: 1 full-time (tu)
- **Tester**: Self (manual testing + automated future)
- **Documentazione**: Self
- **Hardware**: Self (breadboard/Wokwi, future PCB outsourced)

### Time Allocation (Weekly)
```
M1 Phase (Current):
┌────────────────────────────────────────┐
│ Development:    60% (24h/week)         │
│ Testing:        20% (8h/week)          │
│ Documentation:  15% (6h/week)          │
│ Planning:       5%  (2h/week)          │
└────────────────────────────────────────┘

M2 Phase (Enhancement):
┌────────────────────────────────────────┐
│ Development:    40% (16h/week)         │
│ Testing:        30% (12h/week)         │
│ Documentation:  20% (8h/week)          │
│ Automation:     10% (4h/week)          │
└────────────────────────────────────────┘
```

### Hardware Access
- **Wokwi Sim**: 100% available (no hardware needed)
- **Arduino Uno**: 1× unit (per testing fisico parallelo)
- **Components**: Inventory attuale sufficiente per M1+M2
- **PCB fab**: JLCPCB order richiede 2-3 settimane lead time (plan ahead per M3)

---

## 🔄 Review & Adjust Cadence

### Weekly Standup (Self-review)
**When**: Ogni Lunedì mattina  
**Duration**: 30 min  
**Agenda**:
- Review progresso settimana precedente (planned vs actual)
- Identificare blockers (technical, resource, time)
- Adjust priorità se necessario
- Plan tasks per settimana corrente

### Sprint Review (Milestone Gate)
**When**: Fine di ogni milestone (M1/M2/M3)  
**Duration**: 2 ore  
**Agenda**:
- Demo di tutte le features implementate
- Validation contro success criteria
- Review lessons learned
- GO/NO-GO decision per next milestone
- Update ROADMAP se necessario

### Retrospective
**When**: Post ogni milestone  
**Focus**:
- What went well? (da continuare)
- What went wrong? (da evitare)
- What can improve? (process/tools/docs)

---

## 📈 Success Metrics

### Technical Health
| Metric | Target | Current | Status |
|--------|--------|---------|--------|
| Build Success Rate | 100% | 100% | ✅ |
| RAM Usage | <80% | 60.5% | ✅ |
| Flash Usage | <60% | 34.2% | ✅ |
| Compilation Warnings | 0 critical | 0 | ✅ |
| Test Coverage | >50% | 0% | ⏳ Planned M2 |
| Memory Leaks | 0 | ? | ⏳ Test needed |

### Feature Completion
| Milestone | Total Tasks | Completed | In Progress | Remaining |
|-----------|-------------|-----------|-------------|-----------|
| M0: Foundation | 8 | 8 | 0 | 0 |
| M1: MVP Stabile | 8 | 0 | 3 | 5 |
| M2: Enhancement | 6 | 0 | 0 | 6 |
| M3: TacBomb | 6 | 0 | 0 | 6 |

### Quality Gates
```
✅ PASS: Feature compila senza errori
✅ PASS: Feature testata manualmente (100% acceptance criteria)
✅ PASS: Documentazione aggiornata (README + file task)
✅ PASS: Memory impact documentato (<+5% RAM per feature)
⏳ FUTURE: Unit tests passano (quando framework disponibile)
⏳ FUTURE: Integration test automatizzati
```

---

## 🎯 Long-Term Vision (Post Q1 2026)

### Q2 2026: Productization (Se M3 completato)
- Manufacturing partnership con campo airsoft locale
- Beta testing program (10 unità sul campo)
- Feedback loop e iteration
- Cost optimization per produzione batch

### Q3 2026: Scale (Optional)
- Produzione 50-100 unità
- Distribuzione via Etsy/Amazon
- Support e warranty program
- Community building (forum, Discord)

### Q4 2026: Iterate
- V2.1 con features richieste da community
- Alternative hardware (ESP32 per WiFi?) 
- Mobile app companion (Bluetooth control)

---

## 📝 Change Log Roadmap

### v1.0 (19 Dicembre 2025)
- Initial roadmap creation
- Milestones M0/M1/M2/M3 defined
- Timeline Q1 2026 established
- Success criteria documented

---

**Roadmap Ownership**: Antonio Cittadino  
**Review Frequency**: Weekly (update progress), Monthly (adjust priorities)  
**Next Review**: 23 Dicembre 2025 (post holidays planning)

Per dettagli task specifici, vedere:
- `IN_PROGRESS.md` — Task attivi (001-003)
- `BACKLOG.md` — Task pianificati (004-020)
- `DONE.md` — Task completati (M0)
- `GANTT.md` — Timeline visuale dettagliata
