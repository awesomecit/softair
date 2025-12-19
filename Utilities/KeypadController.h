#ifndef KEYPAD_CONTROLLER_H
#define KEYPAD_CONTROLLER_H

#include <Arduino.h>

/**
 * @brief Enum delle chiavi del keypad 4×4
 * 
 * Layout fisico:
 *   1 2 3 A
 *   4 5 6 B
 *   7 8 9 C
 *   * 0 # D
 * 
 * Mapping funzionale (da EPIC_03):
 *   KEY_4 = LEFT
 *   KEY_6 = RIGHT
 *   KEY_2 = UP
 *   KEY_8 = DOWN
 *   KEY_A = MODE
 */
enum class KeypadKey : uint8_t {
    NONE = 0,
    KEY_1, KEY_2, KEY_3, KEY_A,
    KEY_4, KEY_5, KEY_6, KEY_B,
    KEY_7, KEY_8, KEY_9, KEY_C,
    KEY_STAR, KEY_0, KEY_HASH, KEY_D
};

/**
 * @brief Controller per tastierino matriciale 4×4
 * 
 * Implementa scanning non-blocking con debounce software (50ms).
 * Usa matrice row-scan: imposta row LOW, legge cols per detect pressione.
 * 
 * Pinout hardware (da EPIC_03.md):
 *   Rows: R1→D13, R2→D12, R3→D11, R4→D10
 *   Cols: C1→A5, C2→D9, C3→D8, C4→D7
 * 
 * Pattern:
 *   - Non-blocking: scan() chiamato ogni loop(), controlla micros() interno
 *   - Debounce: ignora transizioni entro DEBOUNCE_DELAY_MS (50ms)
 *   - Edge detection: ritorna chiave solo su pressione (LOW→HIGH transition)
 * 
 * Memoria footprint:
 *   - RAM: ~20 bytes (4+4 pin arrays + state vars)
 *   - Flash: ~600 bytes (matrix scan logic)
 * 
 * Esempio uso:
 *   KeypadController keypad(rowPins, colPins);
 *   keypad.begin();
 *   
 *   void loop() {
 *     KeypadKey key = keypad.scan();
 *     if (key != KeypadKey::NONE) {
 *       handleKeyPress(key);  // Gestisci pressione
 *     }
 *   }
 */
class KeypadController {
public:
    /**
     * @brief Costruttore
     * @param rowPins Array di 4 pin per le righe (R1-R4)
     * @param colPins Array di 4 pin per le colonne (C1-C4)
     */
    KeypadController(const uint8_t rowPins[4], const uint8_t colPins[4]);
    
    /**
     * @brief Inizializza il keypad
     * 
     * Configura:
     *   - Row pins: OUTPUT, HIGH (idle state)
     *   - Col pins: INPUT_PULLUP (detect LOW quando row active)
     */
    void begin();
    
    /**
     * @brief Scansiona il keypad (non-blocking)
     * 
     * Chiamare ad ogni loop(). Usa micros() per debounce timing.
     * 
     * Algoritmo:
     *   1. Se in debounce window → ritorna NONE
     *   2. Per ogni riga:
     *      - Imposta riga LOW (attiva)
     *      - Leggi tutte le colonne
     *      - Se col=LOW → chiave premuta a [row, col]
     *      - Riporta riga HIGH (disattiva)
     *   3. Se chiave trovata E diversa da lastKey_ → registra pressione
     *   4. Aggiorna lastKey_ e debounceTimestamp_
     * 
     * @return KeypadKey premuto (NONE se nessuna pressione o in debounce)
     */
    KeypadKey scan();
    
    /**
     * @brief Converte KeypadKey in carattere ASCII
     * @param key Chiave da convertire
     * @return Carattere ('1'-'9', '0', '*', '#', 'A'-'D', '?' per NONE)
     */
    static char keyToChar(KeypadKey key);

private:
    // Pin configuration
    uint8_t rowPins_[4];  ///< Pins per righe R1-R4
    uint8_t colPins_[4];  ///< Pins per colonne C1-C4
    
    // State tracking
    KeypadKey lastKey_;           ///< Ultima chiave rilevata (per edge detection)
    unsigned long debounceTimestamp_;  ///< Timestamp ultima transizione (micros)
    
    // Constants
    static constexpr unsigned long DEBOUNCE_DELAY_MS = 50;  ///< Debounce window (50ms)
    
    // Keypad matrix layout (4 rows × 4 cols)
    static constexpr KeypadKey KEY_MAP[4][4] = {
        { KeypadKey::KEY_1, KeypadKey::KEY_2, KeypadKey::KEY_3, KeypadKey::KEY_A },
        { KeypadKey::KEY_4, KeypadKey::KEY_5, KeypadKey::KEY_6, KeypadKey::KEY_B },
        { KeypadKey::KEY_7, KeypadKey::KEY_8, KeypadKey::KEY_9, KeypadKey::KEY_C },
        { KeypadKey::KEY_STAR, KeypadKey::KEY_0, KeypadKey::KEY_HASH, KeypadKey::KEY_D }
    };
    
    /**
     * @brief Legge stato di una singola chiave nella matrice
     * @param row Indice riga (0-3)
     * @param col Indice colonna (0-3)
     * @return KeypadKey a posizione [row, col] se premuta, altrimenti NONE
     */
    KeypadKey readKey(uint8_t row, uint8_t col);
};

#endif // KEYPAD_CONTROLLER_H
