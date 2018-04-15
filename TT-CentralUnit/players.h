#pragma once
#include <avr/io.h>

class Player
{
public:
  Player();

  uint8_t getScore();

  uint8_t incrementScore();

  uint8_t decrementScore();

  void clearScore();

  uint8_t getServes();

  uint8_t incrementServes();

  uint8_t decrementServes();

  void clearServes();
  
  void refillServes();
  
  uint8_t digits[5] = {0, 0, 0, 0, 0};
  
private:
  uint8_t score = 0;

  uint8_t serves = 0;
};
