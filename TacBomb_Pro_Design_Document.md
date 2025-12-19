# TacBomb Pro
## Sistema di Simulazione Tattica per Airsoft

**Design Document — Versione 1.0**  
*Documento Confidenziale — Dicembre 2024*

---

## 1. Executive Summary

TacBomb Pro è un dispositivo elettronico professionale progettato per elevare l'esperienza di gioco nelle competizioni di airsoft e paintball. Ispirato alle meccaniche di giochi tattici come Counter-Strike, il dispositivo offre scenari di gioco realistici con countdown configurabile, disinnesco tramite codice numerico o taglio cavi, e feedback audiovisivo immersivo.

Il prodotto si posiziona nel segmento premium del mercato, offrendo funzionalità avanzate non disponibili nei competitor a prezzi accessibili, con un focus sulla robustezza costruttiva e l'affidabilità in ambiente outdoor.

---

## 2. Presentazione Prodotto

### 2.1 Caratteristiche Principali

- **Display 4 cifre 7-segmenti:** visualizzazione countdown formato MM:SS con alta visibilità, funzionamento in multiplexing per ottimizzazione pin
- **Tastierino numerico 4x3:** inserimento codici disinnesco e configurazione timer
- **Sistema disinnesco a fili:** 4 connettori colorati (rosso, arancione, giallo, verde) con sequenza randomizzata
- **LED di stato RGB:** indicazione visiva dello stato dispositivo (armato, countdown, disinnescato, esploso)
- **Allarme sonoro:** buzzer piezoelettrico ad alta potenza (>85dB)
- **Indicatore batteria:** 4 LED per monitoraggio stato carica con lettura ADC
- **Memoria persistente:** salvataggio configurazioni in EEPROM
- **Clock al quarzo:** precisione temporale garantita con cristallo 16MHz
- **Debounce hardware:** tutti i pulsanti con circuito RC anti-rimbalzo

### 2.2 Modalità di Gioco

1. **Bomb Defusal Classic:** Una squadra arma la bomba, l'altra deve disinnescarla inserendo il codice a 4 cifre o tagliando la sequenza corretta di fili
2. **Wire Cut Challenge:** Modalità solo taglio fili con configurazione 1-4 fili attivi tramite DIP switch
3. **Time Attack:** Countdown accelerato con penalità per tentativi errati
4. **Training Mode:** Modalità pratica con feedback immediato e nessuna penalità

### 2.3 Interfaccia Utente

| Stato | LED RGB | Display | Audio |
|-------|---------|---------|-------|
| Standby | Verde fisso | Timer impostato | Silenzioso |
| Armato | Rosso lampeggiante | Countdown attivo | Beep ogni secondo |
| Ultimo minuto | Rosso rapido | Countdown | Beep accelerato |
| Disinnescato | Verde lampeggiante | "dEF" | Melodia successo |
| Esploso | Rosso fisso | "----" | Sirena 5 secondi |
| Batteria bassa | Arancione | Normale | Beep warning |

---

## 3. Bill of Materials — Prototipo Singolo

Prezzi riferiti ad acquisto singolo da distributori retail (Amazon, AliExpress, elettronici locali). Valori in EUR.

### Microcontrollore e Clock

| Componente | Qtà | €/unità | Totale | Note |
|------------|-----|---------|--------|------|
| ATmega328P-PU (DIP-28) | 1 | 3.50 | 3.50 | Con bootloader Arduino |
| Cristallo quarzo 16MHz | 1 | 0.30 | 0.30 | HC-49S |
| Condensatori 22pF (load caps) | 2 | 0.05 | 0.10 | Ceramici |
| Condensatore 100nF bypass | 2 | 0.05 | 0.10 | Decoupling VCC |

### Display e Visualizzazione

