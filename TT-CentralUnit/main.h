#pragma once

#include <avr/interrupt.h>
#include <avr/io.h>

#include "common/A1001Display.h"
#include "common/timer0.h"
#include "common/uart.h"
#include "common/utils.h"

#include "settings.h"

uint8_t score_myself = 0;
uint8_t score_other = 0;
