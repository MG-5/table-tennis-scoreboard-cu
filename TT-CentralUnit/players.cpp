#include "players.h"

Player::Player()
{
}

uint8_t Player::getScore()
{
  return score;
}

uint8_t Player::incrementScore()
{
  if (score < 255)
    score++;

  return score;
}

uint8_t Player::decrementScore()
{
  if (score > 0)
    score--;

  return score;
}

void Player::clearScore()
{
  score = 0;
}

uint8_t Player::getServes()
{
	return serves;
}

uint8_t Player::incrementServes()
{
  if (serves < 5)
    serves++;

  return serves;
}

uint8_t Player::decrementServes()
{
  if (serves > 0)
    serves--;

  return serves;
}

void Player::clearServes()
{
  serves = 0;
}

void Player::refillServes()
{
  serves = 5;
}