| Componente | Qtà | €/unità | Totale | Note |
|------------|-----|---------|--------|------|
| Display 7-seg 4 digit common cathode (0.56") | 1 | 2.50 | 2.50 | Rosso alta visibilità |
| Transistor BC547 (NPN switching) | 4 | 0.10 | 0.40 | Driver digit multiplex |
| Resistori 220Ω (segmenti) | 8 | 0.02 | 0.16 | 1/4W 5% |
| Resistori 1kΩ (base transistor) | 4 | 0.02 | 0.08 | 1/4W 5% |

### Input e Controlli

| Componente | Qtà | €/unità | Totale | Note |
|------------|-----|---------|--------|------|
| Tastierino membrana 4x3 | 1 | 1.80 | 1.80 | Adesivo IP65 |
| Pulsanti tattili 6x6mm | 4 | 0.15 | 0.60 | START/ARM/OK/CLR |
| Resistori 10kΩ (pull-up) | 12 | 0.02 | 0.24 | 1/4W 5% |
| Condensatori 100nF (debounce HW) | 8 | 0.05 | 0.40 | RC debounce |
| DIP Switch 4 posizioni | 1 | 0.40 | 0.40 | Config fili attivi |

### Sistema Disinnesco Fili

| Componente | Qtà | €/unità | Totale | Note |
|------------|-----|---------|--------|------|
| Morsettiere a vite 2 poli | 8 | 0.25 | 2.00 | Fissaggio fili |
| Cavo AWG22 colorato (kit 4 colori) | 1 | 3.00 | 3.00 | R/O/Y/G 5m cad. |

### LED e Audio

| Componente | Qtà | €/unità | Totale | Note |
|------------|-----|---------|--------|------|
| LED RGB 5mm common cathode | 1 | 0.35 | 0.35 | Stato dispositivo |
| LED 3mm verdi (batteria) | 4 | 0.08 | 0.32 | Indicatore carica |
| Buzzer piezo attivo 5V (>85dB) | 1 | 1.20 | 1.20 | TMB12A05 |
| Resistori 330Ω (LED) | 7 | 0.02 | 0.14 | 1/4W 5% |

### Alimentazione

| Componente | Qtà | €/unità | Totale | Note |
|------------|-----|---------|--------|------|
| Portabatterie 4xAA con coperchio | 1 | 1.50 | 1.50 | 6V nominale |
| Regolatore LM7805 TO-220 | 1 | 0.45 | 0.45 | 5V 1A output |
| Diodo 1N4007 (protezione inversione) | 1 | 0.10 | 0.10 | 1A 1000V |
| Condensatore elettrolitico 100µF | 2 | 0.15 | 0.30 | Filtro ingresso/uscita |
| Diodo Zener 5.1V (riferimento ADC) | 1 | 0.12 | 0.12 | 1N4733A |
| Partitore resistivo 10k+10k | 2 | 0.02 | 0.04 | Lettura Vbatt |

### Meccanica e PCB

| Componente | Qtà | €/unità | Totale | Note |
|------------|-----|---------|--------|------|
| PCB prototipo (JLCPCB 2 layer) | 1 | 8.00 | 8.00 | 5pz min + spedizione |
| Case ABS IP65 200x120x75mm | 1 | 12.00 | 12.00 | Resistente outdoor |
| Zoccolo DIP-28 (MCU) | 1 | 0.30 | 0.30 | Sostituzione MCU |
| Connettore FTDI 6 pin (programmazione) | 1 | 0.50 | 0.50 | Aggiornamento FW |
| Minuteria (viti, distanziali, cavi) | 1 | 3.00 | 3.00 | Kit assemblaggio |

### **TOTALE COMPONENTI PROTOTIPO: € 43.10** *(+ manodopera)*

---

## 4. Bill of Materials — Stock Produzione (100 unità)

Prezzi riferiti ad acquisti wholesale da LCSC, DigiKey, Mouser con MOQ ottimizzati. Include sconti volume.

| Componente | Qtà Tot | €/unità | Totale | Risparmio |
|------------|---------|---------|--------|-----------|
| ATmega328P-PU (DIP-28) bulk | 100 | 1.85 | 185.00 | **-47%** |
| Display 7-seg 4 digit (reel) | 100 | 0.95 | 95.00 | **-62%** |
| Tastierino 4x3 bulk | 100 | 0.65 | 65.00 | **-64%** |
| Componenti passivi (kit R/C) | 100 | 0.85 | 85.00 | **-70%** |
| Transistor BC547 (tape) | 400 | 0.02 | 8.00 | **-80%** |
| LED RGB + batteria LED | 500 | 0.08 | 40.00 | **-65%** |
| Buzzer piezo bulk | 100 | 0.45 | 45.00 | **-63%** |
| Alimentazione (reg, diodi, caps) | 100 | 0.80 | 80.00 | **-60%** |
| Sistema fili (morsetti + cavi) | 100 | 1.80 | 180.00 | **-64%** |
| PCB produzione (JLCPCB 100pz) | 100 | 1.20 | 120.00 | **-85%** |
| Case ABS IP65 bulk (MOQ 50) | 100 | 5.50 | 550.00 | **-54%** |
| Portabatterie + minuteria | 100 | 2.20 | 220.00 | **-51%** |

### **TOTALE COMPONENTI (100 unità): € 1,673.00**
### **COSTO UNITARIO PRODUZIONE: € 16.73** *(-61% vs prototipo)*

---

## 5. Analisi Concorrenza

Ricerca di mercato condotta su prodotti disponibili online a Dicembre 2024. Prezzi in EUR (conversione 1 USD = 0.92 EUR).

| Prodotto | Prezzo | Canale | Feature | Limiti |
|----------|--------|--------|---------|--------|
| **Game Timer Pro** (Nootropic Design) | ~€55-65 | Direct | 4 fili, keypad opzionale, hackable Arduino | Solo elettronica, no case, no batteria |
| **KMS-ANT Bomb Prop** (Makerspace.lt) | ~€85-95 | Tindie | IP55, sirena 110dB, multi-mode | No keypad numerico, batteria non inclusa |
| **Etsy Handmade Props** (KevinsCamos e altri) | €55-120 | Etsy | Variabili, alcuni con keypad | Qualità inconsistente, tempi lunghi, no IP rating |
| **CS:GO C4 Replica** (PrintFusion e altri) | €25-45 | Etsy/Ali | LED, suono, estetica gaming | Solo cosplay, no gameplay reale, DIY assembly |
| **BombONE** (Custom Made Electronics) | ~€40-50 | Tindie | 4 game modes, Arduino-based | Solo PCB, richiede assemblaggio completo |

### 5.1 Gap Analysis e Posizionamento

L'analisi della concorrenza evidenzia diverse opportunità di differenziazione:

- **Prodotto completo:** Nessun competitor offre un sistema chiavi-in-mano con case IP65, batterie, e tutte le feature integrate
- **Tastierino + Fili:** La combinazione di entrambi i metodi di disinnesco è rara nel mercato
- **Monitoraggio batteria:** Feature assente in quasi tutti i competitor
- **Qualità costruttiva:** I prodotti Etsy spesso mancano di robustezza per uso outdoor intensivo
- **Supporto italiano:** Documentazione e assistenza in lingua italiana per il mercato nazionale

---

## 6. Strategia di Pricing

### 6.1 Struttura Costi

| Voce | Prototipo | Produzione | Note |
|------|-----------|------------|------|
| Costo componenti | €43.10 | €16.73 | Volume discount |
| Assemblaggio e test (1h) | €25.00 | €12.00 | Efficienza produzione |
| Packaging e documentazione | €5.00 | €3.00 | Bulk printing |
| **COSTO TOTALE** | **€73.10** | **€31.73** | |
| Margine target (40%) | €48.73 | €21.15 | Sostenibilità business |
| **PREZZO VENDITA** | — | **€52.88** | ≈ €55 retail |

### 6.2 Posizionamento Prezzo Finale

## **Prezzo consigliato: €59.90** (IVA inclusa)

Questo posizionamento si basa su:

- **Competitività:** Sotto il KMS-ANT (€85-95) che è il competitor più simile per completezza
- **Valore percepito:** Sopra i prodotti Etsy base (€55-65) che richiedono assemblaggio o mancano di feature
- **Margine sano:** 47% lordo che consente promozioni e margine distributori
- **Psicologia prezzo:** Sotto la soglia psicologica dei €60, percepito come "sotto i 60 euro"

### 6.3 Opzioni Bundle

| Bundle | Prezzo | Contenuto |
|--------|--------|-----------|
| **TacBomb Pro Base** | €59.90 | Dispositivo completo senza batterie |
| **TacBomb Pro Kit** | €69.90 | Include batterie AA ricaricabili + caricatore |
| **TacBomb Pro Team (2x)** | €109.90 | Due unità per scenario attacco/difesa simultaneo |

---

## 7. Use Case — Scenario di Gioco

### "Operazione Thunderstrike" — Modalità Search & Destroy

**Ambientazione:** Campo airsoft "Zona Rossa", area industriale dismessa. Due squadre da 8 giocatori.

#### Setup (5 minuti prima del match)

1. Il game master configura il TacBomb Pro: timer 10:00, modalità "Code + 2 Wires"
2. Il dispositivo genera automaticamente: codice disinnesco **"7294"**, sequenza fili **"Rosso → Verde"**
3. LED stato: VERDE fisso (standby), display mostra "10:00"
4. Il game master consegna il codice alla squadra Difesa in busta sigillata
5. I 4 switch vengono configurati: ON-OFF-ON-OFF (solo fili 1 e 3 attivi)

#### Fase 1 — Attivazione (Squadra Attacco)

1. La squadra Attacco raggiunge il sito di piazzamento sotto fuoco nemico
2. Il bomb carrier preme il tasto **"ARM"** e tiene premuto per 3 secondi
3. Feedback: 3 beep crescenti, LED passa a **ROSSO lampeggiante**
4. Il countdown **10:00** inizia a scorrere
5. La squadra Attacco deve ora difendere la posizione

#### Fase 2 — Countdown (Tensione crescente)

| Tempo rimanente | Comportamento |
|-----------------|---------------|
| 10:00 - 05:00 | Beep ogni 10 secondi, LED rosso lento |
| 05:00 - 01:00 | Beep ogni 5 secondi, LED rosso medio |
| 01:00 - 00:30 | Beep ogni secondo, LED rosso rapido |
| 00:30 - 00:00 | Beep continuo accelerato, LED rosso stroboscopico |

#### Fase 3a — Disinnesco Riuscito (Squadra Difesa)

1. Un operatore della Difesa raggiunge il dispositivo
2. **Opzione A - Codice:** Inserisce "7294" sul tastierino → conferma con OK
3. **Opzione B - Fili:** Taglia prima il filo ROSSO, poi il VERDE (in quest'ordine!)
4. Feedback successo: melodia vittoria, LED **VERDE lampeggiante**, display "dEF"
5. Il countdown si ferma, la squadra Difesa vince

#### Fase 3b — Esplosione (Squadra Attacco vince)

1. Il countdown raggiunge 00:00
2. Feedback: sirena 5 secondi (85dB), LED **ROSSO fisso**, display "----"
3. La squadra Attacco vince la partita

#### Fase 3c — Errore Disinnesco

1. Codice errato inserito OPPURE filo sbagliato tagliato
2. Feedback: buzzer errore (3 beep bassi)
3. **Penalità:** -30 secondi dal countdown (accelerazione)
4. Dopo 3 errori: esplosione immediata

---

## 8. Specifiche Tecniche

### 8.1 Caratteristiche Elettriche

| Parametro | Valore | Note |
|-----------|--------|------|
| Tensione alimentazione | 4.5V - 6V DC | 4x AA o alimentatore esterno |
| Consumo standby | ~15mA | Display spento, solo MCU |
| Consumo attivo | ~80mA | Display + LED + buzzer intermittente |
| Consumo picco | ~150mA | Tutti i segmenti + buzzer continuo |
| Autonomia tipica | 8-12 ore | Con batterie AA alcaline 2500mAh |

### 8.2 Dimensioni e Peso

| Parametro | Valore |
|-----------|--------|
| Dimensioni case | 200 x 120 x 75 mm |
| Peso (senza batterie) | ~350g |
| Peso (con batterie) | ~450g |
| Grado protezione | IP65 |

### 8.3 Pin Assignment ATmega328P

| Pin | Funzione | Note |
|-----|----------|------|
| PD0-PD6 | Segmenti a-g display | 7 segmenti + dp |
| PD7 | Buzzer | PWM per toni |
| PB0-PB3 | Digit select (via transistor) | Multiplexing 4 cifre |
| PB4-PB5 | LED RGB (R, G) | B non usato |
| PC0-PC3 | Lettura fili | Pull-up interno |
| PC4 | Lettura batteria (ADC) | Partitore resistivo |
| PC5 | LED batteria (shift register) | 74HC595 opzionale |
| A6-A7 | Colonne keypad | Matrice 4x3 |

---

## 9. Roadmap Sviluppo

### Fase 1 — Prototipo (4-6 settimane)
- [ ] Progettazione schematico in KiCad
- [ ] Sviluppo firmware base Arduino
- [ ] Test su breadboard
- [ ] Design PCB v1.0
- [ ] Ordine componenti prototipo
- [ ] Assemblaggio e debug

### Fase 2 — Validazione (2-3 settimane)
- [ ] Test sul campo con gruppo airsoft locale
- [ ] Raccolta feedback utenti
- [ ] Iterazione design (PCB v1.1 se necessario)
- [ ] Ottimizzazione firmware
- [ ] Test autonomia batteria

### Fase 3 — Produzione Pilota (3-4 settimane)
- [ ] Ordine componenti stock (50-100 unità)
- [ ] Setup linea assemblaggio
- [ ] Quality control procedures
- [ ] Packaging design
- [ ] Documentazione utente

### Fase 4 — Lancio (ongoing)
- [ ] Presenza online (sito web, social)
- [ ] Partnership con campi airsoft
- [ ] Distribuzione Etsy/Amazon
- [ ] Supporto post-vendita

---

## 10. Rischi e Mitigazioni

| Rischio | Probabilità | Impatto | Mitigazione |
|---------|-------------|---------|-------------|
| Shortage componenti (MCU) | Media | Alto | Stock buffer, alternative pin-compatible |
| Competitor copia design | Media | Medio | Iterazione feature, community building |
| Problemi qualità case | Bassa | Alto | Campioni pre-produzione, QC rigoroso |
| Normative (aspetto realistico) | Bassa | Alto | Design chiaramente "gaming", disclaimer |
| Costi spedizione elevati | Media | Medio | Produzione locale EU, consolidamento ordini |

---

## 11. Conclusioni

TacBomb Pro rappresenta un'opportunità di mercato concreta nel segmento degli accessori per airsoft/paintball. L'analisi mostra:

1. **Gap di mercato:** Nessun prodotto offre la combinazione di feature, qualità costruttiva e prezzo che proponiamo
2. **Margini sostenibili:** 47% lordo a produzione che consente crescita e reinvestimento
3. **Scalabilità:** Passaggio da €43 a €17 di costo componenti con volumi modesti (100 unità)
4. **Barriere all'ingresso basse:** Tecnologia accessibile, investimento iniziale contenuto (~€2,000 per primo lotto)

Il prezzo di **€59.90** posiziona il prodotto come scelta premium accessibile, differenziandosi sia dai prodotti DIY economici che dalle soluzioni professionali costose.

---

*Documento preparato per valutazione interna. Tutti i prezzi sono stime basate su ricerche di mercato a Dicembre 2024 e soggetti a variazione.*
