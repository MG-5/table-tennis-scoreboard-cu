#pragma once

#define UART_BAUD 1000000UL
#define LED_PIN (1 << PINC2)
#define TEMP_CHANNEL 0

const unsigned char SIEG_SEGMENTS[5] = {0b01101101, 0b00000110, 0b01111001, 0b01111101, false};
const unsigned char FAIL_SEGMENTS[5] = {0b01110001, 0b01110111, 0b00000110, 0b00111000, false};