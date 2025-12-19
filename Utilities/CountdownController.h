#ifndef COUNTDOWN_CONTROLLER_H
#define COUNTDOWN_CONTROLLER_H

#include <Arduino.h>

/**
 * @brief Controller per countdown timer con cursor navigation
 * 
 * Gestisce:
 *   - Valore tempo in formato HH:MM (0-3 digits: H1 H2 M1 M2)
 *   - Cursor position (0-3) per editing digit singolo
 *   - Increment/decrement con validazione max 23:59
 *   - Cursor movement con wrap (left/right)
 * 
 * Mapping cursor → digit:
 *   0 → H1 (tens of hours)
 *   1 → H2 (units of hours)
 *   2 → M1 (tens of minutes)
 *   3 → M2 (units of minutes)
 * 
 * Regole validazione (da EPIC_03):
 *   - H1: 0-2 (max 2 perché 23:59 è il massimo)
 *   - H2: 0-9 (ma max 3 quando H1=2)
 *   - M1: 0-5 (max 59 minuti)
 *   - M2: 0-9
 *   - Valore totale: max 23:59 (1439 minuti)
 * 
 * Esempio uso:
 *   CountdownController countdown;
 *   countdown.setTime(10, 0);  // Default 10:00
 *   
 *   // Navigazione cursor
 *   countdown.moveCursorLeft();   // Wrap 0 → 3
 *   countdown.moveCursorRight();  // Wrap 3 → 0
 *   
 *   // Editing
 *   countdown.incrementDigit();   // +1 alla posizione cursor
 *   countdown.decrementDigit();   // -1 alla posizione cursor
 *   
 *   // Lettura
 *   uint8_t h, m;
 *   countdown.getTime(h, m);      // Leggi ore e minuti
 *   uint8_t pos = countdown.getCursorPosition();  // 0-3
 * 
 * Memoria footprint:
 *   - RAM: ~10 bytes (hours_, minutes_, cursorPos_, prev state vars)
 *   - Flash: ~800 bytes (validation logic)
 */
class CountdownController {
public:
    /**
     * @brief Costruttore (default 10:00, cursor su H1)
     */
    CountdownController();
    
    /**
     * @brief Imposta tempo countdown
     * @param hours Ore (0-23)
     * @param minutes Minuti (0-59)
     * @return true se valido, false se fuori range (no-op)
     */
    bool setTime(uint8_t hours, uint8_t minutes);
    
    /**
     * @brief Leggi tempo countdown
     * @param hours Output: ore attuali
     * @param minutes Output: minuti attuali
     */
    void getTime(uint8_t& hours, uint8_t& minutes) const;
    
    /**
     * @brief Ottieni posizione cursor (0-3)
     * @return Cursor position: 0=H1, 1=H2, 2=M1, 3=M2
     */
    uint8_t getCursorPosition() const { return cursorPos_; }
    
    /**
     * @brief Muovi cursor a sinistra con wrap
     * 
     * 0 → 3 (H1 → M2)
     * 1 → 0 (H2 → H1)
     * 2 → 1 (M1 → H2)
     * 3 → 2 (M2 → M1)
     */
    void moveCursorLeft();
    
    /**
     * @brief Muovi cursor a destra con wrap
     * 
     * 0 → 1 (H1 → H2)
     * 1 → 2 (H2 → M1)
     * 2 → 3 (M1 → M2)
     * 3 → 0 (M2 → H1)
     */
    void moveCursorRight();
    
    /**
     * @brief Incrementa digit alla posizione cursor (+1 con validation)
     * 
     * Regole:
     *   - H1: 0→1→2→0 (max 2)
     *   - H2: dipende da H1
     *     - Se H1=2: 0→1→2→3→0 (max 3)
     *     - Altrimenti: 0→1→...→9→0
     *   - M1: 0→1→2→3→4→5→0 (max 5)
     *   - M2: 0→1→...→9→0
     * 
     * Se incremento causerebbe valore > 23:59 → wrap a valore valido minimo
     */
    void incrementDigit();
    
    /**
     * @brief Decrementa digit alla posizione cursor (-1 con validation)
     * 
     * Come incrementDigit() ma direzione opposta.
     * Wrap: 0 → max_value per quella posizione
     */
    void decrementDigit();
    
    /**
     * @brief Ottieni singolo digit per display (0-9)
     * @param digitIndex 0-3 (H1 H2 M1 M2)
     * @return Valore digit (0-9)
     */
    uint8_t getDigit(uint8_t digitIndex) const;

private:
    uint8_t hours_;     ///< Ore countdown (0-23)
    uint8_t minutes_;   ///< Minuti countdown (0-59)
    uint8_t cursorPos_; ///< Posizione cursor (0-3)
    
    /**
     * @brief Valida e corregge tempo se necessario
     * 
     * Se hours_ > 23 o minutes_ > 59, clamp a valore massimo valido (23:59)
     */
    void validateTime();
    
    /**
     * @brief Ottieni max value per digit alla posizione cursor
     * @param pos Posizione cursor (0-3)
     * @return Max valore (es: H1=2, H2=9 o 3, M1=5, M2=9)
     */
    uint8_t getMaxValueForPosition(uint8_t pos) const;
};

#endif // COUNTDOWN_CONTROLLER_H
