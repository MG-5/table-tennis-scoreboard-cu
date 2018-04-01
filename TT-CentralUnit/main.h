#pragma once

#include <avr/interrupt.h>
#include <avr/io.h>
#include <math.h>

#include "common/A1001Display.h"
#include "common/OneButton2.h"
#include "common/bit_manipulation.h"
#include "common/timer0.h"
#include "common/uart.h"
#include "common/utils.h"

#include "settings.h"

uint8_t score_PlayerOne = 0;
uint8_t score_PlayerTwo = 0;

uint8_t digits_PlayerOne[5] = {0, 0, 0, 0, 0};
uint8_t digits_PlayerTwo[5] = {0, 0, 0, 0, 0};

void updateDisplayOne();

void updateDisplayTwo();

void showScoreline(uint8_t left, uint8_t right, uint8_t (&digits)[5]);

void showServes(uint8_t serves, bool myself,uint8_t (&digits)[5]);

void showTemp();

void playerOne_btn1_click();

void playerOne_btn1_longPressStart();

void playerOne_btn2_click();

void playerOne_btn2_longPressStart();

void playerTwo_btn1_click();

void playerTwo_btn1_longPressStart();

void playerTwo_btn2_click();

void playerTwo_btn2_longPressStart();

void ADC_init();

uint16_t ADC_read(uint8_t channel);

uint16_t ADC_read_avg(uint8_t channel, uint8_t nsamples);

long map(long x, long in_min, long in_max, long out_min, long out_max);

float adcToTemp(uint16_t val);
