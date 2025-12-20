# 💣 TACBOMB - Countdown Timer

**Sistema di countdown Arduino per simulazioni softair** con tastierino 4×4, display 7-segmenti e LED di stato.

---

## ⚠️ DISCLAIMER - IMPORTANTE

**QUESTO DISPOSITIVO È ESCLUSIVAMENTE A SCOPO DIDATTICO, LUDICO E RICREATIVO.**

- ✅ **Uso consentito**: Scenari di gioco softair/airsoft, escape room, simulazioni didattiche, progetti educativi embedded
- ❌ **Uso vietato**: Qualsiasi utilizzo che possa causare allarme pubblico, panico o simulare dispositivi pericolosi in contesti non autorizzati
- ⚖️ **Responsabilità legale**: L'utilizzatore è l'unico responsabile dell'uso del dispositivo nel rispetto delle leggi vigenti (art. 658 C.P. Procurato allarme presso l'Autorità, D.Lgs. 493/96 Segnaletica di sicurezza)
- 🚨 **Contesto controllato**: Utilizzare solo in ambienti privati o eventi autorizzati con consenso esplicito di tutti i partecipanti
- 📢 **Informativa obbligatoria**: Informare preventivamente organizzatori e partecipanti della natura simulata del dispositivo

**L'autore declina ogni responsabilità per usi impropri, illeciti o che violino normative locali.** L'uso del dispositivo in spazi pubblici senza autorizzazione può configurare reati penali. Verificare sempre le normative locali prima dell'utilizzo.

---

## 🎯 Descrizione

TACBOMB è un timer programmabile con codice di disinnesco casuale, pensato per scenari di gioco softair. Il sistema genera un codice segreto a 4 cifre ad ogni avvio e offre feedback visivi e sonori completi durante tutte le fasi operative.

---

## 🚀 Sequenza di Boot

All'accensione il sistema esegue una sequenza di inizializzazione con feedback visivi e sonori:

### Feedback Visivi
| LED Status | Stato | Descrizione |
|------------|-------|-------------|
| 🔴 **ROSSO** | START | Sistema in avvio |
| 🟠 **ARANCIONE lampeggiante** | INIT | Lettura configurazione e inizializzazione hardware |
| 🟢 **VERDE** | READY | Sistema pronto, entrando in modalità SETUP |

### Feedback Sonori
- **Beep di avvio**: Tono breve all'accensione
- **Test diagnostico**: Sequenza di 3 toni (LOW-MEDIUM-HIGH) durante l'inizializzazione
- **Beep di successo**: Sistema pronto

### Log Seriale
Durante il boot, il sistema mostra:
```
╔══════════════════════════════════════════════════════╗
║  SECRET DISARM CODE: XXXX                            ║
╚══════════════════════════════════════════════════════╝
```
Il **codice segreto** è generato casualmente ad ogni avvio (4 cifre casuali 0-9).

---

## 🎮 Modalità Operative

### 📋 SETUP_INIT (Stato Iniziale)

**Display**: `10:00` (timer predefinito)  
**LED Status**: 🔴 SPENTO  
**Colon**: FISSO

#### Comandi Disponibili:
| Tasto | Funzione |
|-------|----------|
| **A** | Entra in modalità EDIT |
| **#** | Toggle visibilità codice segreto sul display |

#### 🔐 Visualizzazione Codice Segreto

Premendo **#** in questa fase, il display alterna tra:
- `10:00` → Tempo iniziale predefinito
- `XX:XX` → **Codice segreto a 4 cifre** (es. `73:94` per codice `7394`)

Il formato sul display è `MM:SS` dove:
- **MM** = prime 2 cifre del codice (es. `73`)
- **SS** = ultime 2 cifre del codice (es. `94`)

**Esempi:**
- Codice `1234` → Display mostra `12:34`
- Codice `0789` → Display mostra `07:89`
- Codice `5003` → Display mostra `50:03`

