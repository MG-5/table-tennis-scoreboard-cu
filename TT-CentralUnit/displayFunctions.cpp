#include "displayFunctions.h"
#include "common/A1001Display.h"
#include "main.h"

extern Player playerOne;
extern Player playerTwo;

void showScoreline(uint8_t left, uint8_t right, uint8_t (&digits)[5])
{
  uint8_t players[2] = {left, right};
  digits[4] = true;

  for (uint8_t i = 0; i <= 1; i++)
  {
    if (players[i] > 9)
    {
      digits[2 * i] = digitToSegment[players[i] / 10];
      digits[2 * i + 1] = digitToSegment[players[i] % 10];
    }
    else if (players[i] <= 9)
    {
      if (i == 1)
      {
        digits[2 * i] = digitToSegment[players[i]];
        digits[2 * i + 1] = 0;
      }
      else
      {
        digits[2 * i] = 0;
        digits[2 * i + 1] = digitToSegment[players[i]];
      }
    }
  }
}

void showServes(bool myself, uint8_t serves, uint8_t (&digits)[5])
{
  digits[0] = 0;
  digits[1] = digitToSegment[0xA];

  if (myself)
    digits[2] = 0;
  else
    digits[2] = 64; // Minusstrich

  digits[3] = digitToSegment[serves];
  digits[4] = true;
}

void showTemp()
{
  uint16_t temp_adc = ADC_read_avg(TEMP_CHANNEL, 3); // Temperaturmessung
  uint8_t value = truncf(adcToTemp(temp_adc));

  if (value > 9)
  {

    playerOne.digits[0] = digitToSegment[value / 10];
    playerTwo.digits[0] = playerOne.digits[0];

    playerOne.digits[1] = digitToSegment[value % 10];
    playerTwo.digits[1] = playerOne.digits[1];

    playerOne.digits[2] = 99;
    playerTwo.digits[2] = 99;

    playerOne.digits[3] = digitToSegment[0xC];
    playerTwo.digits[3] = digitToSegment[0xC];

    playerOne.digits[4] = false;
    playerTwo.digits[4] = false;
  }
  else if (value <= 9)
  {
    playerOne.digits[0] = 0;
    playerTwo.digits[0] = 0;

    playerOne.digits[1] = digitToSegment[value];
    playerTwo.digits[1] = playerOne.digits[1];

    playerOne.digits[2] = 99;
    playerTwo.digits[2] = 99;

    playerOne.digits[3] = digitToSegment[0xC];
    playerTwo.digits[3] = digitToSegment[0xC];

    playerOne.digits[4] = false;
    playerTwo.digits[4] = false;
  }
}
