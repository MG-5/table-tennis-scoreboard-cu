#include "main.h"

volatile uint8_t buttonStates = 0b111111; // gespeicherte Zustaende
volatile uint8_t interruptFlags = 0;      // 0 - p1_1	1 - p1_2
                                          // 2 - p2_1	3 - p2_2
                                          // 4 - CU_1	5 - CU_2

Player playerOne;
Player playerTwo;

// display connection
uint32_t displayOneLastResponse = 0;
uint32_t displayTwoLastResponse = 0;
bool displayOneConnection = false;
bool displayTwoConnection = false;

// time stuff - use for player one and common
uint8_t timeCount_p1 = 0;
uint32_t prevTime_p1 = 0;

// time stuff - use only for player two
uint8_t timeCount_p2 = 0;
uint32_t prevTime_p2 = 0;

// states
CommonlyStates currentState_common = CommonlyStates::STARTUP_SEQ;
Mode currentMode = Mode::COMMONLY;
Errors currentError = Errors::NOTHING;
Ranks currentRank = Ranks::NOBODY;
ShowMode currentShowMode = ShowMode::MODE1;
ServesPlayer currentPlayer = ServesPlayer::NOBODY;

// previous states
CommonlyStates prevState_common = CommonlyStates::STARTUP_SEQ;
Mode prevMode = Mode::COMMONLY;

int main(void)
{
  /**PullUp-Widerstaende**/
  PUEA |= (1 << PUEA1) | (1 << PUEA2) | (1 << PUEA4) | (1 << PUEA5);
  PUEC |= (1 << PUEC0);

  /******Interrupts*******/
  GIMSK |= (1 << PCIE0) | (1 << PCIE2);                                    // enable PCINT0 & PCINT2
  PCMSK0 |= (1 << PCINT2) | (1 << PCINT1) | (1 << PCINT4) | (1 << PCINT5); // A1 & A2 & A4 & A5 // mask B3
  PCMSK2 |= (1 << PCINT12);                                                // mask C0

  /* Button - associated functions */
  buttons_init();

  DDRC |= LED_PIN;

  timer0_init();
  uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUD, F_CPU));
  uart1_init(UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUD, F_CPU));
  ADC_init();

  sei();

  uint32_t prevTime = 0;

  // wait for displays starting
  wait_ms(50);
  displayOneLastResponse = displayTwoLastResponse = millis();

  while (true)
  {
    checkForButtonUpdates(interruptFlags);

    // clang-format off
	/*
    if (millis() - prevTime2 >= 1000)
    {
      test = !test;
      prevTime2 = millis();

      if (test)
        PORTC |= LED_PIN;
      else
        PORTC &= ~LED_PIN;
    }
	*/
    // clang-format on

    processCurrentState();

    // 100Hz update rate
    if (millis() - prevTime >= 10)
    {
      prevTime = millis();

      updateDisplayOne();
      updateDisplayTwo();
    }

    if (uart_getc() == ANSWER_CODE)
    {
      displayOneLastResponse = millis();
      displayOneConnection = true;
    }

    if (uart1_getc() == ANSWER_CODE)
    {
      displayTwoLastResponse = millis();
      displayTwoConnection = true;
    }

    // connection to display one lost
    if (displayOneConnection && millis() - displayOneLastResponse >= 30)
    {
      displayOneConnection = false;
      prevMode = currentMode;
      prevState_common = currentState_common;

      currentMode = Mode::COMMONLY;
      currentState_common = CommonlyStates::ERROR;
      currentError = Errors::NO_CONN_DISP_P1;
      currentShowMode = ShowMode::MODE1;
      clearTimeVariables();
    }

    // connection to display two lost
    if (displayTwoConnection && millis() - displayTwoLastResponse >= 30)
    {
      displayTwoConnection = false;
      prevMode = currentMode;
      prevState_common = currentState_common;

      currentMode = Mode::COMMONLY;
      currentState_common = CommonlyStates::ERROR;
      currentError = Errors::NO_CONN_DISP_P2;
      currentShowMode = ShowMode::MODE1;
      clearTimeVariables();
    }
  }
}