> **⚠️ IMPORTANTE**: Il toggle del codice segreto (`#`) funziona **SOLO** in questa fase iniziale (SETUP_INIT). Dopo essere entrati in modalità EDIT premendo `A`, il toggle viene **disabilitato permanentemente** fino al prossimo riavvio del dispositivo. Questo forza il giocatore a **memorizzare il codice** prima di iniziare la configurazione del timer.

---

### ✏️ SETUP_EDIT (Modifica Timer)

**Display**: Timer modificabile con cursore lampeggiante  
**LED Status**: 🟠 ARANCIONE lampeggiante (500ms)  
**Colon**: FISSO

Il cursore lampeggia sulla cifra selezionata (accensione/spegnimento ogni 500ms).

#### Comandi Disponibili:
| Tasto | Funzione |
|-------|----------|
| **2** | Incrementa cifra selezionata (+1, wrap 9→0) |
| **8** | Decrementa cifra selezionata (-1, wrap 0→9) |
| **4** | Sposta cursore a SINISTRA (wrap: S0→M1) |
| **6** | Sposta cursore a DESTRA (wrap: M1→S0) |
| **C** | Conferma timer → passa a ARMED |
| **D** | Cancella modifiche → torna a SETUP_INIT (reset 10:00) |

**Posizioni cursore**: M1 (minuti decine) → M0 (minuti unità) → S1 (secondi decine) → S0 (secondi unità)

**Feedback sonoro**: Beep breve ad ogni pressione tasto.

---

### 🟢 ARMED (Armato - Pronto all'Innesco)

**Display**: Timer impostato (es. `05:30`)  
**LED Status**: 🟢 VERDE fisso  
**Colon**: FISSO

#### Comandi Disponibili:
| Tasto | Funzione |
|-------|----------|
| **\*** | START countdown → passa a COUNTDOWN_RUNNING |
| **D** | Cancella → torna a SETUP_INIT (reset) |

---

### ⏱️ COUNTDOWN_RUNNING (Countdown Attivo)

**Display**: Timer in decremento (es. `04:59` → `04:58`...)  
**LED Status**: 🟠 ARANCIONE lampeggiante (500ms)  
**Colon**: LAMPEGGIANTE (1 lampeggio/secondo - sincrono col countdown)

Il timer decrementa automaticamente ogni secondo. Il countdown continua anche durante la modalità DISARM.

#### Comandi Disponibili:
| Tasto | Funzione |
|-------|----------|
| **#** | Entra in modalità DISARM (disinnesco) |

**⚠️ ATTENZIONE**: Se il timer raggiunge `00:00`, il sistema esplode automaticamente (anche durante il disinnesco).

---

### 🔓 DISARM_MODE (Modalità Disinnesco)

**Display**: **Countdown continua visibile** (es. `03:45`)  
**LED Status**: 🟡 GIALLO fisso  
**Colon**: FISSO

In questa modalità il **display mostra il tempo rimanente** (non il codice inserito), mantenendo visibile la pressione temporale.

#### Input Codice:
| Tasto | Funzione |
|-------|----------|
| **0-9** | Inserisci cifra (audio feedback: beep) |
| **C** | Conferma codice (verifica contro SECRET_CODE) |
| **D** | Cancella codice inserito → reset a `----` |
| **\*** | Annulla disinnesco → torna a COUNTDOWN_RUNNING |

**Feedback**:
- **Visivo**: Display mostra countdown in tempo reale (non il codice)
- **Audio**: Beep breve ad ogni cifra inserita
- **Seriale**: Log mostra codice parziale (es. `1--- (1/4)`, `12-- (2/4)`)

**Verifica Codice**:
- ✅ **Codice CORRETTO** → Display `88:88`, LED 🟢 VERDE fisso, beep successo, sistema si resetta
- ❌ **Codice ERRATO** → Esplosione immediata
- ⏱️ **Tempo SCADUTO** → Esplosione (anche con codice incompleto)

