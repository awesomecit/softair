# DEPRECATED Files

**Data spostamento**: 19 Dicembre 2025  
**Motivo**: File duplicati obsoleti - versioni attive sono in `Utilities/`

## File Deprecati

Questi file sono stati sostituiti dalle versioni in `Utilities/` directory:

- `BuzzerController.cpp` → **ATTIVO**: `Utilities/BuzzerController.cpp`
- `BuzzerController.h` → **ATTIVO**: `Utilities/BuzzerController.h`
- `ConfigLoader.cpp` → **ATTIVO**: `Utilities/ConfigLoader.cpp`
- `ConfigLoader.h` → **ATTIVO**: `Utilities/ConfigLoader.h`
- `SystemInitializer.cpp` → **ATTIVO**: `Utilities/SystemInitializer.cpp`
- `SystemInitializer.h` → **ATTIVO**: `Utilities/SystemInitializer.h`

## Perché Spostati?

1. **Confusione Include Path**: Avere file in root E in `Utilities/` causava ambiguità per il compilatore
2. **Versioni Obsolete**: I file root contenevano codice non aggiornato
3. **Organizzazione**: Tutte le utility devono stare in `Utilities/` per coerenza architetturale
4. **Build Flags**: `platformio.ini` specifica `-I Utilities` come path primario

## Posso Eliminarli?

**NO** - Mantenerli come backup storico per almeno 6 mesi. Dopo verifica che:
- Tutti i build funzionano correttamente
- Nessun riferimento ai file root nei sorgenti
- Almeno 2 release stabili con nuova struttura

Allora potranno essere rimossi permanentemente.

## Data Prevista Eliminazione

**Giugno 2026** (dopo 6 mesi dalla deprecazione)
