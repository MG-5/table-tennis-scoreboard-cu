#pragma once
#include "enums.h"
#include <avr/io.h>
#include <math.h>

#include "settings.h"

void showScoreline(uint8_t left, uint8_t right, uint8_t (&digits)[5]);

void showServes(bool myself, uint8_t serves, uint8_t (&digits)[5]);

void showTemp(uint8_t (&digits)[5]);

void showError(Errors error, uint8_t (&p1)[5], uint8_t (&p2)[5]);