> **Nota**: Puoi cancellare e reinserire il codice più volte finché c'è tempo disponibile.

---

### 💥 EXPLODED (Esplosione)

**Display**: `00:00`  
**LED Status**: 🔴 ROSSO lampeggiante veloce (200ms)  
**Colon**: SPENTO

**Allarme sonoro**: Tono continuo di errore per **60 secondi**.

Dopo l'esplosione il sistema rimane in questo stato. Riavviare Arduino per un nuovo gioco.

---

## 🔊 Riepilogo Feedback Sonori

| Evento | Suono |
|--------|-------|
| Avvio sistema | Beep breve startup |
| Test diagnostico boot | 3 toni (LOW→MEDIUM→HIGH) |
| Pressione tasto (qualsiasi) | Beep breve |
| Ingresso DISARM | Beep successo |
| Inserimento cifra disinnesco | Beep startup |
| Disinnesco riuscito | Beep successo |
| Esplosione | Allarme continuo (60s) |

---

## 🎨 Riepilogo LED Status

| Colore | Stato | Modalità |
|--------|-------|----------|
| 🔴 ROSSO fisso | Boot | Avvio sistema |
| 🟠 ARANCIONE blink | Attivo | SETUP_EDIT, COUNTDOWN_RUNNING |
| 🟢 VERDE fisso | Pronto/Successo | Boot ready, ARMED, Disinnesco OK |
| 🟡 GIALLO fisso | Disinnesco | DISARM_MODE |
| 🔴 ROSSO blink veloce | Esplosione | EXPLODED |
| ⚫ SPENTO | Idle | SETUP_INIT |

---

## 📊 Monitoraggio Batteria

Il sistema include 4 LED per il monitoraggio della carica (simulata con potenziometro in Wokwi):

| LED | Colore | Tensione | Stato |
|-----|--------|----------|-------|
| D2 | 🟢 VERDE | ≥3.75V | FULL |
| D3 | 🟡 GIALLO | 2.50-3.75V | GOOD |
| D4 | 🟠 ARANCIONE | 1.25-2.50V | LOW |
| D5 | 🔴 ROSSO | <1.25V | CRITICAL |

Aggiornamento ogni 2 secondi (non invasivo durante le operazioni).

---

## 🔧 Hardware Setup (Wokwi)

### Componenti:
- **Arduino Uno** (ATmega328P)
- **Tastierino 4×4** membrane (layout `123A456B789C*0#D`)
- **4× Display 7-segmenti** common cathode + **74HC595** shift register
- **NeoPixel WS2812B** (LED status)
- **Buzzer** piezo
- **3× LED** boot (rosso/arancione/verde) [non più usati - pin riutilizzati per display]
- **4× LED** batteria (verde/giallo/arancione/rosso)
- **Potenziometro** (simulatore batteria)

### Connessioni Principali:
- **Keypad**: Rows → D4-D7, Cols → A1-A4
- **Display**: Data→D2, Clock→D3, Latch→A0, Digits→D10-D12+A5
- **Status LED**: D8 (NeoPixel WS2812B)
- **Buzzer**: D9
- **Battery LEDs**: D2-D5 (FULL/GOOD/LOW/CRITICAL)
- **Battery ADC**: A0 (potenziometro 0-5V)

---

## 💾 Utilizzo Seriale (Debug)

Collegare a **115200 baud** per vedere:
- Log di boot con codice segreto
- Stato corrente della macchina
- Pressioni tasti con debug dettagliato
- Countdown updates ogni secondo
- Input codice disinnesco

---

## 📝 Esempio Flusso Completo

