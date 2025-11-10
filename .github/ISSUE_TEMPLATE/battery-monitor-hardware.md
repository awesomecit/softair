---
name: Battery Monitor Hardware Setup
about: Implementare il circuito hardware per il monitoraggio batteria (STORY da EPIC_01)
title: '[HARDWARE] Potenziometro + Circuito rilevamento carica batteria'
labels: hardware, epic-01, story
assignees: ''
---

## 📋 STORY: Hardware task — Potenziometro + Circuito per rilevamento carica batteria

**Titolo:** Aggiungere il potenziometro e il circuito per determinare la carica batteria (hardware only)

**Descrizione breve:**
Implementare l'hardware necessario per misurare la tensione della batteria usando un partitore di tensione e cablare i 4 LED di stato (FULL/GOOD/LOW/CRITICAL). 

⚠️ **Questa story è SOLO hardware: nessuna modifica software richiesta in questa fase.**

---

## ✅ Acceptance Criteria (Hardware)

- [ ] **Circuito con partitore tensione installato**: VBatt → R1 (10kΩ) → A0 → R2 (10kΩ) → GND (vedi EPIC_01)
  - Verifica con multimetro: A0 deve leggere **~4.5V** con VBatt = 9V
  - Calcolo: Vout = VBatt × (R2/(R1+R2)) = 9V × 0.5 = 4.5V
  
- [ ] **Potenziometro meccanico** (opzionale) montato come simulatore/test
  - Connettore separato che permette variare Vout tra 0..Vout_max per test manuale
  
- [ ] **4 LED collegati** ai pin D2/D3/D4/D5 con resistenze da 330Ω
  - Anodo al pin, catodo a GND
  - Test manuale: ciascun LED si accende quando il corrispondente pin è portato HIGH
  
- [ ] **Test points** aggiunti sul partitore e sul pin A0
  - Per misurazioni e debug
  - Etichettare chiaramente sul PCB/breadboard
  
- [ ] **Documentazione fisica** aggiornata
  - Sezione Hardware in `README.md` con schema wiring
  - Foto/breadboard layout
  - Lista componenti completa (valori R, pin, orientamento LED)
  
- [ ] **Safety check** completato
  - Verificare che Vout ≤ VREF (5V) con la massima VBatt prevista
  - Aggiungere nota nel README su come cambiare R1/R2 se si usa batteria con tensione maggiore

---

## 🔧 Componenti Richiesti

| Componente | Quantità | Valore | Note |
|------------|----------|--------|------|
| Resistenze (voltage divider) | 2 | 10kΩ | Tolleranza 1% consigliata |
| Resistenze (LED) | 4 | 330Ω | Current limiting |
| LED Green | 1 | 5mm | FULL state (D2) |
| LED Yellow | 1 | 5mm | GOOD state (D3) |
| LED Orange | 1 | 5mm | LOW state (D4) |
| LED Red | 1 | 5mm | CRITICAL state (D5) |
| Potenziometro | 1 | 10kΩ | Opzionale, per test |
| Connettori / test points | - | - | Per breadboard |
| Multimetro | 1 | - | Per verifica tensione |

---

## 🔌 Wiring Schema (breve)

```
VBatt (9V) ──┬── R1 (10kΩ) ──┬── node Vout ──┬── R2 (10kΩ) ── GND
             │                │               │
             │                └───────────────┼─── A0 (Arduino analog input)
             │                                │
             └────────────────────────────────┼─── VIN (Arduino power)

Arduino D2 ── LED_GREEN ──── 330Ω ── GND
Arduino D3 ── LED_YELLOW ─── 330Ω ── GND
Arduino D4 ── LED_ORANGE ─── 330Ω ── GND
Arduino D5 ── LED_RED ────── 330Ω ── GND
```

**Voltage Divider Calculation:**
- VBatt = 9.0V
- R1 = R2 = 10kΩ
- Vout = VBatt × (R2/(R1+R2)) = 9V × 0.5 = **4.5V** ✅ (safe for 5V ADC)
- ADC Reading: (4.5V / 5V) × 1023 = **921 counts**

---

## 🧪 Test Hardware Steps

1. **Build voltage divider on breadboard**
   - Usare R1=R2=10kΩ
   - Connettere VBatt (9V) e misurare al node Vout
   - **Atteso:** ~4.5V ±0.1V

2. **Collegare Arduino e leggere A0**
   - Alimentare Arduino
   - Leggere A0 con multimetro (o serial print in fase software successiva)
   - **Atteso:** ADC counts ≈ 921 a 4.5V

3. **Test potenziometro** (se presente)
   - Variare potenziometro
   - Verificare che Vout varia tra 0..Vout_max
   - Controllare con multimetro

4. **Test LED wiring**
   - Applicare 5V a ciascun pin momentaneamente (via jumper)
   - Verificare orientamento LED e valori resistenze
   - Tutti i LED devono accendersi quando il pin è HIGH

5. **Documentazione fotografica**
   - Scattare foto del circuito montato
   - Aggiungere schema di wiring in README
   - Includere lista componenti e orientamento LED

---

## 📝 Notes per Software Follow-up (NON implementare ora)

- Software implementerà `BatteryMonitor` (vedi `EPIC_01.md`) usando:
  - Moving-average sampling (10 campioni @ 100ms)
  - Thresholds con hysteresis
- L'hardware deve fornire **Vout stabile** per questi algoritmi
- Considerare aggiunta di **RC filter** se ADC noise è un problema (discussione in software ticket successivo)

---

## 🔗 Collegamenti

- Epic di riferimento: `EPIC_01.md`
- Pattern architetturali: `.github/copilot-instructions.md` (sezione SOLID/DRY)
- Documentazione: `README.md` (da aggiornare con questa story)

---

## 🎯 Definition of Done

- ✅ Circuito montato su breadboard e funzionante
- ✅ Tutte le misure verificate con multimetro
- ✅ LED testati individualmente
- ✅ Foto del circuito caricate
- ✅ README.md aggiornato con schema wiring
- ✅ Safety check completato (Vout ≤ 5V)
- ✅ Peer review del circuito (se applicabile)