inline void processCurrentState()
{
  switch (currentMode)
  {
    case Mode::INDIVIDUAL:
    {
      // player one
      switch (playerOne.state)
      {
        case IndividualStates::SCORE:
          showScoreline(playerOne.getScore(), playerTwo.getScore(), playerOne.digits);
          break;

        case IndividualStates::SERVES:
        {
          if (playerOne.getServes() > 0)
            showServes(true, playerOne.getServes(), playerOne.digits);
          else
            showServes(false, playerTwo.getServes(), playerOne.digits);

          if ((millis() - prevTime_p1) >= 1000)
          {
            prevTime_p1 = millis();

            if (++timeCount_p1 >= 3)
              playerOne.state = IndividualStates::SCORE;
          }
        }
        break;

        case IndividualStates::TEMP:
        {
          showTemp(playerOne.digits);

          if ((millis() - prevTime_p1) >= 1000)
          {
            prevTime_p1 = millis();

            if (++timeCount_p1 > 3)
              playerOne.state = IndividualStates::SCORE;
          }
        }
        break;
      }

      // player two
      switch (playerTwo.state)
      {
        case IndividualStates::SCORE:
          showScoreline(playerTwo.getScore(), playerOne.getScore(), playerTwo.digits);
          break;

        case IndividualStates::SERVES:
        {
          if (playerTwo.getServes() > 0)
            showServes(true, playerTwo.getServes(), playerTwo.digits);
          else
            showServes(false, playerOne.getServes(), playerTwo.digits);

          if ((millis() - prevTime_p2) >= 1000)
          {
            prevTime_p2 = millis();

            if (++timeCount_p2 >= 3)
              playerTwo.state = IndividualStates::SCORE;
          }
        }
        break;

        case IndividualStates::TEMP:
        {
          showTemp(playerTwo.digits);

          if ((millis() - prevTime_p2) >= 1000)
          {
            prevTime_p2 = millis();

            if (++timeCount_p2 > 3)
              playerTwo.state = IndividualStates::SCORE;
          }
        }
        break;
      }
    }
    break;

    case Mode::COMMONLY:
    {
      switch (currentState_common)
      {
        case CommonlyStates::STARTUP_SEQ:
        {
          if (millis() - prevTime_p1 >= 80)
          {
            prevTime_p1 = millis();

            if (timeCount_p1 < SEQ_LENGTH)
            {
              for (uint8_t k = 0; k < 4; k++)
                playerOne.digits[k] = playerTwo.digits[k] = STARTUP_SEQ[timeCount_p1];

              playerOne.digits[4] = playerTwo.digits[4] = (timeCount_p1 == 13);

              timeCount_p1++;
            }
            else
            {
              currentState_common = CommonlyStates::STARTUP_HELLO;
              clearTimeVariables_p1();
              break;
            }
          }
        }
        break;

        case CommonlyStates::STARTUP_HELLO:
        {
          // print running "HALLO"
          const uint8_t halLength = 5;

          if (millis() - prevTime_p1 >= 175)
          {
            prevTime_p1 = millis();

            if (timeCount_p1 == 0)
              for (uint8_t k = 0; k < 5; k++)
                playerOne.digits[k] = playerTwo.digits[k] = 0;

            else
              for (uint8_t k = 0; k < 3; k++)
                playerOne.digits[k] = playerTwo.digits[k] = playerOne.digits[k + 1];

            if (timeCount_p1 < halLength)
              playerOne.digits[3] = playerTwo.digits[3] = HALLO[timeCount_p1];

            else if (timeCount_p1 < halLength + 4)
              playerOne.digits[3] = playerTwo.digits[3] = 0;

            else
            {
              currentState_common = CommonlyStates::WAITING;
              clearTimeVariables_p1();
              break;
            }

            timeCount_p1++;
          }
        }
        break;

        case CommonlyStates::WAITING:
        {
          const uint8_t points[5] = {128, 128, 128, 128, false};

          if (currentShowMode == ShowMode::MODE1)
          {

            for (uint8_t i = 0; i < 5; i++)
            {
              playerOne.digits[i] = points[i];
              playerTwo.digits[i] = points[i];
            }
          }
          else
          {
            for (uint8_t i = 0; i < 5; i++)
            {
              playerOne.digits[i] = EMPTY_SEGMENTS[i];
              playerTwo.digits[i] = EMPTY_SEGMENTS[i];
            }
          }

          // blinking
          if ((millis() - prevTime_p1) >= 500)
          {
            prevTime_p1 = millis();

            if (currentShowMode == ShowMode::MODE1)
              currentShowMode = ShowMode::MODE2;
            else
              currentShowMode = ShowMode::MODE1;
          }
        }
        break;

        case CommonlyStates::RANKING:
        {
          if (currentShowMode == ShowMode::MODE1)
          {
            if (currentRank == Ranks::PLAYER_ONE)
            {
              // SIEG for p1
              for (uint8_t i = 0; i < 5; i++)
                playerOne.digits[i] = SIEG_SEGMENTS[i];

              // FAIL for p2
              for (uint8_t i = 0; i < 5; i++)
                playerTwo.digits[i] = FAIL_SEGMENTS[i];
            }
            else if (currentRank == Ranks::PLAYER_TWO)
            {
              // FAIL for p1
              for (uint8_t i = 0; i < 5; i++)
                playerOne.digits[i] = FAIL_SEGMENTS[i];

              // SIEG for p2
              for (uint8_t i = 0; i < 5; i++)
                playerTwo.digits[i] = SIEG_SEGMENTS[i];
            }
          }
          else
          {
            showScoreline(playerOne.getScore(), playerTwo.getScore(), playerOne.digits);
            showScoreline(playerTwo.getScore(), playerOne.getScore(), playerTwo.digits);
          }

          // switch between score and SIEG or FAIL
          if ((millis() - prevTime_p1) >= 1000)
          {
            prevTime_p1 = millis();

            if (currentShowMode == ShowMode::MODE1)
              currentShowMode = ShowMode::MODE2;
            else
              currentShowMode = ShowMode::MODE1;
          }
        }
        break;

        case CommonlyStates::ERROR:
        {
          // return error code
          // 11 - no connection to player_ones display
          // 22 - no connection to player_ones display
          // ACCU - accu warning

          switch (currentError)
          {
            case Errors::NOTHING:
              resetToPreviousMode();
              break;

            case Errors::NO_CONN_DISP_P1:
            {
              if (displayOneConnection)
                resetToPreviousMode();
            }
            break;
            case Errors::NO_CONN_DISP_P2:
            {
              if (displayTwoConnection)
                resetToPreviousMode();
            }
            break;

            case Errors::ACCU_WARN:
            {
              if (timeCount_p1 > 10) // 5 sec, every 500ms count is incrementing
                resetToPreviousMode();
            }
            break;
          }

          // render error code
          if (currentShowMode == ShowMode::MODE1)
            showError(currentError, playerOne.digits, playerTwo.digits);
          else
          {
            uint8_t errorMsg[5]{};

            for (uint8_t i = 0; i < 5; i++)
              playerOne.digits[i] = playerTwo.digits[i] = errorMsg[i];
          }

          // switching
          if ((millis() - prevTime_p1) >= 500)
          {
            prevTime_p1 = millis();
            timeCount_p1++;

            if (currentShowMode == ShowMode::MODE1)
              currentShowMode = ShowMode::MODE2;
            else
              currentShowMode = ShowMode::MODE1;
          }
        }
        break;
      }
    }
    break;
  }
}

