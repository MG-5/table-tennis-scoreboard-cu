#pragma once

#define UART_BAUD 1000000UL
#define LED_PIN PINC2

#define PIEZO_PORT PORTA
#define PIEZO_PIN PINA4
#define TEMP_CHANNEL 0

#define RQ_CODE 0xC2
#define ANSWER_CODE 0xCC

#define ONE_MINUTE_IN_MS 60000

const uint8_t SEQ_LENGTH = 15;
const uint8_t STARTUP_SEQ[SEQ_LENGTH] = {1, 2, 4, 8, 16, 32, 64, 66, 67, 99, 115, 123, 127, 255, 0};

const unsigned char SIEG_SEGMENTS[5] = {0b01101101, 0b00000110, 0b01111001, 0b01111101, false};
const unsigned char FAIL_SEGMENTS[5] = {0b01110001, 0b01110111, 0b00000110, 0b00111000, false};
const unsigned char EMPTY_SEGMENTS[5] = {0, 0, 0, 0, false};

const unsigned char HALLO[5] = {118, 119, 56, 56, 63};
