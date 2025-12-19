#include "CountdownController.h"

CountdownController::CountdownController()
    : hours_(10)      // Default 10:00
    , minutes_(0)
    , cursorPos_(0)   // Start su H1
{
}

bool CountdownController::setTime(uint8_t hours, uint8_t minutes) {
    // Validazione range
    if (hours > 23 || minutes > 59) {
        return false;  // Valore invalido, no-op
    }
    
    hours_ = hours;
    minutes_ = minutes;
    return true;
}

void CountdownController::getTime(uint8_t& hours, uint8_t& minutes) const {
    hours = hours_;
    minutes = minutes_;
}

void CountdownController::moveCursorLeft() {
    if (cursorPos_ == 0) {
        cursorPos_ = 3;  // Wrap H1 → M2
    } else {
        cursorPos_--;
    }
}

void CountdownController::moveCursorRight() {
    if (cursorPos_ == 3) {
        cursorPos_ = 0;  // Wrap M2 → H1
    } else {
        cursorPos_++;
    }
}

void CountdownController::incrementDigit() {
    uint8_t h1 = hours_ / 10;      // Tens of hours
    uint8_t h2 = hours_ % 10;      // Units of hours
    uint8_t m1 = minutes_ / 10;    // Tens of minutes
    uint8_t m2 = minutes_ % 10;    // Units of minutes
    
    switch (cursorPos_) {
        case 0:  // H1 (tens of hours)
            h1++;
            if (h1 > 2) h1 = 0;  // Max 2 (23:59 è il massimo)
            // Se H1 passa a 2 e H2 > 3, correggi H2
            if (h1 == 2 && h2 > 3) {
                h2 = 3;
            }
            break;
            
        case 1:  // H2 (units of hours)
            h2++;
            if (h1 == 2) {
                // Se H1=2, max H2=3 (23:xx)
                if (h2 > 3) h2 = 0;
            } else {
                // Altrimenti max H2=9
                if (h2 > 9) h2 = 0;
            }
            break;
            
        case 2:  // M1 (tens of minutes)
            m1++;
            if (m1 > 5) m1 = 0;  // Max 5 (59 minuti max)
            break;
            
        case 3:  // M2 (units of minutes)
            m2++;
            if (m2 > 9) m2 = 0;
            break;
    }
    
    // Ricostruisci hours_ e minutes_
    hours_ = h1 * 10 + h2;
    minutes_ = m1 * 10 + m2;
    
    validateTime();  // Safety check
}

void CountdownController::decrementDigit() {
    uint8_t h1 = hours_ / 10;
    uint8_t h2 = hours_ % 10;
    uint8_t m1 = minutes_ / 10;
    uint8_t m2 = minutes_ % 10;
    
    switch (cursorPos_) {
        case 0:  // H1
            if (h1 == 0) {
                h1 = 2;  // Wrap 0 → 2
            } else {
                h1--;
            }
            // Se H1 passa a <2 e H2 era limitato, non serve correzione
            // (H2 può ora andare fino a 9)
            break;
            
        case 1:  // H2
            if (h2 == 0) {
                // Wrap dipende da H1
                h2 = (h1 == 2) ? 3 : 9;
            } else {
                h2--;
            }
            break;
            
        case 2:  // M1
            if (m1 == 0) {
                m1 = 5;  // Wrap 0 → 5
            } else {
                m1--;
            }
            break;
            
        case 3:  // M2
            if (m2 == 0) {
                m2 = 9;  // Wrap 0 → 9
            } else {
                m2--;
            }
            break;
    }
    
    hours_ = h1 * 10 + h2;
    minutes_ = m1 * 10 + m2;
    
    validateTime();
}

uint8_t CountdownController::getDigit(uint8_t digitIndex) const {
    switch (digitIndex) {
        case 0: return hours_ / 10;      // H1
        case 1: return hours_ % 10;      // H2
        case 2: return minutes_ / 10;    // M1
        case 3: return minutes_ % 10;    // M2
        default: return 0;
    }
}

void CountdownController::validateTime() {
    // Clamp a valori validi se necessario
    if (hours_ > 23) {
        hours_ = 23;
    }
    if (minutes_ > 59) {
        minutes_ = 59;
    }
}

uint8_t CountdownController::getMaxValueForPosition(uint8_t pos) const {
    switch (pos) {
        case 0: return 2;  // H1: max 2
        case 1:  // H2: dipende da H1
            return (hours_ / 10 == 2) ? 3 : 9;
        case 2: return 5;  // M1: max 5
        case 3: return 9;  // M2: max 9
        default: return 0;
    }
}
