#pragma once

#include <avr/io.h>
#include <math.h>

#include "enums.h"
#include "players.h"
#include "settings.h"

const uint8_t digitToSegment[] = {
    // 0bPGFEDCBA
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b01110111, // A
    0b01111100, // b
    0b00111001, // C
    0b01011110, // d
    0b01111001, // E
    0b01110001  // F
};

void startupSequence();

void showScoreline(uint8_t left, uint8_t right, uint8_t (&digits)[5]);

void showServes(bool myself, uint8_t serves, uint8_t (&digits)[5]);

void showTemp(uint8_t (&digits)[5]);

void showError(Errors error, uint8_t (&p1)[5], uint8_t (&p2)[5]);
