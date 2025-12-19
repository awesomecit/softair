# TacBomb Pro — Schema Elettrico Completo

## Indice
1. [Configurazione Display e Colon](#1-configurazione-display-e-colon)
2. [Schema a Blocchi Generale](#2-schema-a-blocchi-generale)
3. [Pin Assignment ATmega328P](#3-pin-assignment-atmega328p)
4. [Sottosistema Display](#4-sottosistema-display)
5. [Sottosistema Keypad e Pulsanti](#5-sottosistema-keypad-e-pulsanti)
6. [Sottosistema Wire Sensing](#6-sottosistema-wire-sensing)
7. [Sottosistema Audio](#7-sottosistema-audio)
8. [Sottosistema LED di Stato](#8-sottosistema-led-di-stato)
9. [Sottosistema Alimentazione](#9-sottosistema-alimentazione)
10. [Schema Completo Unificato](#10-schema-completo-unificato)
11. [Note Software per Colon](#11-note-software-per-colon)
12. [File KiCad Export](#12-file-kicad-export)

---

## 1. Configurazione Display e Colon

### 1.1 Layout Display — Formato HH:MM con Colon Lampeggiante

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                                                                             │
│                         DISPLAY FRONTALE TacBomb Pro                        │
│                                                                             │
│    ┏━━━━━━━━━┓      ┏━━━━━━━━━┓            ┏━━━━━━━━━┓      ┏━━━━━━━━━┓    │
│    ┃    A    ┃      ┃    A    ┃     ██     ┃    A    ┃      ┃    A    ┃    │
│    ┃  ┌───┐  ┃      ┃  ┌───┐  ┃            ┃  ┌───┐  ┃      ┃  ┌───┐  ┃    │
│    ┃ F│   │B ┃      ┃ F│   │B ┃     ██     ┃ F│   │B ┃      ┃ F│   │B ┃    │
│    ┃  ├─G─┤  ┃      ┃  ├─G─┤  ┃            ┃  ├─G─┤  ┃      ┃  ├─G─┤  ┃    │
│    ┃ E│   │C ┃      ┃ E│   │C ┃            ┃ E│   │C ┃      ┃ E│   │C ┃    │
│    ┃  └───┘  ┃      ┃  └───┘  ┃            ┃  └───┘  ┃      ┃  └───┘  ┃    │
│    ┃    D    ┃      ┃    D    ┃            ┃    D    ┃      ┃    D    ┃    │
│    ┗━━━━━━━━━┛      ┗━━━━━━━━━┛            ┗━━━━━━━━━┛      ┗━━━━━━━━━┛    │
│                           │                     │                           │
│                          DP2                   DP3                          │
│                      (colon basso)         (colon alto)                     │
│                                                                             │
│      DIGIT 1            DIGIT 2      ":"      DIGIT 3            DIGIT 4   │
│    (Decine Min)       (Unità Min)   COLON   (Decine Sec)       (Unità Sec) │
│                                                                             │
│    ════════════════════════════════════════════════════════════════════    │
│                                                                             │
│    Esempio Countdown:    0  9  :  4  5     →   "09:45"                     │
│                             ██                                              │
│                             ██    (colon lampeggia 0.5s ON / 0.5s OFF)     │
│                                                                             │
│    Esempio Settaggio:    0  9  :  4  5     →   "09:45"                     │
│                             ██                                              │
│                             ██    (colon FISSO durante impostazione)       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 Struttura Interna Display 4-Digit con Colon

```
                    DISPLAY 7-SEGMENTI 4-DIGIT (Common Cathode)
                    Modello consigliato: 5641AS, 5461BS, LTC-4727
    
    ┌─────────────────────────────────────────────────────────────────────────┐
    │                                                                         │
    │            DIGIT 1         DIGIT 2         DIGIT 3         DIGIT 4     │
    │           ┌───────┐       ┌───────┐       ┌───────┐       ┌───────┐    │
    │           │   A   │       │   A   │       │   A   │       │   A   │    │
    │           │ F   B │       │ F   B │       │ F   B │       │ F   B │    │
    │           │   G   │   ●   │   G   │   ●   │   G   │       │   G   │    │
    │           │ E   C │  DP2  │ E   C │  DP3  │ E   C │       │ E   C │    │
    │           │   D   │       │   D   │       │   D   │       │   D   │    │
    │           └───┬───┘       └───┬───┘       └───┬───┘       └───┬───┘    │
    │               │               │               │               │        │
    │              CC1             CC2             CC3             CC4       │
    │          (cathode)       (cathode)       (cathode)       (cathode)     │
    │                                                                         │
    │    ═══════════════════════════════════════════════════════════════     │
    │                                                                         │
    │    PINOUT TIPICO (12 pin):                                             │
    │                                                                         │
    │       Pin 1  ── Segment E      Pin 12 ── Segment A                     │
    │       Pin 2  ── Segment D      Pin 11 ── Segment F                     │
    │       Pin 3  ── DP (comune)    Pin 10 ── CC1 (Digit 1)                 │
    │       Pin 4  ── Segment C      Pin 9  ── CC2 (Digit 2)                 │
    │       Pin 5  ── Segment G      Pin 8  ── CC3 (Digit 3)                 │
    │       Pin 6  ── CC4 (Digit 4)  Pin 7  ── Segment B                     │
    │                                                                         │
    │    NOTA: Il DP è connesso internamente a tutti i digit.                │
    │          Per il COLON useremo:                                         │
    │          - DP attivo durante scan di DIGIT 2 → punto basso ":"        │
    │          - DP attivo durante scan di DIGIT 3 → punto alto ":"         │
    │                                                                         │
    └─────────────────────────────────────────────────────────────────────────┘
```

### 1.3 Timing del Colon

```
    MODALITÀ COUNTDOWN (timer attivo):
    
    Tempo ───────────────────────────────────────────────────────────────►
    
    Colon     ████████████████                ████████████████
    (DP2+DP3) ████████████████                ████████████████
              ████████████████                ████████████████
              │◄──── 500ms ───►│◄── 500ms ──►│◄──── 500ms ───►│
              │      ON        │     OFF      │      ON        │
              
              └────────────── Periodo 1 secondo ──────────────┘
    
    
    MODALITÀ SETTAGGIO (configurazione timer):
    
    Tempo ───────────────────────────────────────────────────────────────►
    
    Colon     ████████████████████████████████████████████████████████████
    (DP2+DP3) ████████████████████████████████████████████████████████████
              ████████████████████████████████████████████████████████████
              │◄──────────────────── SEMPRE ON ─────────────────────────►│
    
    
    DIGIT LAMPEGGIANTE (selezione digit in settaggio):
    
    Tempo ───────────────────────────────────────────────────────────────►
    
    Digit     ████████████        ████████████        ████████████
    Selezion. ████████████        ████████████        ████████████
              │◄── 250ms ►│◄250ms►│◄── 250ms ►│◄250ms►│
              │    ON     │  OFF  │    ON     │  OFF  │
              
              └─────────── Periodo 500ms (più veloce del colon) ─────────┘
```

---

## 2. Schema a Blocchi Generale

```
┌──────────────────────────────────────────────────────────────────────────────────────────┐
│                                 TacBomb Pro — Block Diagram                               │
└──────────────────────────────────────────────────────────────────────────────────────────┘

                                      ┌──────────────────┐
                                      │    BATTERIE      │
                                      │    4x AA         │
                                      │    (6V nom.)     │
                                      └────────┬─────────┘
                                               │
                        ┌──────────────────────┼──────────────────────┐
                        │                      │                      │
                        ▼                      ▼                      ▼
              ┌──────────────────┐   ┌──────────────────┐   ┌──────────────────┐
              │ REVERSE PROTECT  │   │    REGULATOR     │   │  BATTERY MONITOR │
              │    1N4007        │   │     LM7805       │   │   ADC Divider    │
              └────────┬─────────┘   └────────┬─────────┘   └────────┬─────────┘
                       │                      │                      │
                       └──────────┬───────────┘                      │
                                  │                                  │
                               +5V Rail ◄────────────────────────────┘
                                  │
        ┌─────────────────────────┼─────────────────────────┐
        │                         │                         │
        ▼                         ▼                         ▼
┌───────────────┐       ┌─────────────────┐       ┌───────────────────┐
│    INPUT      │       │   ATmega328P    │       │      OUTPUT       │
│   SECTION     │       │                 │       │      SECTION      │
│               │       │   ┌─────────┐   │       │                   │
│ ┌───────────┐ │       │   │         │   │       │ ┌───────────────┐ │
│ │ KEYPAD    │ ├──────►│   │  CPU    │   │──────►│ │   DISPLAY     │ │
│ │ 4x4       │ │ 8 pin │   │         │   │ 8 pin │ │   4x7-seg     │ │
│ │           │ │       │   │ 16MHz   │   │       │ │   + Colon     │ │
│ └───────────┘ │       │   │ Crystal │   │       │ └───────────────┘ │
│               │       │   │         │   │       │                   │
│ ┌───────────┐ │       │   │ 32KB    │   │       │ ┌───────────────┐ │
│ │ WIRE      │ ├──────►│   │ Flash   │   │──────►│ │   LED RGB     │ │
│ │ SENSORS   │ │ 1 ADC │   │         │   │ 3 pin │ │   Status      │ │
│ │ (4 wires) │ │       │   │ 2KB     │   │       │ └───────────────┘ │
│ └───────────┘ │       │   │ SRAM    │   │       │                   │
│               │       │   │         │   │       │ ┌───────────────┐ │
│ ┌───────────┐ │       │   │ 1KB     │   │──────►│ │   BUZZER      │ │
│ │ BATTERY   │ ├──────►│   │ EEPROM  │   │ 1 pin │ │   Piezo       │ │
│ │ SENSE     │ │ 1 ADC │   │         │   │ (PWM) │ │   >85dB       │ │
│ └───────────┘ │       │   └─────────┘   │       │ └───────────────┘ │
│               │       │                 │       │                   │
└───────────────┘       └─────────────────┘       └───────────────────┘


                    ┌─────────────────────────────────────────┐
                    │           CONNESSIONI ESTERNE           │
                    ├─────────────────────────────────────────┤
                    │  • FTDI Header (6 pin) - Programmazione │
                    │  • ISP Header (6 pin) - Bootloader      │
                    │  • Wire Terminals (4x2) - Fili gioco    │
                    │  • Battery Holder - 4xAA                │
                    │  • Power Switch - ON/OFF                │
                    └─────────────────────────────────────────┘
```

---

## 3. Pin Assignment ATmega328P

### 3.1 Tabella Allocazione Pin Completa

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                        ATmega328P-PU (DIP-28) — Pin Assignment                          │
├──────┬────────┬────────────┬──────────────────────────────────────────────────────────────┤
│ Pin  │ Nome   │ Direzione  │ Funzione                                                    │
├──────┼────────┼────────────┼──────────────────────────────────────────────────────────────┤
│  1   │ PC6    │ INPUT      │ RESET (10kΩ pull-up to VCC, 100nF to GND)                  │
│  2   │ PD0    │ OUTPUT     │ 74HC595 DATA (Segment data serial out)                     │
│  3   │ PD1    │ OUTPUT     │ 74HC595 CLOCK (Shift clock)                                │
│  4   │ PD2    │ OUTPUT     │ 74HC595 LATCH (Storage register clock)                     │
│  5   │ PD3    │ OUTPUT     │ Digit 1 Select (via BC547 NPN) - Decine Minuti            │
│  6   │ PD4    │ OUTPUT     │ Digit 2 Select (via BC547 NPN) - Unità Minuti + DP colon  │
│  7   │ VCC    │ POWER      │ +5V (100nF bypass cap to GND)                              │
│  8   │ GND    │ POWER      │ Ground                                                      │
│  9   │ PB6    │ XTAL       │ Crystal 16MHz (22pF to GND)                                │
│ 10   │ PB7    │ XTAL       │ Crystal 16MHz (22pF to GND)                                │
│ 11   │ PD5    │ OUTPUT     │ Digit 3 Select (via BC547 NPN) - Decine Sec + DP colon    │
│ 12   │ PD6    │ OUTPUT     │ Digit 4 Select (via BC547 NPN) - Unità Secondi            │
│ 13   │ PD7    │ OUTPUT     │ Buzzer PWM (via BC547 NPN)                                 │
│ 14   │ PB0    │ OUTPUT     │ LED RGB - RED (330Ω resistor)                              │
│ 15   │ PB1    │ OUTPUT     │ LED RGB - GREEN (330Ω resistor)                            │
│ 16   │ PB2    │ OUTPUT     │ LED RGB - BLUE (330Ω resistor)                             │
│ 17   │ PB3    │ OUTPUT     │ Keypad ROW 1 (scan output)                                 │
│ 18   │ PB4    │ OUTPUT     │ Keypad ROW 2 (scan output)                                 │
│ 19   │ PB5    │ OUTPUT     │ Keypad ROW 3 (scan output)                                 │
│ 20   │ AVCC   │ POWER      │ +5V Analog (100nF + 10µH to VCC)                           │
│ 21   │ AREF   │ ANALOG     │ ADC Reference (100nF to GND, or VCC)                       │
│ 22   │ GND    │ POWER      │ Ground (Analog)                                             │
│ 23   │ PC0    │ OUTPUT     │ Keypad ROW 4 (scan output)                                 │
│ 24   │ PC1    │ INPUT      │ Keypad COL 1 (input with pull-up)                          │
│ 25   │ PC2    │ INPUT      │ Keypad COL 2 (input with pull-up)                          │
│ 26   │ PC3    │ INPUT      │ Keypad COL 3 (input with pull-up)                          │
│ 27   │ PC4    │ INPUT/ADC  │ Keypad COL 4 + Wire Sense ADC (multiplexed)               │
│ 28   │ PC5    │ INPUT/ADC  │ Battery Voltage ADC (divider 10k+10k)                      │
├──────┴────────┴────────────┴──────────────────────────────────────────────────────────────┤
│  Totale I/O utilizzati: 20/20 (100%)                                                     │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### 3.2 Diagramma Pinout Visuale

```
                                    ┌────────────────────┐
                                    │    ATmega328P-PU   │
                                    │      (DIP-28)      │
                                    │                    │
                         RESET ○────┤ 1  PC6      PC5 28 ├────○ BATTERY_ADC
                    595_DATA ○────┤ 2  PD0      PC4 27 ├────○ WIRE_ADC / KEY_COL4
                     595_CLK ○────┤ 3  PD1      PC3 26 ├────○ KEY_COL3
                   595_LATCH ○────┤ 4  PD2      PC2 25 ├────○ KEY_COL2
                      DIGIT1 ○────┤ 5  PD3      PC1 24 ├────○ KEY_COL1
                      DIGIT2 ○────┤ 6  PD4      PC0 23 ├────○ KEY_ROW4
                         VCC ●────┤ 7  VCC      GND 22 ├────● GND
                         GND ●────┤ 8  GND     AREF 21 ├────○ AREF (100nF)
                      XTAL1 ◇────┤ 9  PB6     AVCC 20 ├────● AVCC
                      XTAL2 ◇────┤ 10 PB7      PB5 19 ├────○ KEY_ROW3
                      DIGIT3 ○────┤ 11 PD5      PB4 18 ├────○ KEY_ROW2
                      DIGIT4 ○────┤ 12 PD6      PB3 17 ├────○ KEY_ROW1
                      BUZZER ○────┤ 13 PD7      PB2 16 ├────○ LED_BLUE
                      LED_RED ○────┤ 14 PB0      PB1 15 ├────○ LED_GREEN
                                    │                    │
                                    └────────────────────┘
                                    
    Legenda:  ● = Power    ○ = I/O    ◇ = Crystal
```

---

## 4. Sottosistema Display

### 4.1 Schema Driver Display con 74HC595

```
                                         VCC (+5V)
                                            │
                 ┌──────────────────────────┼──────────────────────────┐
                 │                          │                          │
                 │                         ─┴─                         │
                 │                        100nF                        │
                 │                         ─┬─                         │
                 │                          │                          │
                 │                         GND                         │
                 │                                                     │
                 │              ┌──────────────────────┐               │
                 │              │      74HC595         │               │
                 │              │    Shift Register    │               │
                 │              │                      │               │
                 │     VCC ─────┤ 16 VCC    GND 8     ├───── GND      │
                 │              │                      │               │
    PD0 ─────────┼──────────────┤ 14 SER    Q7' 9     ├───── NC       │
   (DATA)        │              │                      │               │
                 │              │                      │               │
    PD1 ─────────┼──────────────┤ 11 SRCLK  OE  13    ├───── GND      │
   (CLOCK)       │              │                      │               │
                 │              │                      │               │
    PD2 ─────────┼──────────────┤ 12 RCLK   MR  10    ├───── VCC      │
  (LATCH)        │              │                      │               │
                 │              │      Q0  Q1  Q2  Q3  Q4  Q5  Q6  Q7  │
                 │              │      15  1   2   3   4   5   6   7   │
                 │              └──────┼───┼───┼───┼───┼───┼───┼───┼───┘
                 │                     │   │   │   │   │   │   │   │
                 │                    ─┴─ ─┴─ ─┴─ ─┴─ ─┴─ ─┴─ ─┴─ ─┴─
                 │                    R1  R2  R3  R4  R5  R6  R7  R8
                 │                   220Ω (tutti i resistori)
                 │                    ─┬─ ─┬─ ─┬─ ─┬─ ─┬─ ─┬─ ─┬─ ─┬─
                 │                     │   │   │   │   │   │   │   │
                 │                     │   │   │   │   │   │   │   │
                 │                     A   B   C   D   E   F   G   DP
                 │                     │   │   │   │   │   │   │   │
                 │    ┌────────────────┴───┴───┴───┴───┴───┴───┴───┴────────────────┐
                 │    │                                                              │
                 │    │              DISPLAY 4-DIGIT 7-SEGMENTI                     │
                 │    │                  (Common Cathode)                            │
                 │    │                                                              │
                 │    │    ┌─────┐    ┌─────┐    ┌─────┐    ┌─────┐                 │
                 │    │    │     │    │     │ ██ │     │    │     │                 │
                 │    │    │ D1  │    │ D2  │ ██ │ D3  │    │ D4  │                 │
                 │    │    │     │ ●  │     │    │     │    │     │                 │
                 │    │    └──┬──┘    └──┬──┘    └──┬──┘    └──┬──┘                 │
                 │    │       │          │          │          │                    │
                 │    │      CC1        CC2        CC3        CC4                   │
                 │    │                                                              │
                 │    └──────┼──────────┼──────────┼──────────┼─────────────────────┘
                 │           │          │          │          │
                 │           │          │          │          │
                 │       ┌───┴───┐  ┌───┴───┐  ┌───┴───┐  ┌───┴───┐
                 │       │  Q1   │  │  Q2   │  │  Q3   │  │  Q4   │
                 │       │BC547  │  │BC547  │  │BC547  │  │BC547  │
                 │       │  NPN  │  │  NPN  │  │  NPN  │  │  NPN  │
                 │       └───┬───┘  └───┬───┘  └───┬───┘  └───┬───┘
                 │        B  │ E     B  │ E     B  │ E     B  │ E
                 │        │  │      │  │      │  │      │  │
                 │       ─┴─ │     ─┴─ │     ─┴─ │     ─┴─ │
                 │       1kΩ │     1kΩ │     1kΩ │     1kΩ │
                 │       ─┬─ │     ─┬─ │     ─┬─ │     ─┬─ │
                 │        │  │      │  │      │  │      │  │
                 │       PD3 GND   PD4 GND   PD5 GND   PD6 GND
                 │
                 └─────────────────────────────────────────────────────────────────────
```

### 4.2 Tabella Mappatura Bit 74HC595

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    74HC595 Output Bit Assignment                             │
├──────────┬────────┬─────────────────────────────────────────────────────────┤
│ 595 Pin  │ Bit    │ Funzione                                                │
├──────────┼────────┼─────────────────────────────────────────────────────────┤
│ Q0 (15)  │ bit 0  │ Segment A (top)                                         │
│ Q1 (1)   │ bit 1  │ Segment B (top-right)                                   │
│ Q2 (2)   │ bit 2  │ Segment C (bottom-right)                                │
│ Q3 (3)   │ bit 3  │ Segment D (bottom)                                      │
│ Q4 (4)   │ bit 4  │ Segment E (bottom-left)                                 │
│ Q5 (5)   │ bit 5  │ Segment F (top-left)                                    │
│ Q6 (6)   │ bit 6  │ Segment G (middle)                                      │
│ Q7 (7)   │ bit 7  │ Segment DP (decimal point / COLON)                      │
├──────────┴────────┴─────────────────────────────────────────────────────────┤
│                                                                              │
│  Byte da inviare = | DP | G | F | E | D | C | B | A |                       │
│                      7    6   5   4   3   2   1   0                          │
│                                                                              │
│  NOTA COLON:                                                                 │
│  ───────────                                                                 │
│  Il DP (bit 7) viene acceso SOLO durante la scansione di DIGIT 2 e DIGIT 3  │
│  per creare l'effetto del colon ":" centrale.                               │
│                                                                              │
│  Durante countdown: DP lampeggia (500ms ON, 500ms OFF)                      │
│  Durante settaggio: DP sempre ON                                             │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 4.3 Sequenza Multiplexing Display

```
    Ciclo di refresh display (target: 100Hz per digit = 400Hz totale = 2.5ms per ciclo)
    
    Tempo ──────────────────────────────────────────────────────────────────────►
    
    DIGIT 1 ████████                                                ████████
    (PD3)   │ 625µs │                                               │ 625µs │
            
    DIGIT 2          ████████                                                
    (PD4)            │ 625µs │  ← Se colon_on: invia byte con DP=1
    
    DIGIT 3                   ████████                               
    (PD5)                     │ 625µs │  ← Se colon_on: invia byte con DP=1
    
    DIGIT 4                            ████████                      
    (PD6)                              │ 625µs │
    
            └───────────────── 2.5ms ciclo completo ─────────────────┘
    
    
    PSEUDOCODICE REFRESH:
    ──────────────────────
    
    void refresh_display() {
        static uint8_t current_digit = 0;
        static uint32_t last_colon_toggle = 0;
        static bool colon_visible = true;
        
        // Gestione lampeggio colon (solo in modalità countdown)
        if (mode == MODE_COUNTDOWN) {
            if (millis() - last_colon_toggle >= 500) {
                colon_visible = !colon_visible;
                last_colon_toggle = millis();
            }
        } else {
            colon_visible = true;  // Sempre acceso in settaggio
        }
        
        // Spegni digit precedente
        PORTD &= ~(0x78);  // Clear PD3-PD6
        
        // Prepara byte segmenti
        uint8_t segments = digit_segments[current_digit];
        
        // Aggiungi DP per colon su digit 2 e 3
        if ((current_digit == 1 || current_digit == 2) && colon_visible) {
            segments |= 0x80;  // Set bit 7 (DP)
        }
        
        // Invia a 74HC595
        shift_out(segments);
        
        // Accendi digit corrente
        PORTD |= (1 << (3 + current_digit));
        
        // Prossimo digit
        current_digit = (current_digit + 1) % 4;
    }
```

---

## 5. Sottosistema Keypad e Pulsanti

### 5.1 Schema Keypad 4x4 (include pulsanti funzione)

```
                                        VCC (+5V)
                                           │
                    ┌──────────────────────┼──────────────────────┐
                    │                      │                      │
                   ─┴─                    ─┴─                    ─┴─
                   10kΩ                   10kΩ                   10kΩ        (Pull-up interni MCU)
                   ─┬─                    ─┬─                    ─┬─
                    │                      │                      │
    ┌───────────────┼──────────────────────┼──────────────────────┼──────────────────────┐
    │               │                      │                      │                      │
    │            COL 1                  COL 2                  COL 3                  COL 4
    │            (PC1)                  (PC2)                  (PC3)                  (PC4)
    │               │                      │                      │                      │
    │    ┌──────────┼──────────────────────┼──────────────────────┼──────────────────────┤
    │    │          │                      │                      │                      │
    │ ROW 1 ────────┼──────────┬───────────┼──────────┬───────────┼──────────┬───────────┤
    │ (PB3)         │          │           │          │           │          │           │
    │               │        ┌─┴─┐         │        ┌─┴─┐         │        ┌─┴─┐         │
    │               ├────────┤ 1 ├─────────┼────────┤ 2 ├─────────┼────────┤ 3 ├─────────┤
    │               │        └───┘         │        └───┘         │        └───┘         │
    │               │                      │                      │                    ┌─┴─┐
    │               │                      │                      │                    │ARM│
    │               │                      │                      │                    └─┬─┘
    │    ┌──────────┼──────────────────────┼──────────────────────┼──────────────────────┤
    │    │          │                      │                      │                      │
    │ ROW 2 ────────┼──────────┬───────────┼──────────┬───────────┼──────────┬───────────┤
    │ (PB4)         │          │           │          │           │          │           │
    │               │        ┌─┴─┐         │        ┌─┴─┐         │        ┌─┴─┐         │
    │               ├────────┤ 4 ├─────────┼────────┤ 5 ├─────────┼────────┤ 6 ├─────────┤
    │               │        └───┘         │        └───┘         │        └───┘         │
    │               │                      │                      │                    ┌─┴─┐
    │               │                      │                      │                    │STR│
    │               │                      │                      │                    └─┬─┘
    │    ┌──────────┼──────────────────────┼──────────────────────┼──────────────────────┤
    │    │          │                      │                      │                      │
    │ ROW 3 ────────┼──────────┬───────────┼──────────┬───────────┼──────────┬───────────┤
    │ (PB5)         │          │           │          │           │          │           │
    │               │        ┌─┴─┐         │        ┌─┴─┐         │        ┌─┴─┐         │
    │               ├────────┤ 7 ├─────────┼────────┤ 8 ├─────────┼────────┤ 9 ├─────────┤
    │               │        └───┘         │        └───┘         │        └───┘         │
    │               │                      │                      │                    ┌─┴─┐
    │               │                      │                      │                    │ OK│
    │               │                      │                      │                    └─┬─┘
    │    ┌──────────┼──────────────────────┼──────────────────────┼──────────────────────┤
    │    │          │                      │                      │                      │
    │ ROW 4 ────────┼──────────┬───────────┼──────────┬───────────┼──────────┬───────────┤
    │ (PC0)         │          │           │          │           │          │           │
    │               │        ┌─┴─┐         │        ┌─┴─┐         │        ┌─┴─┐         │
    │               └────────┤ * ├─────────┴────────┤ 0 ├─────────┴────────┤ # ├─────────┤
    │                        └───┘                  └───┘                  └───┘         │
    │                                                                                  ┌─┴─┐
    │                                                                                  │CLR│
    │                                                                                  └─┬─┘
    │                                                                                    │
    └────────────────────────────────────────────────────────────────────────────────────┘
    
    
    LAYOUT KEYPAD FISICO:
    ┌─────┬─────┬─────┬─────┐
    │  1  │  2  │  3  │ ARM │  ← Arma il dispositivo (rosso)
    ├─────┼─────┼─────┼─────┤
    │  4  │  5  │  6  │START│  ← Avvia countdown (verde)
    ├─────┼─────┼─────┼─────┤
    │  7  │  8  │  9  │ OK  │  ← Conferma selezione (blu)
    ├─────┼─────┼─────┼─────┤
    │  *  │  0  │  #  │ CLR │  ← Clear/Cancel (giallo)
    └─────┴─────┴─────┴─────┘
```

### 5.2 Circuito Debounce Hardware

```
    Per ogni pulsante della colonna 4 (funzioni critiche), aggiungiamo RC debounce:
    
                    VCC
                     │
                    ─┴─
                    10kΩ  (pull-up)
                    ─┬─
                     │
                     ├─────────────────┬───────────► a MCU (PC4)
                     │                 │
                   ┌─┴─┐             ─┬─
                   │BTN│             100nF  (debounce)
                   └─┬─┘             ─┴─
                     │                 │
                    ───               ───
                    GND               GND
    
    
    Costante di tempo: τ = R × C = 10kΩ × 100nF = 1ms
    Tempo di debounce effettivo: ~5τ = 5ms
    
    
    NOTA: Il debounce software aggiuntivo nel firmware usa 50ms per sicurezza.
```

---

## 6. Sottosistema Wire Sensing

### 6.1 Schema Sensing 4 Fili con ADC Ladder

```
                                              VCC (+5V)
                                                 │
                    ┌────────────────────────────┤
                    │                            │
                    │                           ─┴─
                    │                           10kΩ  R_ref (fisso)
                    │                           ─┬─
                    │                            │
                    │                            ├─────────────────────► PC4 (ADC4)
                    │                            │
                    │    ┌───────────────────────┼───────────────────────┐
                    │    │                       │                       │
                    │    │      ┌────────────────┼────────────────┐      │
                    │    │      │                │                │      │
                    │    │      │    ┌───────────┼───────────┐    │      │
                    │    │      │    │           │           │    │      │
                    │    │      │    │    ┌──────┴──────┐    │    │      │
                    │    │      │    │    │             │    │    │      │
                    │   ─┴─    ─┴─  ─┴─  ─┴─           ─┴─  ─┴─  ─┴─    ─┴─
                    │   10kΩ   22kΩ 47kΩ 100kΩ        100nF             │
                    │   ─┬─    ─┬─  ─┬─  ─┬─           ─┬─              │
                    │    │      │    │    │             │               │
                    │    │      │    │    │            ───              │
                    │    │      │    │    │            GND              │
                    │    │      │    │    │                             │
                    │ ┌──┴──┐┌──┴──┐┌─┴──┐┌─┴──┐                        │
                    │ │WIRE1││WIRE2││WIRE3││WIRE4│                       │
                    │ │ RED ││ORG  ││YEL  ││GRN  │                       │
                    │ └──┬──┘└──┬──┘└──┬──┘└──┬──┘                        │
                    │    │      │      │      │                          │
                    │    │      │      │      │                          │
                    │   ═══    ═══    ═══    ═══    (Morsettiere)        │
                    │  ═════  ═════  ═════  ═════                        │
                    │    │      │      │      │                          │
                    │    └──────┴──────┴──────┴──────────────────────────┘
                    │                         │
                    │                        ───
                    │                        GND
                    │
                    └────────────────────────────────────────────────────────────
    
    
    CALCOLO VALORI ADC PER OGNI COMBINAZIONE:
    ══════════════════════════════════════════
    
    Formula: V_adc = VCC × (R_parallel / (R_ref + R_parallel))
    
    Dove R_parallel = parallelo di tutti i fili COLLEGATI
    
    ┌──────────────────────────────────────────────────────────────────────────┐
    │  Fili collegati        │ R_parallel │ V_adc │ ADC Value │ Margine        │
    │  (1=collegato,0=tagliato)                    (10-bit)                    │
    ├──────────────────────────────────────────────────────────────────────────┤
    │  1111 (tutti)          │  4.17 kΩ   │ 1.47V │   301     │                │
    │  1110 (no W4)          │  4.70 kΩ   │ 1.60V │   327     │  +26           │
    │  1101 (no W3)          │  5.37 kΩ   │ 1.75V │   358     │  +31           │
    │  1100 (no W3,W4)       │  6.88 kΩ   │ 2.04V │   417     │  +59           │
    │  1011 (no W2)          │  6.19 kΩ   │ 1.91V │   391     │                │
    │  1010 (no W2,W4)       │  7.44 kΩ   │ 2.13V │   436     │                │
    │  1001 (no W2,W3)       │  9.17 kΩ   │ 2.39V │   489     │                │
    │  1000 (solo W1)        │  10.0 kΩ   │ 2.50V │   512     │                │
    │  0111 (no W1)          │  6.97 kΩ   │ 2.05V │   419     │                │
    │  0110 (no W1,W4)       │  8.65 kΩ   │ 2.32V │   475     │                │
    │  0101 (no W1,W3)       │  11.0 kΩ   │ 2.62V │   536     │                │
    │  0100 (solo W2)        │  22.0 kΩ   │ 3.44V │   704     │                │
    │  0011 (no W1,W2)       │  14.9 kΩ   │ 2.99V │   612     │                │
    │  0010 (solo W3)        │  47.0 kΩ   │ 4.12V │   843     │                │
    │  0001 (solo W4)        │  100 kΩ    │ 4.55V │   930     │                │
    │  0000 (tutti tagliati) │  ∞ (open)  │ 5.00V │  1023     │                │
    └──────────────────────────────────────────────────────────────────────────┘
    
    
    ALGORITMO RILEVAMENTO:
    ──────────────────────
    
    uint8_t read_wires() {
        uint16_t adc = analogRead(A4);
        uint8_t wires = 0;
        
        // Lookup table con soglie (valori medi tra stati adiacenti)
        if (adc < 314)  wires = 0b1111;  // Tutti collegati
        else if (adc < 342) wires = 0b1110;  // W4 tagliato
        else if (adc < 387) wires = 0b1101;  // W3 tagliato
        // ... etc per tutte le 16 combinazioni
        else wires = 0b0000;  // Tutti tagliati
        
        return wires;
    }
```

### 6.2 Morsettiere Fisiche

```
    Vista frontale dei connettori fili:
    
    ┌────────────────────────────────────────────────────────────────────┐
    │                         PANEL FILI                                 │
    │                                                                    │
    │   ╔═══════════╗  ╔═══════════╗  ╔═══════════╗  ╔═══════════╗     │
    │   ║   WIRE 1  ║  ║   WIRE 2  ║  ║   WIRE 3  ║  ║   WIRE 4  ║     │
    │   ║    RED    ║  ║  ORANGE   ║  ║  YELLOW   ║  ║   GREEN   ║     │
    │   ║  ┌─────┐  ║  ║  ┌─────┐  ║  ║  ┌─────┐  ║  ║  ┌─────┐  ║     │
    │   ║  │ ● ● │  ║  ║  │ ● ● │  ║  ║  │ ● ● │  ║  ║  │ ● ● │  ║     │
    │   ║  └─────┘  ║  ║  └─────┘  ║  ║  └─────┘  ║  ║  └─────┘  ║     │
    │   ╚═══════════╝  ╚═══════════╝  ╚═══════════╝  ╚═══════════╝     │
    │        │              │              │              │             │
    │        ▼              ▼              ▼              ▼             │
    │   ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐       │
    │   │ Filo    │    │ Filo    │    │ Filo    │    │ Filo    │       │
    │   │ rosso   │    │ arancio │    │ giallo  │    │ verde   │       │
    │   │ (20cm)  │    │ (20cm)  │    │ (20cm)  │    │ (20cm)  │       │
    │   └────┬────┘    └────┬────┘    └────┬────┘    └────┬────┘       │
    │        │              │              │              │             │
    │        ▼              ▼              ▼              ▼             │
    │   ╔═══════════╗  ╔═══════════╗  ╔═══════════╗  ╔═══════════╗     │
    │   ║  ┌─────┐  ║  ║  ┌─────┐  ║  ║  ┌─────┐  ║  ║  ┌─────┐  ║     │
    │   ║  │ ● ● │  ║  ║  │ ● ● │  ║  ║  │ ● ● │  ║  ║  │ ● ● │  ║     │
    │   ║  └─────┘  ║  ║  └─────┘  ║  ║  └─────┘  ║  ║  └─────┘  ║     │
    │   ╚═══════════╝  ╚═══════════╝  ╚═══════════╝  ╚═══════════╝     │
    │                                                                    │
    │   Il giocatore deve TAGLIARE il filo giusto per disattivare!      │
    │                                                                    │
    └────────────────────────────────────────────────────────────────────┘
```

---

## 7. Sottosistema Audio

### 7.1 Schema Driver Buzzer

```
                                    VCC (+5V)
                                       │
                                       │
                              ┌────────┴────────┐
                              │     BUZZER      │
                              │   Piezoelettrico│
                              │     Attivo      │
                              │    (5V, 85dB)   │
                              │                 │
                              │    (+)   (-)    │
                              └───┬────────┬────┘
                                  │        │
                                  │        │
                               ┌──┴──┐     │
                               │     │     │
                               │ Q5  │     │
                               │BC547│     │
                               │ NPN │     │
                               └──┬──┘     │
                                C │ E      │
                                  │ │      │
                                  │ └──────┤
                                  │        │
                                 ─┴─      ───
                                 1kΩ      GND
                                 ─┬─
                                  │
                                  │
                                 PD7 (MCU)
    
    
    DIODO DI PROTEZIONE (opzionale ma consigliato):
    
                                    VCC (+5V)
                                       │
                              ┌────────┴────────┐
                              │                 │
                              │     BUZZER      │◄───┐
                              │                 │    │
                              └───┬─────────────┘    │
                                  │                  │
                                  │      ┌───────────┤
                                  │      │           │
                               ┌──┴──┐   │         ──┴──
                               │BC547│   │          /\   1N4148
                               └──┬──┘   │         /  \  (flyback)
                                  │      │        ──┬──
                                  └──────┼──────────┘
                                         │
                                        ───
                                        GND
    
    
    FREQUENZE TONI:
    ═══════════════
    
    ┌────────────────────────────────────────────────────────────────┐
    │  Evento                    │ Frequenza │ Durata   │ Pattern    │
    ├────────────────────────────────────────────────────────────────┤
    │  Beep countdown (normale)  │  1000 Hz  │ 100ms    │ ogni 1s    │
    │  Beep countdown (urgente)  │  2000 Hz  │  50ms    │ ogni 250ms │
    │  Conferma tasto            │  1500 Hz  │  50ms    │ singolo    │
    │  Errore codice             │   400 Hz  │ 200ms    │ 3x ripetuto│
    │  Disinnesco OK             │  2500 Hz  │ 100ms    │ scala asc. │
    │  Esplosione                │   200 Hz  │  5 sec   │ continuo   │
    │  Batteria bassa            │   800 Hz  │  50ms    │ 2x ogni 30s│
    └────────────────────────────────────────────────────────────────┘
    
    
    GENERAZIONE TONO CON TIMER (esempio):
    ─────────────────────────────────────
    
    Per 1000Hz con Timer2:
    
    Prescaler = 64
    Compare = (16MHz / 64 / 1000Hz / 2) - 1 = 124
    
    void tone_1kHz() {
        TCCR2A = (1 << WGM21);              // CTC mode
        TCCR2B = (1 << CS22);               // Prescaler 64
        OCR2A = 124;                        // Compare value
        TIMSK2 = (1 << OCIE2A);             // Enable interrupt
    }
    
    ISR(TIMER2_COMPA_vect) {
        PORTD ^= (1 << PD7);                // Toggle buzzer
    }
```

---

## 8. Sottosistema LED di Stato

### 8.1 Schema LED RGB Common Cathode

```
                                  VCC (+5V)
                                     │
         ┌───────────────────────────┼───────────────────────────┐
         │                           │                           │
         │                           │                           │
        ─┴─                         ─┴─                         ─┴─
        330Ω                        330Ω                        330Ω
        ─┬─                         ─┬─                         ─┬─
         │                           │                           │
         │    ┌──────────────────────┼──────────────────────┐    │
         │    │                      │                      │    │
         │    │         ┌────────────┴────────────┐         │    │
         │    │         │                         │         │    │
         │    │         │    LED RGB 5mm          │         │    │
         │    │         │    Common Cathode       │         │    │
         │    │         │                         │         │    │
         │    │         │    ┌───┐   ┌───┐   ┌───┐│         │    │
         │    │         │    │ R │   │ G │   │ B ││         │    │
         │    │         │    │LED│   │LED│   │LED││         │    │
         │    │         │    └─┬─┘   └─┬─┘   └─┬─┘│         │    │
         │    │         │      │       │       │  │         │    │
         └────┼─────────┼──────┘       │       └──┼─────────┼────┘
              │         │              │          │         │
              │         │              │          │         │
             PB0       ─┴─            PB1        ─┴─       PB2
            (RED)      GND          (GREEN)      GND     (BLUE)
                        │                         │
                        └─────────────────────────┘
                                    │
                                   GND
    
    
    NOTA: I resistori sono calcolati per:
    - I_led = (5V - V_forward) / R
    - Rosso:  V_f = 2.0V → I = 9mA
    - Verde:  V_f = 2.2V → I = 8.5mA
    - Blu:    V_f = 3.0V → I = 6mA
    
    
    TABELLA COLORI E STATI:
    ═══════════════════════
    
    ┌───────────────────────────────────────────────────────────────────────┐
    │  Stato dispositivo        │ R │ G │ B │ Colore    │ Modalità         │
    ├───────────────────────────────────────────────────────────────────────┤
    │  Standby                  │ 0 │ 1 │ 0 │ VERDE     │ Fisso            │
    │  Settaggio timer          │ 0 │ 1 │ 1 │ CIANO     │ Fisso            │
    │  Armato (pronto)          │ 1 │ 1 │ 0 │ GIALLO    │ Fisso            │
    │  Countdown attivo         │ 1 │ 0 │ 0 │ ROSSO     │ Lampeggio lento  │
    │  Countdown < 1min         │ 1 │ 0 │ 0 │ ROSSO     │ Lampeggio rapido │
    │  Countdown < 10sec        │ 1 │ 0 │ 0 │ ROSSO     │ Lampeggio veloce │
    │  Disinnescato             │ 0 │ 1 │ 0 │ VERDE     │ Lampeggio        │
    │  Esploso                  │ 1 │ 0 │ 0 │ ROSSO     │ Fisso 5sec       │
    │  Errore codice            │ 1 │ 0 │ 1 │ MAGENTA   │ Flash 3x         │
    │  Batteria bassa           │ 1 │ 0.5│ 0 │ ARANCIONE │ Pulse            │
    └───────────────────────────────────────────────────────────────────────┘
    
    
    NOTA PWM per ARANCIONE:
    Rosso = ON (100%)
    Verde = PWM 50% (per mixing con rosso = arancione)
    Blu = OFF
```

---

## 9. Sottosistema Alimentazione

### 9.1 Schema Completo Power Supply

```
                                    BATTERIE 4x AA (6V nominale)
                                    ┌─────────────────────┐
                                    │  ┌───┐┌───┐┌───┐┌───┐│
                                    │  │1.5││1.5││1.5││1.5││
                                    │  │ V ││ V ││ V ││ V ││
                                    │  └─┬─┘└─┬─┘└─┬─┘└─┬─┘│
                                    │    └──┬─┴──┬─┘    │  │
                                    │       │    │      │  │
                                    └───────┼────┼──────┼──┘
                                            │    │      │
                                           (+)  (-)     │
                                            │    │      │
     ┌──────────────────────────────────────┼────┼──────┘
     │                                      │    │
     │                     ┌────────────────┤    │
     │   POWER SWITCH      │                │    │
     │   ┌─────────────┐   │               ─┴─   │
     │   │    ON/OFF   │   │           D1  /_\  1N4007
     │   │   ┌─┬─┐     │   │               ─┬─   │
     │   │   │ │█│◄────┼───┘                │    │
     │   │   └─┴─┘     │                    │    │
     │   └──────┬──────┘                    │    │
     │          │                           │    │
     │          └───────────────────────────┤    │
     │                                      │    │
     │                             V_batt   │    │
     │              (5.4V - 6.4V) ──────────┤    │
     │                                      │    │
     │         ┌────────────────────────────┤    │
     │         │                            │    │
     │         │                ┌───────────┼────┘
     │         │                │           │
     │         │              ─┬─          ─┴─
     │         │          C1  ─┴─ 100µF    C2  100nF
     │         │              ─┬─          ─┬─
     │         │                │           │
     │         │                └─────┬─────┘
     │         │                      │
     │         │    ┌─────────────────┴─────────────────┐
     │         │    │                                   │
     │         │    │            LM7805                 │
     │         │    │         ┌─────────┐               │
     │         │    │    VIN  │         │ VOUT          │
     │         └────┼────────►│    1    │───────┬───────┼────────► +5V Rail
     │              │         │         │       │       │
     │              │         │    2    │      ─┴─     ─┴─
     │              │         │   GND   │  C3  ─┬─ C4  ─┬─
     │              │         └────┬────┘  10µF ─┴─100nF─┴─
     │              │              │           │       │
     │              │              └───────────┴───────┴────────► GND
     │              │
     │              │
     │              │    BATTERY MONITORING (ADC)
     │              │    ═══════════════════════
     │              │
     │              │              V_batt (6V max)
     │              │                   │
     │              └───────────────────┤
     │                                  │
     │                                 ─┴─
     │                                 10kΩ  R1
     │                                 ─┬─
     │                                  │
     │                                  ├───────────────────► PC5 (ADC5)
     │                                  │
     │                                 ─┴─
     │                                 10kΩ  R2
     │                                 ─┬─
     │                                  │
     │                                 ─┴─
     │                                 100nF  C_filter
     │                                 ─┬─
     │                                  │
     │                                 GND
     │
     │
     │    CALCOLO TENSIONE BATTERIA:
     │    ═══════════════════════════
     │
     │    V_adc = V_batt × (R2 / (R1 + R2)) = V_batt × 0.5
     │
     │    ADC_value = (V_adc / 5.0) × 1023
     │
     │    Quindi: V_batt = (ADC_value × 5.0 × 2) / 1023
     │
     │    ┌──────────────────────────────────────────────────────┐
     │    │  Stato batteria  │ V_batt  │ V_adc  │ ADC   │ LED    │
     │    ├──────────────────────────────────────────────────────┤
     │    │  Piena           │ >5.8V   │ >2.9V  │ >593  │ 4 LED  │
     │    │  Buona           │ 5.4-5.8 │ 2.7-2.9│ 552-593│ 3 LED  │
     │    │  Media           │ 5.0-5.4 │ 2.5-2.7│ 512-552│ 2 LED  │
     │    │  Bassa           │ 4.6-5.0 │ 2.3-2.5│ 471-512│ 1 LED  │
     │    │  Critica (<4.6V) │ <4.6V   │ <2.3V  │ <471  │ Flash  │
     │    └──────────────────────────────────────────────────────┘
     │
     └────────────────────────────────────────────────────────────────────
```

---

## 10. Schema Completo Unificato

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                                                 │
│                                    TacBomb Pro — COMPLETE SCHEMATIC                                            │
│                                                                                                                 │
│  ╔══════════════════════════════════════════════════════════════════════════════════════════════════════════╗  │
│  ║                                                                                                          ║  │
│  ║   POWER SUPPLY                         MICROCONTROLLER                        DISPLAY                   ║  │
│  ║   ════════════                         ══════════════                        ═══════                    ║  │
│  ║                                                                                                          ║  │
│  ║   4xAA ──► 1N4007 ──► LM7805 ──► +5V   ┌────────────────────┐               ┌──────────────────────┐   ║  │
│  ║    6V       │           │               │   ATmega328P-PU   │               │  4-DIGIT 7-SEGMENT   │   ║  │
│  ║             │          ─┴─              │                   │    74HC595    │    COMMON CATHODE    │   ║  │
│  ║            ─┴─        100nF             │    ┌─────────┐    │   ┌───────┐   │                      │   ║  │
│  ║           100µF        ─┬─              │    │         │    │   │       │   │  ┌───┐┌───┐┌───┐┌───┐│   ║  │
│  ║            ─┬─          │               │    │  16MHz  │    │   │ SHIFT │   │  │ H ││ h ││ M ││ m ││   ║  │
│  ║             │           │               │    │  XTAL   │    │   │  REG  │   │  └─┬─┘└─┬─┘└─┬─┘└─┬─┘│   ║  │
│  ║            GND         GND              │    │         │    │   │       │   │    ●    ● ●        ●  │   ║  │
│  ║                                         │    └─────────┘    │   └───┬───┘   │   DP2  COLON      DP4 │   ║  │
│  ║                                         │                   │       │       │                      │   ║  │
│  ║   ┌───────────────────────────────────►│ PD0 ─────► DATA ──┼───────┘       │  CC1 CC2 CC3 CC4    │   ║  │
│  ║   │                                    │ PD1 ─────► CLK   ─┼───────────────┼───┬───┬───┬───┬─────┘   ║  │
│  ║   │                                    │ PD2 ─────► LATCH ─┤               │   │   │   │   │         ║  │
│  ║   │                                    │                   │               │   Q1  Q2  Q3  Q4        ║  │
│  ║   │                                    │ PD3 ─────► DIG1 ──┼───► BC547 ────┼───┘   │   │   │         ║  │
│  ║   │                                    │ PD4 ─────► DIG2 ──┼───► BC547 ────┼───────┘   │   │         ║  │
│  ║   │                                    │ PD5 ─────► DIG3 ──┼───► BC547 ────┼───────────┘   │         ║  │
│  ║   │                                    │ PD6 ─────► DIG4 ──┼───► BC547 ────┼───────────────┘         ║  │
│  ║   │                                    │                   │               └──────────────────────┘   ║  │
│  ║   │                                    │ PD7 ─────► BUZZER─┼──► BC547 ──► PIEZO (85dB)              ║  │
│  ║   │                                    │                   │                                          ║  │
│  ║   │   LED RGB                          │ PB0 ─────► LED_R ─┼──► 330Ω ──► LED                         ║  │
│  ║   │   ════════                         │ PB1 ─────► LED_G ─┼──► 330Ω ──► LED                         ║  │
│  ║   │                                    │ PB2 ─────► LED_B ─┼──► 330Ω ──► LED                         ║  │
│  ║   │                                    │                   │                                          ║  │
│  ║   │   KEYPAD 4x4                       │ PB3 ─────► ROW1 ──┼──► [1][2][3][ARM]                       ║  │
│  ║   │   ══════════                       │ PB4 ─────► ROW2 ──┼──► [4][5][6][STR]                       ║  │
│  ║   │                                    │ PB5 ─────► ROW3 ──┼──► [7][8][9][OK ]                       ║  │
│  ║   │                                    │ PC0 ─────► ROW4 ──┼──► [*][0][#][CLR]                       ║  │
│  ║   │                                    │                   │       │   │   │   │                      ║  │
│  ║   │                                    │ PC1 ◄───── COL1 ──┼───────┘   │   │   │                      ║  │
│  ║   │                                    │ PC2 ◄───── COL2 ──┼───────────┘   │   │                      ║  │
│  ║   │                                    │ PC3 ◄───── COL3 ──┼───────────────┘   │                      ║  │
│  ║   │                                    │ PC4 ◄───── COL4 ──┼───────────────────┘                      ║  │
│  ║   │                                    │      ◄───── ADC ──┼──► WIRE SENSE (ladder)                  ║  │
│  ║   │                                    │                   │      ├─ 10k ─ WIRE1 (RED)               ║  │
│  ║   │                                    │                   │      ├─ 22k ─ WIRE2 (ORG)               ║  │
│  ║   │   BATTERY MONITOR                  │                   │      ├─ 47k ─ WIRE3 (YEL)               ║  │
│  ║   │   ════════════════                 │                   │      └─100k ─ WIRE4 (GRN)               ║  │
│  ║   │                                    │                   │                                          ║  │
│  ║   └── V_batt ──► 10k ─┬─ 10k ─► GND   │ PC5 ◄───── ADC ──┼───────────────────────────────────────┘   ║  │
│  ║                       │                │                   │                                          ║  │
│  ║                       └───────────────►│                   │                                          ║  │
│  ║                                        │                   │                                          ║  │
│  ║   PROGRAMMING                          │ PC6 ◄───── RESET ┤──► 10k ──► VCC                          ║  │
│  ║   ════════════                         │                   │   │                                      ║  │
│  ║                                        │                   │  100nF                                   ║  │
│  ║   FTDI Header:                         │ PB6 ◄────► XTAL1 ┤   │                                      ║  │
│  ║   [DTR][RX][TX][VCC][CTS][GND]        │ PB7 ◄────► XTAL2 ┤  GND                                     ║  │
│  ║     │              │                   │                   │                                          ║  │
│  ║     └──100nF──► RESET                 └────────────────────┘                                          ║  │
│  ║                                                                                                          ║  │
│  ╚══════════════════════════════════════════════════════════════════════════════════════════════════════════╝  │
│                                                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

---

## 11. Note Software per Colon

### 11.1 Gestione Lampeggio Colon

```c
/**
 * TacBomb Pro - Gestione Display con Colon Lampeggiante
 * 
 * Il colon è formato dai DP di DIGIT 2 e DIGIT 3.
 * - Durante countdown: lampeggia 500ms ON / 500ms OFF
 * - Durante settaggio: sempre acceso
 * - Durante selezione digit: digit selezionato lampeggia 250ms
 */

#include <avr/io.h>
#include <avr/interrupt.h>

// === DEFINIZIONI PIN ===
#define SR_DATA   PD0
#define SR_CLK    PD1
#define SR_LATCH  PD2
#define DIGIT1    PD3
#define DIGIT2    PD4
#define DIGIT3    PD5
#define DIGIT4    PD6

// === VARIABILI GLOBALI ===
volatile uint8_t display_digits[4] = {0, 0, 0, 0};  // Valori 0-9 per ogni digit
volatile uint8_t current_digit = 0;
volatile bool colon_state = true;
volatile bool setting_mode = false;
volatile int8_t selected_digit = -1;  // -1 = nessuno, 0-3 = digit selezionato

// Timer per lampeggio
volatile uint16_t colon_timer = 0;
volatile uint16_t digit_blink_timer = 0;
volatile bool digit_visible = true;

// === LOOKUP TABLE SEGMENTI ===
// Bit order: DP-G-F-E-D-C-B-A (MSB to LSB)
const uint8_t SEGMENT_MAP[10] = {
    0b00111111,  // 0
    0b00000110,  // 1
    0b01011011,  // 2
    0b01001111,  // 3
    0b01100110,  // 4
    0b01101101,  // 5
    0b01111101,  // 6
    0b00000111,  // 7
    0b01111111,  // 8
    0b01101111   // 9
};

// === FUNZIONI SHIFT REGISTER ===
void shift_out(uint8_t data) {
    for (int8_t i = 7; i >= 0; i--) {
        // Set data bit
        if (data & (1 << i)) {
            PORTD |= (1 << SR_DATA);
        } else {
            PORTD &= ~(1 << SR_DATA);
        }
        
        // Clock pulse
        PORTD |= (1 << SR_CLK);
        PORTD &= ~(1 << SR_CLK);
    }
    
    // Latch data
    PORTD |= (1 << SR_LATCH);
    PORTD &= ~(1 << SR_LATCH);
}

// === REFRESH DISPLAY (chiamata da Timer Interrupt ~2.5ms) ===
void refresh_display(void) {
    // Spegni tutti i digit
    PORTD &= ~((1 << DIGIT1) | (1 << DIGIT2) | (1 << DIGIT3) | (1 << DIGIT4));
    
    // Prepara byte segmenti per digit corrente
    uint8_t segments = SEGMENT_MAP[display_digits[current_digit]];
    
    // === GESTIONE COLON ===
    // Aggiungi DP (bit 7) per digit 1 e 2 (formano il ":")
    // NOTA: Il colon è tra D2 e D3, quindi:
    //       - DP di D2 = punto basso ":"
    //       - DP di D3 = punto alto ":"
    if ((current_digit == 1 || current_digit == 2) && colon_state) {
        segments |= 0b10000000;  // Set DP bit
    }
    
    // === GESTIONE DIGIT LAMPEGGIANTE (settaggio) ===
    if (selected_digit == current_digit && !digit_visible) {
        segments = 0x00;  // Spegni digit se in fase OFF del lampeggio
    }
    
    // Invia segmenti a 74HC595
    shift_out(segments);
    
    // Accendi digit corrente (transistor NPN, logica positiva)
    switch (current_digit) {
        case 0: PORTD |= (1 << DIGIT1); break;
        case 1: PORTD |= (1 << DIGIT2); break;
        case 2: PORTD |= (1 << DIGIT3); break;
        case 3: PORTD |= (1 << DIGIT4); break;
    }
    
    // Prossimo digit
    current_digit = (current_digit + 1) % 4;
}

// === TIMER INTERRUPT (ogni 1ms) ===
ISR(TIMER0_COMPA_vect) {
    static uint8_t refresh_counter = 0;
    
    // Refresh display ogni ~0.625ms (625µs per digit = 2.5ms ciclo)
    refresh_counter++;
    if (refresh_counter >= 1) {  // Adjust for desired refresh rate
        refresh_counter = 0;
        refresh_display();
    }
    
    // === LAMPEGGIO COLON (500ms ON / 500ms OFF) ===
    if (!setting_mode) {
        colon_timer++;
        if (colon_timer >= 500) {
            colon_timer = 0;
            colon_state = !colon_state;
        }
    } else {
        colon_state = true;  // Sempre acceso in modalità settaggio
    }
    
    // === LAMPEGGIO DIGIT SELEZIONATO (250ms ON / 250ms OFF) ===
    if (selected_digit >= 0) {
        digit_blink_timer++;
        if (digit_blink_timer >= 250) {
            digit_blink_timer = 0;
            digit_visible = !digit_visible;
        }
    } else {
        digit_visible = true;  // Sempre visibile se nessun digit selezionato
    }
}

// === SETUP TIMER0 ===
void setup_timer0(void) {
    // Timer0 in CTC mode, 1ms interrupt
    TCCR0A = (1 << WGM01);               // CTC mode
    TCCR0B = (1 << CS01) | (1 << CS00);  // Prescaler 64
    OCR0A = 249;                          // (16MHz / 64 / 1000Hz) - 1 = 249
    TIMSK0 = (1 << OCIE0A);              // Enable compare interrupt
}

// === FUNZIONI API ===
void display_set_time(uint8_t minutes, uint8_t seconds) {
    display_digits[0] = minutes / 10;
    display_digits[1] = minutes % 10;
    display_digits[2] = seconds / 10;
    display_digits[3] = seconds % 10;
}

void display_enter_setting_mode(void) {
    setting_mode = true;
    colon_state = true;
}

void display_exit_setting_mode(void) {
    setting_mode = false;
    selected_digit = -1;
    digit_visible = true;
}

void display_select_digit(int8_t digit) {
    selected_digit = digit;  // -1 = nessuno, 0-3 = digit
    digit_blink_timer = 0;
    digit_visible = true;
}
```

### 11.2 Diagramma Stati Colon

```
    ┌─────────────────────────────────────────────────────────────────────┐
    │                     STATE MACHINE - COLON                           │
    └─────────────────────────────────────────────────────────────────────┘
    
                                    ┌───────────┐
                                    │   BOOT    │
                                    └─────┬─────┘
                                          │
                                          ▼
                            ┌─────────────────────────────┐
                            │         STANDBY             │
                            │    Colon: ON (fisso)        │
                            │    Display: "00:00"         │
                            └──────────────┬──────────────┘
                                           │
                          ┌────────────────┼────────────────┐
                          │                │                │
                          ▼                ▼                ▼
               ┌──────────────────┐ ┌──────────────┐ ┌──────────────────┐
               │    SETTAGGIO     │ │   ARMATO     │ │     WIRE         │
               │                  │ │              │ │   SENSING        │
               │ Colon: ON        │ │ Colon: ON    │ │                  │
               │ (sempre fisso)   │ │ (fisso)      │ │ Colon: ON        │
               │                  │ │              │ │ (fisso)          │
               │ Digit selezion.  │ │              │ │                  │
               │ lampeggia 250ms  │ │              │ │                  │
               └────────┬─────────┘ └──────┬───────┘ └────────┬─────────┘
                        │                  │                  │
                        │                  ▼                  │
                        │     ┌────────────────────────┐      │
                        │     │       COUNTDOWN        │      │
                        │     │                        │      │
                        │     │  Colon: LAMPEGGIANTE   │      │
                        │     │  500ms ON / 500ms OFF  │      │
                        │     │                        │      │
                        │     │  ┌──────────────────┐  │      │
                        │     │  │ < 60 sec:        │  │      │
                        │     │  │ LED rosso rapido │  │      │
                        │     │  └──────────────────┘  │      │
                        │     │                        │      │
                        │     │  ┌──────────────────┐  │      │
                        │     │  │ < 10 sec:        │  │      │
                        │     │  │ Beep continuo    │  │      │
                        │     │  └──────────────────┘  │      │
                        │     └───────────┬────────────┘      │
                        │                 │                   │
                        │    ┌────────────┴────────────┐      │
                        │    │                         │      │
                        ▼    ▼                         ▼      ▼
               ┌──────────────────┐           ┌──────────────────┐
               │   DISINNESCATO   │           │     ESPLOSO      │
               │                  │           │                  │
               │ Colon: OFF       │           │ Colon: OFF       │
               │ Display: "dEF"   │           │ Display: "----"  │
               │ LED: Verde flash │           │ LED: Rosso fisso │
               └──────────────────┘           └──────────────────┘
```

---

## 12. File KiCad Export

### 12.1 Netlist in Formato KiCad (.net)

```
(export (version D)
  (design
    (source "TacBomb_Pro.sch")
    (date "2024-12-19")
    (tool "Hand-generated for KiCad import")
  )
  (components
    ; Microcontroller
    (comp (ref U1) (value ATmega328P-PU) (footprint Package_DIP:DIP-28_W7.62mm))
    
    ; Crystal
    (comp (ref Y1) (value 16MHz) (footprint Crystal:Crystal_HC49-4H_Vertical))
    (comp (ref C1) (value 22pF) (footprint Capacitor_THT:C_Disc_D3.0mm_W1.6mm_P2.50mm))
    (comp (ref C2) (value 22pF) (footprint Capacitor_THT:C_Disc_D3.0mm_W1.6mm_P2.50mm))
    
    ; Shift Register
    (comp (ref U2) (value 74HC595) (footprint Package_DIP:DIP-16_W7.62mm))
    
    ; Display
    (comp (ref DS1) (value 5641AS) (footprint Display_7Segment:7SegmentLED_LTS6760_LTS6780))
    
    ; Transistors for digit select
    (comp (ref Q1) (value BC547) (footprint Package_TO_SOT_THT:TO-92_Inline))
    (comp (ref Q2) (value BC547) (footprint Package_TO_SOT_THT:TO-92_Inline))
    (comp (ref Q3) (value BC547) (footprint Package_TO_SOT_THT:TO-92_Inline))
    (comp (ref Q4) (value BC547) (footprint Package_TO_SOT_THT:TO-92_Inline))
    (comp (ref Q5) (value BC547) (footprint Package_TO_SOT_THT:TO-92_Inline))
    
    ; Voltage Regulator
    (comp (ref U3) (value LM7805) (footprint Package_TO_SOT_THT:TO-220-3_Vertical))
    
    ; Protection Diode
    (comp (ref D1) (value 1N4007) (footprint Diode_THT:D_DO-41_SOD81_P10.16mm_Horizontal))
    
    ; LED RGB
    (comp (ref LED1) (value RGB_CC) (footprint LED_THT:LED_D5.0mm-4_RGB))
    
    ; Buzzer
    (comp (ref BZ1) (value Buzzer) (footprint Buzzer_Beeper:Buzzer_12x9.5RM7.6))
    
    ; Resistors - Segments (220 ohm)
    (comp (ref R1) (value 220) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R2) (value 220) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R3) (value 220) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R4) (value 220) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R5) (value 220) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R6) (value 220) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R7) (value 220) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R8) (value 220) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    
    ; Resistors - Transistor base (1k)
    (comp (ref R9) (value 1k) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R10) (value 1k) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R11) (value 1k) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R12) (value 1k) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R13) (value 1k) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    
    ; Resistors - LED RGB (330 ohm)
    (comp (ref R14) (value 330) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R15) (value 330) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R16) (value 330) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    
    ; Resistors - Wire sensing ladder
    (comp (ref R17) (value 10k) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R18) (value 10k) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R19) (value 22k) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R20) (value 47k) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R21) (value 100k) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    
    ; Resistors - Battery divider
    (comp (ref R22) (value 10k) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    (comp (ref R23) (value 10k) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    
    ; Resistor - Reset pullup
    (comp (ref R24) (value 10k) (footprint Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal))
    
    ; Capacitors - Bypass
    (comp (ref C3) (value 100nF) (footprint Capacitor_THT:C_Disc_D3.0mm_W1.6mm_P2.50mm))
    (comp (ref C4) (value 100nF) (footprint Capacitor_THT:C_Disc_D3.0mm_W1.6mm_P2.50mm))
    (comp (ref C5) (value 100nF) (footprint Capacitor_THT:C_Disc_D3.0mm_W1.6mm_P2.50mm))
    (comp (ref C6) (value 100nF) (footprint Capacitor_THT:C_Disc_D3.0mm_W1.6mm_P2.50mm))
    
    ; Capacitors - Power supply
    (comp (ref C7) (value 100uF) (footprint Capacitor_THT:CP_Radial_D5.0mm_P2.00mm))
    (comp (ref C8) (value 10uF) (footprint Capacitor_THT:CP_Radial_D5.0mm_P2.00mm))
    
    ; Connectors
    (comp (ref J1) (value Conn_01x06) (footprint Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical) (datasheet "FTDI Header"))
    (comp (ref J2) (value Screw_Terminal_01x02) (footprint TerminalBlock:TerminalBlock_bornier-2_P5.08mm))
    (comp (ref J3) (value Screw_Terminal_01x02) (footprint TerminalBlock:TerminalBlock_bornier-2_P5.08mm))
    (comp (ref J4) (value Screw_Terminal_01x02) (footprint TerminalBlock:TerminalBlock_bornier-2_P5.08mm))
    (comp (ref J5) (value Screw_Terminal_01x02) (footprint TerminalBlock:TerminalBlock_bornier-2_P5.08mm))
    (comp (ref J6) (value Conn_01x02) (footprint Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical) (datasheet "Battery"))
    
    ; Keypad
    (comp (ref SW1) (value Keypad_4x4) (footprint Button_Switch_Keyboard:Keypad_4x4))
  )
  
  (nets
    ; Power nets
    (net (code 1) (name VCC) ...)
    (net (code 2) (name GND) ...)
    (net (code 3) (name +5V) ...)
    
    ; MCU to 74HC595
    (net (code 10) (name SR_DATA) (node (ref U1) (pin 2)) (node (ref U2) (pin 14)))
    (net (code 11) (name SR_CLK) (node (ref U1) (pin 3)) (node (ref U2) (pin 11)))
    (net (code 12) (name SR_LATCH) (node (ref U1) (pin 4)) (node (ref U2) (pin 12)))
    
    ; Digit select
    (net (code 20) (name DIG1) (node (ref U1) (pin 5)) (node (ref R9) (pin 1)))
    (net (code 21) (name DIG2) (node (ref U1) (pin 6)) (node (ref R10) (pin 1)))
    (net (code 22) (name DIG3) (node (ref U1) (pin 11)) (node (ref R11) (pin 1)))
    (net (code 23) (name DIG4) (node (ref U1) (pin 12)) (node (ref R12) (pin 1)))
    
    ; Segments A-G, DP
    (net (code 30) (name SEG_A) (node (ref U2) (pin 15)) (node (ref R1) (pin 1)))
    (net (code 31) (name SEG_B) (node (ref U2) (pin 1)) (node (ref R2) (pin 1)))
    (net (code 32) (name SEG_C) (node (ref U2) (pin 2)) (node (ref R3) (pin 1)))
    (net (code 33) (name SEG_D) (node (ref U2) (pin 3)) (node (ref R4) (pin 1)))
    (net (code 34) (name SEG_E) (node (ref U2) (pin 4)) (node (ref R5) (pin 1)))
    (net (code 35) (name SEG_F) (node (ref U2) (pin 5)) (node (ref R6) (pin 1)))
    (net (code 36) (name SEG_G) (node (ref U2) (pin 6)) (node (ref R7) (pin 1)))
    (net (code 37) (name SEG_DP) (node (ref U2) (pin 7)) (node (ref R8) (pin 1)))
    
    ; ... (additional nets for keypad, wire sensing, etc.)
  )
)
```

### 12.2 Bill of Materials per KiCad

```csv
"Reference","Value","Footprint","Qty"
"U1","ATmega328P-PU","Package_DIP:DIP-28_W7.62mm","1"
"U2","74HC595","Package_DIP:DIP-16_W7.62mm","1"
"U3","LM7805","Package_TO_SOT_THT:TO-220-3_Vertical","1"
"Y1","16MHz","Crystal:Crystal_HC49-4H_Vertical","1"
"DS1","5641AS","Display_7Segment:7SegmentLED_LTS6760","1"
"Q1-Q5","BC547","Package_TO_SOT_THT:TO-92_Inline","5"
"D1","1N4007","Diode_THT:D_DO-41_SOD81_P10.16mm","1"
"LED1","RGB_CC","LED_THT:LED_D5.0mm-4_RGB","1"
"BZ1","Buzzer","Buzzer_Beeper:Buzzer_12x9.5RM7.6","1"
"R1-R8","220","Resistor_THT:R_Axial_DIN0207","8"
"R9-R13","1k","Resistor_THT:R_Axial_DIN0207","5"
"R14-R16","330","Resistor_THT:R_Axial_DIN0207","3"
"R17-R18,R22-R24","10k","Resistor_THT:R_Axial_DIN0207","5"
"R19","22k","Resistor_THT:R_Axial_DIN0207","1"
"R20","47k","Resistor_THT:R_Axial_DIN0207","1"
"R21","100k","Resistor_THT:R_Axial_DIN0207","1"
"C1-C2","22pF","Capacitor_THT:C_Disc_D3.0mm","2"
"C3-C6","100nF","Capacitor_THT:C_Disc_D3.0mm","4"
"C7","100uF","Capacitor_THT:CP_Radial_D5.0mm","1"
"C8","10uF","Capacitor_THT:CP_Radial_D5.0mm","1"
"J1","FTDI_Header","Connector_PinHeader_2.54mm:PinHeader_1x06","1"
"J2-J5","Screw_Terminal","TerminalBlock:TerminalBlock_bornier-2_P5.08mm","4"
"J6","Battery_Conn","Connector_PinHeader_2.54mm:PinHeader_1x02","1"
"SW1","Keypad_4x4","Button_Switch_Keyboard:Keypad_4x4","1"
```

---

## Appendice A — Legenda Simboli

```
    ═══════════════════════════════════════════════════════════════════
    
    COMPONENTI:
    
    ─┴─         Resistore (con valore sopra)
    ─┬─
    
    ─┴─         Condensatore (ceramico/film)
    ─┬─
    
    ─┬─         Condensatore elettrolitico (polarizzato)
    ═╪═
    ─┴─
    
    ─►|─        Diodo (catodo a destra)
    
    ─►LED─      LED
    
     C          Transistor NPN (BCE = Base, Collector, Emitter)
    B─┤
     E
    
    ┌───┐       IC / Chip
    │   │
    └───┘
    
    ●           Nodo di connessione / Punto di saldatura
    
    ○           Pin non connesso
    
    ───         Connessione elettrica
    
    ─ ─ ─       Connessione opzionale / alternativa
    
    ═══════════════════════════════════════════════════════════════════
```

---

*Documento generato per TacBomb Pro v1.0 — Dicembre 2024*
*Compatibile con KiCad 7.0+, VSCode Markdown Preview, GitHub*