void resetToPreviousMode()
{
  currentMode = prevMode;
  currentState_common = prevState_common;
  clearTimeVariables();
}

void compareScores()
{
  uint8_t p1 = playerOne.getScore();
  uint8_t p2 = playerTwo.getScore();

  if (abs(p1 - p2) >= 2)
  {
    if (p1 >= 21 && p1 > p2)
    {
      currentRank = Ranks::PLAYER_ONE;
      currentMode = Mode::COMMONLY;
      currentState_common = CommonlyStates::RANKING;
      currentShowMode = ShowMode::MODE1;
      clearTimeVariables();
      return;
    }
    else if (p2 >= 21 && p2 > p1)
    {
      currentRank = Ranks::PLAYER_TWO;
      currentMode = Mode::COMMONLY;
      currentState_common = CommonlyStates::RANKING;
      currentShowMode = ShowMode::MODE1;
      clearTimeVariables();
      return;
    }
  }
}

void updateServes(bool directionNormal)
{
  if (currentPlayer == ServesPlayer::PLAYERONE)
  {
    if (directionNormal)
    {
      if (playerOne.decrementServes() == 0)
      {
        currentPlayer = ServesPlayer::PLAYERTWO;
        playerTwo.refillServes();
        playerOne.state = IndividualStates::SERVES;
        playerTwo.state = IndividualStates::SERVES;
        clearTimeVariables();
      }
    }
    else
    {
      if (playerOne.getServes() == 5)
      {
        currentPlayer = ServesPlayer::PLAYERTWO;
        playerTwo.refillServes();
        playerOne.clearServes();
        playerOne.state = IndividualStates::SERVES;
        playerTwo.state = IndividualStates::SERVES;
        clearTimeVariables();
      }
      else
        playerOne.incrementServes();
    }
  }
  else if (currentPlayer == ServesPlayer::PLAYERTWO)
  {
    if (directionNormal)
    {
      if (playerTwo.decrementServes() == 0)
      {
        currentPlayer = ServesPlayer::PLAYERONE;
        playerOne.refillServes();
        playerOne.state = IndividualStates::SERVES;
        playerTwo.state = IndividualStates::SERVES;
        clearTimeVariables();
      }
    }
    else
    {
      if (playerTwo.getServes() == 5)
      {
        currentPlayer = ServesPlayer::PLAYERONE;
        playerOne.refillServes();
        playerTwo.clearServes();
        playerOne.state = IndividualStates::SERVES;
        playerTwo.state = IndividualStates::SERVES;
        clearTimeVariables();
      }
      else
        playerTwo.incrementServes();
    }
  }
}
void clearTimeVariables_p1()
{
  timeCount_p1 = 0;
  prevTime_p1 = millis();
}

