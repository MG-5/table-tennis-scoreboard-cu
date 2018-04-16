#pragma once
#include <avr/io.h>
#include <math.h>

#include "settings.h"

void showScoreline(uint8_t left, uint8_t right, uint8_t (&digits)[5]);

void showServes(bool myself, uint8_t serves, uint8_t (&digits)[5]);

void showTemp(uint8_t (&digits)[5]);
