# Arduino PlatformIO Template 🚀# Arduino PlatformIO Template 🚀# Arduino LED & Button System 🚦



Ultra-minimal Arduino project template with PlatformIO and Wokwi simulation support.



[![PlatformIO](https://img.shields.io/badge/PlatformIO-orange.svg)](https://platformio.org/)Minimal Arduino project template with PlatformIO and Wokwi simulation support.Sistema Arduino avanzato con boot sequence visuale, gestione configurazione JSON e controllo LED tramite pulsante con debouncing hardware/software.

[![Framework](https://img.shields.io/badge/Framework-Arduino-blue.svg)](https://www.arduino.cc/)

[![Board](https://img.shields.io/badge/Board-Arduino%20Uno-green.svg)](https://docs.arduino.cc/hardware/uno-rev3)



## ✨ Features[![PlatformIO](https://img.shields.io/badge/PlatformIO-orange.svg)](https://platformio.org/)[![PlatformIO](https://img.shields.io/badge/PlatformIO-Arduino-orange.svg)](https://platformio.org/)



- 🎯 **PlatformIO** - Modern build system for embedded development[![Framework](https://img.shields.io/badge/Framework-Arduino-blue.svg)](https://www.arduino.cc/)[![Framework](https://img.shields.io/badge/Framework-Arduino-blue.svg)](https://www.arduino.cc/)

- 🔧 **Modular Structure** - Custom libraries in `lib/` folder

- 🖥️ **Wokwi Simulation** - Hardware-less testing in VS Code[![Board](https://img.shields.io/badge/Board-Arduino%20Uno-green.svg)](https://docs.arduino.cc/hardware/uno-rev3)[![Board](https://img.shields.io/badge/Board-Arduino%20Uno-green.svg)](https://store.arduino.cc/products/arduino-uno-rev3)

- 📊 **Low RAM Usage** - Optimized for ATmega328P (58% RAM, 21.5% Flash)

- ⚡ **Boot Sequence** - Visual LED feedback with 3-phase initialization

- 🔘 **Interrupt-driven** - Button handling with debouncing

## ✨ Features## 📋 Indice

## 🚀 Quick Start



### Prerequisites

- 🎯 **PlatformIO** - Modern build system for embedded development- [Caratteristiche](#-caratteristiche)

```bash

# Install PlatformIO- 🔧 **Modular Structure** - Organized libs, src, and test folders- [Requisiti Hardware](#-requisiti-hardware)

pip install platformio

- 🧪 **Unit Testing** - Unity test framework included- [Schema Circuitale](#-schema-circuitale)

# Or use VS Code PlatformIO IDE extension

```- 🖥️ **Wokwi Simulation** - Hardware-less testing in VS Code- [Installazione](#-installazione)



### Build & Upload- 📊 **Low RAM Usage** - Optimized for ATmega328P (58% RAM, 21.5% Flash)- [Configurazione](#-configurazione)



```bash- ⚡ **Boot Sequence** - Visual LED feedback system- [Utilizzo](#-utilizzo)

git clone <your-repo-url>

cd softair- 🔘 **Interrupt-driven** - Button handling with debouncing- [Architettura](#-architettura)



# Build firmware- [Testing](#-testing)

pio run

## 🚀 Quick Start- [API Reference](#-api-reference)

# Upload to Arduino

pio run --target upload



# Monitor serial output### Prerequisites## ✨ Caratteristiche

pio device monitor

```



### Simulate with Wokwi```bash### 🚀 Boot Sequence Visuale



1. Install [Wokwi VS Code extension](https://marketplace.visualstudio.com/items?itemName=wokwi.wokwi-vscode)# Install PlatformIO- **LED Rosso**: Sistema in avvio

2. Build firmware: `pio run`

3. Command Palette (`Ctrl+Shift+P`) → **"Wokwi: Start Simulator"**pip install platformio- **LED Arancione (lampeggiante)**: Lettura configurazione e inizializzazione



Watch the boot sequence:- **LED Verde**: Sistema pronto all'uso

- 🔴 Red LED → System starting

- 🟠 Orange LED (blinking) → Initializing# Or install VS Code PlatformIO IDE extension

- 🟢 Green LED → Ready!

```### ⚙️ Gestione Configurazione

## 📁 Project Structure

- Caricamento configurazione da JSON

```

softair/### Clone & Build- Validazione parametri

├── src/

│   └── main.cpp              # Main application (356 lines)- Configurazione di default fallback

├── lib/

│   └── Utilities/            # Custom libraries```bash- Supporto per configurazioni embedded o da storage esterno

│       ├── ConfigLoader.*    # System configuration

│       └── SystemInitializer.* # Boot sequence with LED controlgit clone <your-repo-url>

├── platformio.ini            # PlatformIO configuration

├── diagram.json              # Wokwi circuit (3 LEDs + button)cd softair### 🎛️ Controllo LED

├── wokwi.toml               # Wokwi simulator config

└── README.md                 # This file- Pulsante con debouncing hardware (resistor pull-up interno)



Generated (ignored by git):# Build firmware- Debouncing software configurabile (default 50ms)

├── .pio/                     # Build output

└── .vscode/                  # IDE settingspio run- Gestione interrupt per risposta immediata

```

- Toggle LED su pressione pulsante

**Total code:** ~1450 lines

# Upload to Arduino

## ⚙️ Configuration

pio run --target upload### 📊 Monitoring & Debug

### PlatformIO (`platformio.ini`)

- Output seriale dettagliato con box UTF-8

```ini

[env:uno]# Open serial monitor- Statistiche di sistema (uptime, RAM libera, contatore pressioni)

platform = atmelavr

board = unopio device monitor- Comandi seriali interattivi

framework = arduino

monitor_speed = 115200```- Modalità DEBUG con informazioni extra

build_flags = -D DEBUG=1 -Wall -Wextra

```



### Hardware Pins### Simulate with Wokwi (Optional)## 🔧 Requisiti Hardware



| Component | Pin | Description |

|-----------|-----|-------------|

| Button | 2 | Digital input with interrupt |1. Install [Wokwi VS Code extension](https://marketplace.visualstudio.com/items?itemName=wokwi.wokwi-vscode)### Componenti Necessari

| LED (user) | 13 | Built-in LED (toggle) |

| Red LED | 10 | Boot: START |2. Build firmware: `pio run`

| Orange LED | 11 | Boot: INIT |

| Green LED | 12 | Boot: READY |3. Open Command Palette (`Ctrl+Shift+P`)| Componente | Quantità | Note |



## 🔄 Development Workflow4. Run: **"Wokwi: Start Simulator"**|------------|----------|------|



```bash| Arduino Uno | 1 | O board compatibile |

# 1. Edit code

vim src/main.cpp## 📁 Project Structure| LED Rosso | 1 | + resistenza 220Ω |



# 2. Build| LED Arancione | 1 | + resistenza 220Ω |

pio run

```| LED Verde | 1 | + resistenza 220Ω |

# 3a. Test on hardware

pio run --target uploadsoftair/| LED Standard | 1 | Built-in pin 13 (opzionale) |

pio device monitor

├── src/| Pulsante | 1 | Normalmente aperto |

# 3b. Test in Wokwi (VS Code)

# Command Palette → "Wokwi: Start Simulator"│   └── main.cpp              # Main application code| Resistenze 220Ω | 3 | Per LED esterni |

```

├── lib/| Breadboard | 1 | Per prototipazione |

## 🧩 Custom Libraries

│   └── Utilities/            # Custom libraries| Cavi jumper | ~10 | |

### SystemInitializer

│       ├── ConfigLoader.*    # Configuration management

Manages 4-phase boot sequence with LED indicators:

│       └── SystemInitializer.* # Boot sequence & LED control## 📐 Schema Circuitale

```cpp

#include <SystemInitializer.h>├── test/



SystemConfig config = ConfigLoader::getDefaultConfig();│   └── test_embedded/        # Unit tests (Unity framework)```

SystemInitializer init(config);

├── include/                  # Shared headers (empty by default)                    Arduino Uno

if (init.initialize()) {

    // Phase 1: Red LED (START) - 500ms├── platformio.ini            # PlatformIO configuration                  ┌──────────────┐

    // Phase 2: Orange LED blinking (INIT) - 5x blinks

    // Phase 3: Initializing - 500ms├── diagram.json              # Wokwi circuit diagram                  │              │

    // Phase 4: Green LED (READY) - stays on

}└── wokwi.toml               # Wokwi simulator config    Button ───────┤ D2      D10 ├──── [220Ω] ──── LED Rosso ──── GND

```

                  │              │

### ConfigLoader

Build artifacts (ignored by git):                  │         D11 ├──── [220Ω] ──── LED Arancione ─ GND

Provides default configuration (JSON parsing disabled to save RAM):

├── .pio/                     # Build cache and output                  │              │

```cpp

#include <ConfigLoader.h>└── compile_commands.json     # For IntelliSense                  │         D12 ├──── [220Ω] ──── LED Verde ───── GND



SystemConfig config = ConfigLoader::getDefaultConfig();```                  │              │

// Returns:

//   deviceName: "Arduino-Softair"                  │         D13 ├──── LED Built-in ──────────── GND

//   version: "1.0.0"

//   baudRate: 115200## ⚙️ Configuration                  │              │

//   LED pins: 10, 11, 12

//   blinkInterval: 200ms                  │         GND ├──── Common Ground

```

### PlatformIO (`platformio.ini`)                  │              │

## 📊 Memory Usage

                  │         5V  │

Optimized for Arduino Uno (ATmega328P: 2KB RAM, 32KB Flash):

```ini                  └──────────────┘

```

RAM:   [======    ]  58.0% (1188 / 2048 bytes)[env:uno]```

Flash: [==        ]  21.5% (6924 / 32256 bytes)

```platform = atmelavr



**Optimization techniques used:**board = uno### Connessioni Dettagliate

- `F()` macro for strings → stores in Flash (PROGMEM)

- JSON parsing disabled → saves ~1KB RAMframework = arduino

- Minimal serial output → reduces string storage

- Efficient boot sequence → compact state machinemonitor_speed = 115200#### LED Boot Sequence (configurabili)



## 🖥️ Wokwi Simulationtest_framework = unity- **Pin 10** → LED Rosso (Anodo) → Resistenza 220Ω → GND (Catodo)



The project includes a complete circuit (`diagram.json`):build_flags = -D DEBUG=1 -Wall -Wextra- **Pin 11** → LED Arancione (Anodo) → Resistenza 220Ω → GND (Catodo)



**Components:**```- **Pin 12** → LED Verde (Anodo) → Resistenza 220Ω → GND (Catodo)

- Arduino Uno

- 3× LEDs (Red, Orange, Green) with 220Ω resistors

- Push button (for future features)

### Hardware Pins#### LED Controllabile

**How it works:**

1. `pio run` generates `.pio/build/uno/firmware.hex`- **Pin 13** → LED Built-in (già con resistenza interna)

2. `wokwi.toml` tells Wokwi to load this firmware

3. Wokwi simulates the hardware without a physical boardDefault pin configuration in `src/main.cpp`:



**Serial Monitor:** Output appears in VS Code terminal (Wokwi Terminal tab)#### Pulsante



## 🔧 Customization| Component | Pin | Description |- **Pin 2** → Un lato del pulsante



### Add a New Library|-----------|-----|-------------|- **GND** → Altro lato del pulsante



```bash| Button | 2 | Digital input with interrupt |- **Nota**: Usa `INPUT_PULLUP` - non serve resistenza esterna!

mkdir -p lib/MyLibrary

touch lib/MyLibrary/MyLibrary.h| LED (user) | 13 | Built-in LED (toggle) |

touch lib/MyLibrary/MyLibrary.cpp

| Red LED | 10 | Boot sequence: START |## 🚀 Installazione

# PlatformIO auto-discovers it

# Include with: #include <MyLibrary.h>| Orange LED | 11 | Boot sequence: INIT |

```

| Green LED | 12 | Boot sequence: READY |### 1. Prerequisiti

### Modify Boot Sequence

```bash

Edit `lib/Utilities/SystemInitializer.cpp`:

## 🔄 Development Workflow# Installa PlatformIO

```cpp

bool SystemInitializer::initialize() {pip install platformio

    setRedLed();

    delay(500);  // ← Adjust timing### Standard Development

    

    setOrangeLedBlinking();# O usa PlatformIO IDE (VS Code extension)

    // ← Add your initialization here

    ```bash```

    setGreenLed();

    return true;# 1. Edit code

}

```vim src/main.cpp### 2. Clone del Repository



### Add Serial Commands```bash



Edit `src/main.cpp` in `handleSerialCommands()`:# 2. Buildgit clone <repository-url>



```cpppio runcd softair

case 'x':  // Add new command

    Serial.println(F("Custom command!"));```

    break;

```# 3. Upload to hardware



## 🎯 Use Casespio run --target upload### 3. Build & Upload



This template is perfect for:pio device monitor```bash



- ✅ Arduino projects with visual feedback (LEDs)```# Compila il progetto

- ✅ Learning PlatformIO and modular Arduino development

- ✅ Prototyping with Wokwi before building hardwarepio run

- ✅ Projects requiring low RAM usage optimization

- ✅ IoT devices with boot status indicators### With Wokwi Simulation

- ✅ Educational Arduino projects with clean structure

# Upload su Arduino

## 📚 Resources

```bashpio run --target upload

- [PlatformIO Docs](https://docs.platformio.org/)

- [Arduino Uno Reference](https://docs.arduino.cc/hardware/uno-rev3)# 1. Edit code

- [Wokwi Simulator](https://docs.wokwi.com/)

- [ATmega328P Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)vim src/main.cpp# Monitor seriale



## 📄 Licensepio device monitor



MIT License - Free to use for any project!# 2. Build firmware```



## 🤝 Contributingpio run



This is a template repository:### 4. Verifica Installazione



1. Click **"Use this template"** on GitHub# 3. Start Wokwi simulator (VS Code)Dopo l'upload dovresti vedere:

2. Clone your new repo

3. Start coding!# Command Palette → "Wokwi: Start Simulator"1. ✅ LED rosso si accende



---```2. ✅ LED arancione lampeggia



**Ready to build amazing Arduino projects! 🚀**3. ✅ LED verde si accende (sistema pronto)



**Memory optimized • Wokwi ready • Production tested**### Run Tests4. ✅ Output seriale con banner




```bash### 5. Simulazione Wokwi (Opzionale)

# Run all unit tests

pio testTesta il firmware senza hardware usando Wokwi:



# Run specific test```bash

pio test -f test_config_loader# 1. Build firmware

```pio run



## 🧩 Custom Libraries# 2. Apri Wokwi in VS Code

# Command Palette → "Wokwi: Start Simulator"

### ConfigLoader```



Manages system configuration with default values:📖 **Vedi**: [`docs/WOKWI_WORKFLOW.md`](docs/WOKWI_WORKFLOW.md) per il workflow completo



```cpp## ⚙️ Configurazione

SystemConfig config = ConfigLoader::getDefaultConfig();

// Returns: Arduino-Softair, v1.0.0, 115200 baud, LED pins 10/11/12### File di Configurazione JSON

```

Il sistema può caricare configurazioni da JSON. Esempio di configurazione completa:

### SystemInitializer

```json

Handles boot sequence with LED feedback:{

    "version": "1.0.0",

```cpp    "deviceName": "Arduino-Softair-001",

SystemInitializer init(config);    "networkSSID": "MyWiFiNetwork",

if (init.initialize()) {    "baudRate": 115200,

    // Phase 1: Red LED (START)    "leds": {

    // Phase 2: Orange LED blinking (INIT)        "red": 10,

    // Phase 3: Green LED (READY)        "orange": 11,

}        "green": 12

```    },

    "blinkIntervalMs": 200

## 📊 Memory Usage}

```

Optimized for Arduino Uno (ATmega328P):

### Parametri di Configurazione

```

RAM:   [======    ]  58.0% (1188 / 2048 bytes)| Parametro | Tipo | Default | Descrizione |

Flash: [==        ]  21.5% (6924 / 32256 bytes)|-----------|------|---------|-------------|

```| `version` | string | "1.0.0" | Versione firmware |

| `deviceName` | string | "Arduino-Device" | Nome identificativo |

Tips for staying under RAM limit:| `networkSSID` | string | "Unknown" | SSID rete (info) |

- Use `F()` macro for strings: `Serial.println(F("text"))`| `baudRate` | uint32 | 115200 | Baud rate seriale |

- Avoid large JSON parsing (disabled by default)| `leds.red` | uint8 | 10 | Pin LED rosso |

- Use PROGMEM for constant data| `leds.orange` | uint8 | 11 | Pin LED arancione |

| `leds.green` | uint8 | 12 | Pin LED verde |

## 🧪 Testing| `blinkIntervalMs` | uint16 | 200 | Intervallo lampeggio (ms) |



Tests use Unity framework (PlatformIO built-in):### Configurazione Embedded



```cppPer default, la configurazione è embedded nel codice (`main.cpp`):

// test/test_embedded/test_example.cpp

#include <unity.h>```cpp

const char* SYSTEM_CONFIG = R"({

void test_example(void) {    "version": "1.0.0",

    TEST_ASSERT_EQUAL(42, my_function());    "deviceName": "Arduino-Softair",

}    // ... resto della configurazione

})";

void setup() {```

    UNITY_BEGIN();

    RUN_TEST(test_example);### Configurazione Esterna (Futuro)

    UNITY_END();

}Il sistema supporterà caricamento da:

```- 📁 File su SD card

- 💾 EEPROM

Run with: `pio test`- 📡 Download da server HTTP



## 🖥️ Wokwi Simulation## 📖 Utilizzo



The project includes a complete Wokwi circuit (`diagram.json`):### Comandi Seriali Interattivi

- Arduino Uno

- 3x LEDs (Red, Orange, Green) with 220Ω resistorsApri il monitor seriale (115200 baud) e usa questi comandi:

- Button (future expansion)

| Comando | Azione |

**Workflow:**|---------|--------|

1. `pio run` generates `.pio/build/uno/firmware.hex`| `s` | Mostra statistiche sistema |

2. Wokwi loads the firmware automatically via `wokwi.toml`| `i` | Mostra informazioni configurazione |

3. Simulates hardware without physical board| `r` | Reset statistiche |

| `h` o `?` | Mostra aiuto |

## 🔧 Customization| `t` | Toggle manuale LED (test) |



### Add New Library### Esempio Sessione



```bash```

# Create library folder╔════════════════════════════════════════════════════════╗

mkdir -p lib/MyLibrary║          ARDUINO LED & BUTTON DEMO                     ║

║          With Boot Sequence System                     ║

# Add header and source╚════════════════════════════════════════════════════════╝

touch lib/MyLibrary/MyLibrary.h

touch lib/MyLibrary/MyLibrary.cppPhase 1: Loading configuration...

✓ Configuration loaded successfully

# PlatformIO auto-discovers and links it

```Phase 2: Running boot sequence...

[RED LED] System start

### Modify Boot Sequence[ORANGE LED] Reading configuration...

[ORANGE LED] Initializing system...

Edit `lib/Utilities/SystemInitializer.cpp`:✓ Boot sequence completed successfully



```cppPhase 3: Configuring hardware...

bool SystemInitializer::initialize() {✓ Button configured on pin 2

    setRedLed();✓ LED configured on pin 13

    delay(500);  // Adjust timing✓ Button interrupt attached

    

    setOrangeLedBlinking();╔════════════════════════════════════════════════════════╗

    // Add custom initialization here║              SYSTEM READY                              ║

    ╚════════════════════════════════════════════════════════╝

    setGreenLed();

    return true;Monitoring button presses...

}

```Button pressed! LED is now ON

  → Total presses: 1

## 📚 Resources  → Free RAM: 1432 bytes



- [PlatformIO Documentation](https://docs.platformio.org/)Button pressed! LED is now OFF

- [Arduino Uno Reference](https://docs.arduino.cc/hardware/uno-rev3)  → Total presses: 2

- [Wokwi Simulator Docs](https://docs.wokwi.com/)  → Free RAM: 1432 bytes

- [Unity Test Framework](http://www.throwtheswitch.org/unity)```



## 📄 License## 🏗️ Architettura



MIT License - Feel free to use this template for your projects!### Struttura del Progetto



## 🤝 Contributing```

softair/

This is a template repository. Fork it and customize for your needs!├── src/

│   └── main.cpp              # Applicazione principale

---├── lib/

│   └── Utilities/

**Happy coding! 🚀**│       ├── ConfigLoader.h    # Caricamento configurazione JSON

│       ├── ConfigLoader.cpp
│       ├── SystemInitializer.h  # Boot sequence manager
│       └── SystemInitializer.cpp
├── test/
│   └── test_embedded/
│       ├── test_config_loader.cpp     # Test ConfigLoader
│       └── test_system_init.cpp       # Test SystemInitializer
├── docs/
│   ├── ARDUINO.md            # Documentazione Arduino
│   ├── config-example.json   # Esempio configurazione
│   └── config-examples.md    # Esempi configurazioni
├── platformio.ini            # Configurazione PlatformIO
└── README.md                 # Questo file
```

### Classi Principali

#### ConfigLoader
Gestisce il caricamento e parsing della configurazione JSON.

```cpp
SystemConfig config;
if (ConfigLoader::loadFromJson(jsonString, config)) {
    // Configurazione caricata con successo
}
```

#### SystemInitializer
Gestisce la boot sequence con indicatori LED.

```cpp
SystemInitializer init(config);
if (init.initialize()) {
    // Inizializzazione completata
}
```

### Diagramma Stati Boot Sequence

```
START
  ↓
READING_CONFIG (LED Rosso)
  ↓
INITIALIZING (LED Arancione lampeggiante)
  ↓
READY (LED Verde)
  ↓
[Error] → ERROR (Tutti LED spenti)
```

### Interrupt & Timing

- **Button Interrupt**: `attachInterrupt()` su pin 2
- **Debounce**: Software, 50ms default
- **Stats Display**: Ogni 10 secondi (in DEBUG mode)

## 🧪 Testing

### Esecuzione Test

```bash
# Esegui tutti i test su hardware
pio test -e uno

# Test specifici
pio test -e uno --filter test_config_loader
pio test -e uno --filter test_system_init
```

### Coverage Test

Il progetto include test completi per:

#### ConfigLoader (test_config_loader.cpp)
- ✅ Parsing JSON valido
- ✅ Gestione JSON malformato
- ✅ Validazione campi richiesti
- ✅ Gestione valori di default
- ✅ Test boundary values
- ✅ Validazione tipi di dati
- ✅ Gestione pin duplicati

#### SystemInitializer (test_system_init.cpp)
- ✅ Stati di inizializzazione
- ✅ Boot sequence completa
- ✅ Validazione configurazione
- ✅ Gestione errori
- ✅ Test integrazione con ConfigLoader

### Output Test di Esempio

```
test/test_embedded/test_config_loader.cpp:42:test_load_valid_config	[PASSED]
test/test_embedded/test_config_loader.cpp:43:test_get_default_config	[PASSED]
test/test_embedded/test_system_init.cpp:89:test_system_initializer_completes_successfully	[PASSED]
-----------------------
16 Tests 0 Failures 0 Ignored 
OK
```

## 📚 API Reference

### ConfigLoader Class

#### Static Methods

```cpp
static bool loadFromJson(const char* jsonString, SystemConfig& config);
static SystemConfig getDefaultConfig();
```

### SystemInitializer Class

#### Constructor

```cpp
explicit SystemInitializer(const SystemConfig& config);
```

#### Methods

```cpp
bool initialize();                    // Esegue boot sequence
InitState getState() const;           // Ritorna stato corrente
void printSystemInfo() const;         // Stampa info su seriale
```

### SystemConfig Struct

```cpp
struct SystemConfig {
    const char* version;
    const char* deviceName;
    const char* networkSSID;
    uint32_t baudRate;
    uint8_t redLedPin;
    uint8_t orangeLedPin;
    uint8_t greenLedPin;
    uint16_t blinkIntervalMs;
};
```

### InitState Enum

```cpp
enum class InitState {
    START,          // Inizializzazione non avviata
    READING_CONFIG, // Lettura configurazione
    INITIALIZING,   // Inizializzazione in corso
    READY,          // Sistema pronto
    ERROR           // Errore durante init
};
```

## 🐛 Debug

### Modalità Debug

Attiva il debug aggiungendo al `platformio.ini`:

```ini
build_flags = -D DEBUG=1 -Wall -Wextra
```

In modalità DEBUG vedrai:
- Output dettagliato boot sequence
- RAM disponibile
- Statistiche automatiche ogni 10 secondi
- Informazioni extra su ogni pressione pulsante

### Risoluzione Problemi Comuni

#### LED non si accendono
- ✅ Verifica connessioni hardware
- ✅ Controlla resistenze (220Ω)
- ✅ Verifica orientamento LED (anodo/catodo)
- ✅ Controlla pin nella configurazione

#### Pulsante non risponde
- ✅ Verifica connessione a pin 2 e GND
- ✅ Controlla che il pulsante sia normalmente aperto
- ✅ Verifica nel monitor seriale se viene rilevato

#### Upload fallito
- ✅ Verifica connessione USB
- ✅ Seleziona porta corretta
- ✅ Chiudi altri programmi che usano la seriale
- ✅ Premi reset su Arduino prima dell'upload

---

## 🔋 Battery Monitor (Implemented - EPIC_01)

**Status:** ✅ Completato - Monitoring attivo con 4 stati di carica

Sistema di monitoraggio batteria con feedback LED basato su soglie di tensione e lettura ADC periodica.

### Implementazione Attuale

**Hardware:**
- **Potenziometro** su A0 (simula tensione batteria 0-5V in Wokwi)
- **4× LED** su pin D2/D3/D4/D5 con resistenze 330Ω
- **ADC 10-bit**: Lettura analogica con conversione a volt

**Software:**
- Polling periodico ogni 2 secondi (`BATTERY_UPDATE_INTERVAL`)
- Threshold-based state determination (no state machine)
- Funzioni: `readBatteryVoltage()`, `updateBatteryLeds()`
- Memory footprint: ~20 bytes RAM, ~400 bytes Flash

### Voltage Thresholds (Test Configuration)

| Stato | Tensione @ A0 | Range POT | LED | Pin |
|-------|---------------|-----------|-----|-----|
| **CRITICAL** | < 1.25V | 0-25% | 🔴 Red | D5 |
| **LOW** | 1.25-2.50V | 25-50% | 🟠 Orange | D4 |
| **GOOD** | 2.50-3.75V | 50-75% | 🟡 Yellow | D3 |
| **FULL** | ≥ 3.75V | 75-100% | 🟢 Green | D2 |

> **Note:** Soglie ottimizzate per test con potenziometro 0-5V in Wokwi. Per batteria reale 9V con voltage divider R1=R2=10kΩ, usare soglie originali (V_FULL=4.25V, V_GOOD=4.0V, V_LOW=3.75V).

### Schema Circuito (Wokwi Simulation)

```
                    Arduino Uno
               ┌─────────────────┐
               │                 │
POT 5V ────────┤ 5V              │
               │                 │
POT SIG ───────┤ A0              │ ← Voltage sensing (0-5V)
               │                 │
POT GND ───────┤ GND             │
               │                 │
               │  D2 ├───[330Ω]──●─┐ LED Green (FULL)
               │                    ─
               │  D3 ├───[330Ω]──●─┐ LED Yellow (GOOD)
               │                    ─
               │  D4 ├───[330Ω]──●─┐ LED Orange (LOW)
               │                    ─
               │  D5 ├───[330Ω]──●─┐ LED Red (CRITICAL)
               │                    ─
               │         GND ──────┴── Common Ground
               │                 │
               └─────────────────┘
```

### Schema Circuito (Hardware Reale con Batteria)

```
                         Arduino Uno
                    ┌─────────────────┐
                    │                 │
VBatt (9V) ─────┬───┤ VIN             │
                │   │                 │
                │   │  A0 ├───────────┼─── Voltage Sense (Vout)
                │   │                 │
             [R1]   │  D2 ├───[330Ω]───●──┐ LED Green (FULL)
             10kΩ   │                      ─
                │   │  D3 ├───[330Ω]───●──┐ LED Yellow (GOOD)
                ├───┤                      ─
                │   │  D4 ├───[330Ω]───●──┐ LED Orange (LOW)
             [R2]   │                      ─
             10kΩ   │  D5 ├───[330Ω]───●──┐ LED Red (CRITICAL)
                │   │                      ─
               GND  │         GND ────┴─── Common Ground
                    │                 │
                    └─────────────────┘

Voltage Divider: Vout = VBatt × (R2/(R1+R2)) = 9V × 0.5 = 4.5V ✅
ADC Reading: (4.5V / 5V) × 1023 ≈ 921 counts
```

### Codice Esempio

```cpp
// In main.cpp - chiamato ogni 2 secondi nel loop()
if (millis() - lastBatteryUpdate >= BATTERY_UPDATE_INTERVAL) {
    currentVoltage = readBatteryVoltage();  // Legge A0, converte a volt
    updateBatteryLeds(currentVoltage);      // Accende LED appropriato
    lastBatteryUpdate = millis();
    
    #ifdef DEBUG
    Serial.print(F("Battery: "));
    Serial.print(currentVoltage, 2);
    Serial.println(F("V"));
    #endif
}
```

### API Reference (Battery Monitor)

```cpp
// Read battery voltage from ADC
float readBatteryVoltage();
// Returns: voltage at A0 pin (0.0-5.0V)

// Update battery status LEDs
void updateBatteryLeds(float voltage);
// Turns off all LEDs, then lights appropriate one based on thresholds
```

### Test Wokwi

1. **Avvia simulatore**: Build con `pio run`, poi "Wokwi: Start Simulator"
2. **Ruota potenziometro**: Osserva cambio LED ogni 2 secondi
3. **Monitor seriale** (DEBUG): `Battery: X.XXV` ogni update
4. **Verifica soglie**:
   - 0-25% POT → LED rosso
   - 25-50% POT → LED arancione
   - 50-75% POT → LED giallo
   - 75-100% POT → LED verde

### Future Enhancements (EPIC_01 Full)

Per implementazione completa, considerare:

- **State Machine con Hysteresis**: Evitare flapping LED vicino a soglie
- **Entry Actions**: Buzzer beep solo al cambio stato (LOW→CRITICAL)
- **Moving Average Filter**: 10 campioni @ 100ms per smoothing
- **Auto Shutdown**: Spegnimento automatico sotto CRITICAL per >30s
- **Calibrazione**: Offset ADC configurabile per compensare tolleranze R1/R2

📖 **Documentazione completa:**
- Epic design: `EPIC_01.md`
- Flow diagrams: `main.flow.md` (Flowchart 4: Battery Monitor)
- Copilot patterns: `.github/copilot-instructions.md`

---

## ⏰ Display Controller (4× 7-Segment)

### ✨ Caratteristiche

Sistema di visualizzazione tempo con **multiplexing hardware** per display 7-segmenti:

- **4 Display 7-segmenti** common-cathode (formato HH:mm)
- **74HC595 Shift Register** per controllo segmenti (pin 6/7/8)
- **Multiplexing 50Hz** (5ms per digit) - refresh non-blocking
- **Colon lampeggiante** (500ms) su D2/D3 per scandire i secondi
- **Rotazione 180°** display D3/D4 per layout clock-style
- **Test diagnostico** all'avvio (2s tutti accesi + 3× blink)

### 🔌 Configurazione Hardware

**Pin Utilizzati:**
```cpp
// 74HC595 Shift Register
const uint8_t DISPLAY_DATA_PIN = 6;   // DS (Serial Data)
const uint8_t DISPLAY_CLOCK_PIN = 7;  // SHCP (Shift Clock)
const uint8_t DISPLAY_LATCH_PIN = 8;  // STCP (Latch Clock)

// Digit Selectors (common cathode control)
const uint8_t DISPLAY_DIGIT1_PIN = A1;  // Ore decine (leftmost)
const uint8_t DISPLAY_DIGIT2_PIN = A2;  // Ore unità (DP colon)
const uint8_t DISPLAY_DIGIT3_PIN = A3;  // Minuti decine (DP colon, rotated 180°)
const uint8_t DISPLAY_DIGIT4_PIN = A4;  // Minuti unità (rightmost, rotated 180°)
```

**Cablaggio 74HC595:**
- Q0-Q7 → Segmenti A-G + DP (parallel a tutti i 4 display)
- Arduino A1-A4 → PIN COM dei display (multiplexing digit selector)
- VCC → 5V, GND → GND

### 📊 API DisplayController

```cpp
#include <DisplayController.h>

// Inizializzazione
DisplayController display(DATA_PIN, CLOCK_PIN, LATCH_PIN, 
                         DIGIT1_PIN, DIGIT2_PIN, DIGIT3_PIN, DIGIT4_PIN);
display.begin();

// Display tempo HH:MM
display.displayTime(12, 34);  // Mostra "12:34"

// Lampeggio colon (chiamare ogni 500ms in loop)
display.setColonBlink(true);   // Mostra ":"
display.setColonBlink(false);  // Nascondi ":"

// Refresh multiplexing (chiamare continuamente in loop!)
display.refresh();  // Non-blocking, ~5ms per digit

// Test diagnostico (chiamare in setup)
display.runDiagnosticTest();  // 2s ON + 3× blink (blocking ~5s)
```

### 🎬 Sequenza di Boot Display

1. **Phase 1 (2s):** Tutti i segmenti A-G + DP accesi → verifica hardware
2. **Phase 2 (3s):** 3 cicli blink (ON/OFF 500ms) → test multiplexing
3. **Funzionamento normale:** Display "12:34" con ":" lampeggianti a 1Hz

### 💾 Footprint Memoria

- **RAM:** ~50 bytes (4 digit buffers + state variables)
- **Flash:** ~1.2KB (pattern tables + multiplexing logic)
- **Totale progetto:** 61.7% RAM (1264/2048), 39.2% Flash (12648/32256)

### 🧪 Test & Debugging

**Test hardware (Wokwi simulation):**
1. Build firmware: `pio run`
2. Avvia Wokwi: `Ctrl+Shift+P` → "Wokwi: Start Simulator"
3. Osserva sequenza boot e tempo incrementante

**Comandi seriali:**
```
s - Mostra statistiche tempo/sistema
i - Info progetto e versione
r - Reset statistiche
h - Help comandi
```

**Verifica visiva:**
- ✅ Tutti i segmenti si accendono in Phase 1 (2s)
- ✅ Blink sincronizzato in Phase 2 (3× 500ms)
- ✅ ":" lampeggia ogni 0.5s durante funzionamento
- ✅ Tempo incrementa ogni secondo (12:34 → 12:35 → ...)

### 🔧 Pattern di Design

**Multiplexing non-blocking:**
```cpp
// In loop() - chiamare continuamente!
display->refresh();  // Aggiorna UN digit ogni 5ms
```

**Lampeggio colon (1Hz):**
```cpp
// In loop() - ogni 500ms
if (millis() - lastBlink >= 500) {
    colonState = !colonState;
    display->setColonBlink(colonState);
    lastBlink = millis();
}
```

**Rotazione 180° segmenti:**
- D1, D2: orientamento normale
- D3, D4: `getSegmentPattern(num, true)` → rotazione software A↔D, B↔E, C↔F

---

## 📝 Licenza

Questo progetto è rilasciato sotto licenza MIT. Vedi file `LICENSE` per dettagli.

## 🤝 Contribuire

Contributi benvenuti! Per favore:
1. Fork del repository
2. Crea un branch per la feature (`git checkout -b feature/AmazingFeature`)
3. Commit delle modifiche (`git commit -m 'Add AmazingFeature'`)
4. Push al branch (`git push origin feature/AmazingFeature`)
5. Apri una Pull Request

## 📧 Contatti

Per domande o supporto, apri una issue su GitHub.

## 🙏 Ringraziamenti

- Arduino community
- PlatformIO team
- ArduinoJson library di Benoit Blanchon

---

**Made with ❤️ for Arduino enthusiasts**
