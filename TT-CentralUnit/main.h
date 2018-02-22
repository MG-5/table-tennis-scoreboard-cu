#pragma once

#include <avr/interrupt.h>
#include <avr/io.h>
#include "utils.h"

#include "A1001Display.h"
#include "settings.h"
#include "timer0.h"
#include "uart.h"

uint8_t score_myself = 0;
uint8_t score_other = 0;