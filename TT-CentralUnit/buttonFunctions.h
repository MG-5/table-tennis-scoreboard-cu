#pragma once

#include <avr/io.h>

#include "common/OneButton2.h"
#include "common/bit_manipulation.h"
#include "main.h"

void buttons_init();

void checkForButtonUpdates(uint8_t interruptFlags);

void playerOne_btn1_click();

void playerOne_btn1_longPressStart();

void playerOne_btn2_click();

void playerOne_btn2_longPressStart();

void playerTwo_btn1_click();

void playerTwo_btn1_longPressStart();

void playerTwo_btn2_click();

void playerTwo_btn2_longPressStart();