1. **Boot** → 🔴→🟠→🟢 → Display `10:00` (SETUP_INIT)
2. Premi **#** → Display mostra codice segreto `7394` (esempio)
3. Premi **#** di nuovo → Torna a `10:00`
4. Premi **A** → 🟠 blink, cursore su M1 (SETUP_EDIT)
5. Premi **2** 3 volte → `03:00` (imposta 3 minuti)
6. Premi **6** 2 volte → Cursore su S1
7. Premi **2** 3 volte → `03:30` (imposta 30 secondi)
8. Premi **C** → 🟢 fisso (ARMED)
9. Premi **\*** → 🟠 blink, countdown inizia `03:29`... (COUNTDOWN_RUNNING)
10. Durante countdown premi **#** → 🟡 fisso (DISARM_MODE)
11. Inserisci codice: **7** → beep, **3** → beep, **9** → beep, **4** → beep
12. Premi **C** → 
    - ✅ Se corretto: `88:88`, 🟢 fisso, beep successo → RESET
    - ❌ Se errato: `00:00`, 🔴 blink, allarme 60s → EXPLODED

---

## 🎯 Strategia di Gioco

- **Memorizza il codice** all'avvio (appare solo nei primi secondi di boot nel log seriale, oppure usa `#` in SETUP_INIT)
- **Imposta timer strategico** (es. 5-10 minuti per scenari dinamici)
- **Countdown visibile** durante disinnesco = massima tensione!
- **Tentativi multipli possibili** (cancella con `D` e riprova) ma il tempo scorre!
- **Nessun limite di tentativi** → corsa contro il tempo!

---

## 🔒 Note di Sicurezza

- ⚠️ Codice segreto **cambia ad ogni riavvio** (generazione casuale)
- ⚠️ Toggle `#` del codice **disabilitato dopo EDIT** (forza memorizzazione)
- ⚠️ Countdown **continua durante disinnesco** (no pause)
- ⚠️ Esplosione **immediata** se timer scade o codice errato

---

## 📦 Build & Flash

```bash
# Build firmware
pio run

# Upload su Arduino
pio run -t upload

# Monitor seriale
pio device monitor
```

### Simulazione Wokwi:
1. Build: `pio run`
2. Cmd+Shift+P → "Wokwi: Start Simulator"
3. Test completo senza hardware fisico!

---

## 📈 Utilizzo Memoria

- **RAM**: ~21.7% (444/2048 bytes)
- **Flash**: ~60.4% (19474/32256 bytes)

Ottimizzato per ATmega328P con pattern embedded-safe (no heap allocation, PROGMEM strings).

---

## 🏆 Crediti

**Hardware**: Arduino Uno + componenti standard  
**Framework**: PlatformIO + Arduino  
**Simulazione**: Wokwi VS Code Extension  

---

**Versione**: 1.0.0 - Disarm Mode con Random Secret Code  
**Licenza**: MIT  
**Autore**: Antonio Cittadino

---

## ⚖️ Note Legali

Questo progetto è rilasciato esclusivamente per scopi educativi e ricreativi. L'utilizzo del dispositivo è soggetto alle seguenti condizioni:

1. **Uso responsabile**: Il dispositivo simula un timer con effetti sonori e visivi che potrebbero essere scambiati per dispositivi pericolosi se utilizzati in contesti inappropriati
2. **Normativa italiana applicabile**:
   - **Art. 658 C.P.** - Procurato allarme presso l'Autorità (reclusione fino a 6 mesi o multa fino a €516)
   - **D.Lgs. 81/2008** - Sicurezza nei luoghi di lavoro (uso in contesti lavorativi)
   - **Normative locali** su eventi pubblici e simulazioni (verificare con le autorità competenti)
3. **Consenso informato**: Richiedere sempre il consenso scritto di organizzatori e partecipanti prima dell'uso
4. **Etichettatura**: Etichettare chiaramente il dispositivo come "SIMULATORE - NON PERICOLOSO - SOLO USO LUDICO"
5. **Limiti di età**: Uso consigliato sotto supervisione di adulti responsabili

**L'autore non si assume alcuna responsabilità per danni diretti, indiretti o conseguenti derivanti dall'uso improprio del dispositivo.**

