#include "displayFunctions.h"
#include "main.h"

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

void showTemp(uint8_t (&digits)[5])
{
  uint16_t temp_adc = ADC_read_avg(TEMP_CHANNEL, 3); // Temperaturmessung
  uint8_t value = truncf(adcToTemp(temp_adc));

  if (value > 9)
  {
    digits[0] = digitToSegment[value / 10];
    digits[1] = digitToSegment[value % 10];
    digits[2] = 99;
    digits[3] = digitToSegment[0xC];
    digits[4] = false;
  }
  else if (value <= 9)
  {
    digits[0] = 0;
    digits[1] = digitToSegment[value];
    digits[2] = 99;
    digits[3] = digitToSegment[0xC];
    digits[4] = false;
  }
}

void showError(Errors error, uint8_t (&p1)[5], uint8_t (&p2)[5])
{
  uint8_t errorMsg[5] = {0, digitToSegment[0xE], 64, 64, true};

  switch (error)
  {
    case Errors::NOTHING:
      break;

    case Errors::NO_CONN_DISP_P1:
      errorMsg[2] = errorMsg[3] = digitToSegment[1];
      break;

    case Errors::NO_CONN_DISP_P2:
      errorMsg[2] = errorMsg[3] = digitToSegment[2];
      break;

    case Errors::ACCU_WARN:
    {
      errorMsg[0] = digitToSegment[0xA];
      errorMsg[1] = digitToSegment[0xC];
      errorMsg[2] = digitToSegment[0xC];
      errorMsg[3] = 62; //U
    }
    break;
  }

  for (uint8_t i = 0; i < 5; i++)
    p1[i] = p2[i] = errorMsg[i];
}
