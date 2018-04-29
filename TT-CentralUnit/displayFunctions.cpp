#include "displayFunctions.h"
#include "main.h"

extern Player playerOne;
extern Player playerTwo;

void startupSequence()
{
  const uint8_t timePassed = 10 * 8;
  const uint8_t sequence_length = 15;
  const uint8_t sequence[sequence_length] = {1, 2, 4, 8, 16, 32, 64, 66, 67, 99, 115, 123, 127, 255, 0};
  uint32_t prevTimeStartup = millis();
  uint16_t timeCount = 0;

  uint8_t j = 0;
  timeCount = 0;

  while (j < sequence_length)
  {
    for (uint8_t k = 0; k < 4; k++)
    {
      playerOne.digits[k] = sequence[j];
      playerTwo.digits[k] = sequence[j];
    }

    if (j == 13)
    {
      playerOne.digits[4] = true;
      playerTwo.digits[4] = true;
    }
    else
    {
      playerOne.digits[4] = false;
      playerTwo.digits[4] = false;
    }

    timeCount = 0;

    while (timeCount < timePassed)
    {

      // 100Hz
      if (millis() - prevTimeStartup >= 10)
      {
        prevTimeStartup = millis();
        updateDisplayOne();
        updateDisplayTwo();

        uart_getc();
        uart1_getc();

        // TODO:
        // uart_clearBuffer();
        // uart1_clearBuffer();

        timeCount += 10;
      }
    }
    j++;
  }

  timeCount = 0;

  while (timeCount < 100)
  {
    // 100Hz
    if (millis() - prevTimeStartup >= 10)
    {
      prevTimeStartup = millis();
      updateDisplayOne();
      updateDisplayTwo();

      uart_getc();
      uart1_getc();
      // TODO:
      // uart_clearBuffer();
      // uart1_clearBuffer();

      timeCount += 10;
    }
  }

  // print running "HALLO"
  const uint8_t halLength = 5;
  uint8_t idxHal = 0;
  bool running = true;

  while (running)
  {
    for (uint8_t k = 0; k < 3; k++)
    {
      playerOne.digits[k] = playerOne.digits[k + 1];
      playerTwo.digits[k] = playerTwo.digits[k + 1];
    }

    if (idxHal < halLength)
    {
      playerOne.digits[3] = HALLO[idxHal];
      playerTwo.digits[3] = HALLO[idxHal];
    }
    else if (idxHal < halLength + 4)
    {
      playerOne.digits[3] = 0;
      playerTwo.digits[3] = 0;
    }
    else
      running = false;

    idxHal++;
    timeCount = 0;
    prevTimeStartup = millis();

    while (timeCount < 175)
    {
      // 100Hz
      if (millis() - prevTimeStartup >= 10)
      {
        prevTimeStartup = millis();
        updateDisplayOne();
        updateDisplayTwo();

        uart_getc();
        uart1_getc();

        // TODO:
        // uart_clearBuffer();
        // uart1_clearBuffer();

        timeCount += 10;
      }
    }
  }
}

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
    {
      errorMsg[2] = digitToSegment[1];
      errorMsg[3] = digitToSegment[1];
    }
    break;

    case Errors::NO_CONN_DISP_P2:
    {
      errorMsg[2] = digitToSegment[2];
      errorMsg[3] = digitToSegment[2];
    }
    break;

    case Errors::ACCU_WARN:
    {
      errorMsg[2] = digitToSegment[3];
      errorMsg[3] = digitToSegment[3];
    }
    break;

    case Errors::ACCU_CRITCIAL:
    {
      errorMsg[2] = digitToSegment[9];
      errorMsg[3] = digitToSegment[9];
    }
    break;
  }

  for (uint8_t i = 0; i < 5; i++)
  {
    p1[i] = errorMsg[i];
    p2[i] = errorMsg[i];
  }
}
