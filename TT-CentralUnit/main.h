#pragma once

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>

#include "common/bit_manipulation.h"
#include "common/timer0.h"
#include "common/uart.h"
#include "common/utils.h"

#include "buttonFunctions.h"
#include "displayFunctions.h"
#include "enums.h"
#include "players.h"
#include "settings.h"

inline void processCurrentState();

void resetToScoreMode();

void compareScores();

void clearTimeVariables_p1();

void clearTimeVariables_p2();

void clearTimeVariables();

void updateDisplayOne();

void updateDisplayTwo();

void ADC_init();

uint16_t ADC_read(uint8_t channel);

uint16_t ADC_read_avg(uint8_t channel, uint8_t nsamples);

long map(long x, long in_min, long in_max, long out_min, long out_max);

float adcToTemp(uint16_t val);