void clearTimeVariables_p2()
{
  timeCount_p2 = 0;
  prevTime_p2 = millis();
}

void clearTimeVariables()
{
  clearTimeVariables_p1();
  clearTimeVariables_p2();
}

void updateDisplayOne()
{
  uart_putc(0x42);
  uart_putc(playerOne.digits[0]);
  uart_putc(playerOne.digits[1]);
  uart_putc(playerOne.digits[2]);
  uart_putc(playerOne.digits[3]);
  uart_putc(playerOne.digits[4]);
}

void updateDisplayTwo()
{
  uart1_putc(0x42);
  uart1_putc(playerTwo.digits[0]);
  uart1_putc(playerTwo.digits[1]);
  uart1_putc(playerTwo.digits[2]);
  uart1_putc(playerTwo.digits[3]);
  uart1_putc(playerTwo.digits[4]);
}

void ADC_init()
{
  // ADC einschalten und Prescaler 8 - 8MHz system clock -> 1MHz ADC Clock
  ADCSRA |= (1 << ADEN) | (1 << ADPS1) | (1 << ADPS0);

  ADC_read(0); // dummy Wandlung
}

uint16_t ADC_read(uint8_t channel)
{
  ADMUX = channel;             // select input
  ADCSRA |= (1 << ADSC);       // Wandlung starten
  while (ADCSRA & (1 << ADSC)) // Wandlung abwarten
  {
  }

  return ADCW; // ADC auslesen und zurückgeben
}

/* ADC Mehrfachmessung mit Mittelwertbbildung */
// nsamples <=64!!
uint16_t ADC_read_avg(uint8_t channel, uint8_t nsamples)
{
  uint16_t sum = 0;
  for (uint8_t i = 0; i < nsamples; ++i)
  {
    sum += ADC_read(channel);
  }

  return (uint16_t)(sum / nsamples);
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float adcToTemp(uint16_t val)
{
  float a = 0.00335401643468053;
  float b = 0.000275887095698156;
  float c = 2.94107946409955E-06;
  float d = 1.16353763429527E-07;

  uint32_t resistance = ((uint32_t)val * 10000) / (1024 - val);
  float temp2 = log(resistance / 10000.0);

  return (1.0 / (a + b * temp2 + c * temp2 * temp2 + d * temp2 * temp2 * temp2) - 264.8);
}

ISR(PCINT0_vect)
{
  if (check_bit(PINA, 1) != check_bit(buttonStates, 0))
  {
    toggle_bit(buttonStates, 0);
    set_bit(interruptFlags, 0);
  }
  else if (check_bit(PINA, 2) != check_bit(buttonStates, 1))
  {
    toggle_bit(buttonStates, 1);
    set_bit(interruptFlags, 1);
  }
  else if (check_bit(PINA, 4) != check_bit(buttonStates, 3))
  {
    toggle_bit(buttonStates, 3);
    set_bit(interruptFlags, 3);
  }
  else if (check_bit(PINA, 5) != check_bit(buttonStates, 4))
  {
    toggle_bit(buttonStates, 4);
    set_bit(interruptFlags, 4);
  }
}

ISR(PCINT2_vect)
{
  if (check_bit(PINC, 0) != check_bit(buttonStates, 2))
  {
    toggle_bit(buttonStates, 2);
    set_bit(interruptFlags, 2);
  }
}
