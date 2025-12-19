# Task #020: Hardware PCB Design

**ID**: 020  
**Titolo**: Design PCB 2-Layer con KiCad + BOM  
**Tipo**: 🔧 HARDWARE  
**Priorità**: 🔵 LOW (OPTIONAL)  
**Stato**: 📋 BACKLOG  
**Milestone**: M3 — TacBomb Parity  

**Stima**: 16-20 ore  
**Dipendenze**: Task 017 (all features finalized)  
**Files**: `hardware/kicad/` (schematic, PCB layout, gerbers)  

---

## 📋 Descrizione

Design PCB custom 2-layer per TacBomb: Arduino Nano mount, display connector, keypad header, battery sensing circuit, wire terminals, RGB LED, buzzer. Generate gerbers per produzione, BOM completo con Digikey part numbers.

---

## ✅ Acceptance Criteria

- [ ] KiCad schematic completo (all connections verified)
- [ ] PCB layout 2-layer, max 100×80mm (cost-effective size)
- [ ] Gerber files generated (ready per JLCPCB/PCBWay)
- [ ] BOM con part numbers, total cost <€30/board (qty 10)
- [ ] 3D render PCB per visualization

---

## 🛠️ PCB Features

- Arduino Nano socket (through-hole headers)
- 4×7-segment display connector (10-pin)
- 4×4 keypad membrane connector (8-pin)
- Battery input (screw terminal, 9V)
- Voltage divider circuit on-board (R1/R2 10kΩ)
- RGB LED footprint + resistors
- Buzzer mount (piezo through-hole)

---

## 📚 Teoria (da completare)

- Hardware: PCB design best practices, trace width calculations
- Tools: KiCad workflow, gerber file formats

---

**Task Owner**: Antonio Cittadino  
**Last Updated**: 19 Dicembre 2025
