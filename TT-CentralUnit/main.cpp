#include "main.h"

volatile uint8_t buttonStates = 0b111111; // gespeicherte Zustände
volatile uint8_t interruptFlags = 0;      // 0-p1_1	1-p1_2	2-p2_1	3-p2_2 4-CU_1 5-CU_2

Player playerOne;
Player playerTwo;

uint8_t timeCount_p1 = 0;
uint32_t prevTime_p1 = 0;
uint8_t timeCount_p2 = 0;
uint32_t prevTime_p2 = 0;

CommonlyStates state_common = CommonlyStates::STARTUP;
Mode currentMode = Mode::COMMONLY;
Ranks currentRank = Ranks::NOBODY;

int main(void)
{
  /**Eingaenge
  DDRA &= ~((1 << PINA1) | (1 << PINA2) | (1 << PINA4) | (1 << PINA5));
  DDRC &= ~(1 << PINC0);
  DDRB &= ~(1 << PINB3);
  */

  /**PullUp-Widerstaende**/
  PUEA |= (1 << PUEA1) | (1 << PUEA2) | (1 << PUEA4) | (1 << PUEA5);
  PUEC |= (1 << PUEC0);
  PUEB |= (1 << PUEB3);

  /******Interrupts*******/
  GIMSK |= (1 << PCIE0) | (1 << PCIE1) | (1 << PCIE2);                     // enable PCINT0 & PCINT1 & PCINT2
  PCMSK0 |= (1 << PCINT2) | (1 << PCINT1) | (1 << PCINT4) | (1 << PCINT5); // A1 & A2 & A4 & A5
  PCMSK1 |= (1 << PCINT11);                                                // mask B3
  PCMSK2 |= (1 << PCINT12);                                                // mask C0

  /* Button - associated functions */
  buttons_init();

  DDRC |= LED_PIN;
  timer0_init();
  uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUD, F_CPU));
  uart1_init(UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUD, F_CPU));
  ADC_init();

  sei();
  bool test = false;
  uint32_t prevTime = 0;
  uint32_t prevTime2 = 0;

  while (true)
  {
    checkForButtonUpdates(interruptFlags);

    if (millis() - prevTime2 >= 1000)
    {
      test = !test;
      prevTime2 = millis();

      if (test)
        PORTC |= LED_PIN;
      else
        PORTC &= ~LED_PIN;
    }

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
      }
      break;
      }

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

          if (++timeCount_p2 >= 3)
            playerTwo.state = IndividualStates::SCORE;
        }
      }
      break;
      }
    }
    break;

    case Mode::COMMONLY:
    {
      switch (state_common)
      {
      case CommonlyStates::STARTUP:
      {
        // check for connection
        // startup animation
      }
      break;

      case CommonlyStates::RANKING:
      {
        // switch between score and SIEG or FAIL
      }
      break;

      case CommonlyStates::ERROR:
      {
        // return error code
        // 11 - no connection to player_ones display
        // 22 - no connection to player_ones display
        // 33 - accu warning
        // 99 - accu critical
      }
      break;
      }
    }
    break;
    }

    // 100Hz update rate
    if (millis() - prevTime >= 10)
    {
      prevTime = millis();

      updateDisplayOne();
      updateDisplayTwo();
    }
  }
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
    }
    else if (p2 >= 21 && p2 > p1)
    {
      currentRank = Ranks::PLAYER_TWO;
      currentMode = Mode::COMMONLY;
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
  else if (check_bit(PINA, 5) != check_bit(buttonStates, 4))
  {
    toggle_bit(buttonStates, 4);
    set_bit(interruptFlags, 4);
  }
  else if (check_bit(PINA, 4) != check_bit(buttonStates, 5))
  {
    toggle_bit(buttonStates, 5);
    set_bit(interruptFlags, 5);
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

ISR(PCINT1_vect)
{
  if (check_bit(PINB, 3) != check_bit(buttonStates, 3))
  {
    toggle_bit(buttonStates, 3);
    set_bit(interruptFlags, 3);
  }
}
