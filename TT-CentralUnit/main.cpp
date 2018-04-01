#include "main.h"

OneButton button_p1_1(&PINA, 1);
OneButton button_p1_2(&PINA, 2);

OneButton button_p2_1(&PINC, 0);
OneButton button_p2_2(&PINB, 3);

volatile uint8_t buttonStates = 0b111111; // gespeicherte Zustände
volatile uint8_t interruptFlags = 0;      // 0-p1_1	1-p1_2	2-p2_1	3-p2_2 4-CU_1 5-CU_2

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
  button_p1_1.attachClick(playerOne_btn1_click);
  button_p1_1.attachLongPressStart(playerOne_btn1_longPressStart);

  button_p1_2.attachClick(playerOne_btn2_click);
  button_p1_2.attachLongPressStart(playerOne_btn2_longPressStart);

  button_p2_1.attachClick(playerTwo_btn1_click);
  button_p2_1.attachLongPressStart(playerTwo_btn1_longPressStart);

  button_p2_2.attachClick(playerTwo_btn2_click);
  button_p2_2.attachLongPressStart(playerTwo_btn2_longPressStart);

  DDRC |= LED_PIN;
  timer0_init();
  uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUD, F_CPU));
  uart1_init(UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUD, F_CPU));
  ADC_init();

  sei();
  bool test = false;
  uint32_t prevTime = 0;

  while (true)
  {

#pragma region checking button inputs
    // check playerOne first button
    if (button_p1_1.state != 0)
      button_p1_1.tick();

    if (check_bit(interruptFlags, 0))
    {
      clear_bit(interruptFlags, 0);
      button_p1_1.tick();
    }

    // check playerOne second button
    if (button_p1_2.state != 0)
      button_p1_2.tick();

    if (check_bit(interruptFlags, 1))
    {
      clear_bit(interruptFlags, 1);
      button_p1_2.tick();
    }

    // check playerTwo first button
    if (button_p2_1.state != 0)
      button_p2_1.tick();

    if (check_bit(interruptFlags, 2))
    {
      clear_bit(interruptFlags, 2);
      button_p2_1.tick();
    }

    // check playerTwo second button
    if (button_p2_2.state != 0)
      button_p2_2.tick();

    if (check_bit(interruptFlags, 3))
    {
      clear_bit(interruptFlags, 3);
      button_p2_2.tick();
    }
#pragma endregion

    /*
        if (millis() - prevTime2 >= 1000)
        {
          test = !test;
          prevTime2 = millis();

          if (test)
            PORTC |= LED_PIN;
          else
            PORTC &= ~LED_PIN;
        }*/

    if (check_bit(PINC, 0))
      PORTC |= LED_PIN;
    else
      PORTC &= ~LED_PIN;

    // showScoreline(score_PlayerOne, score_PlayerTwo, digits_PlayerOne);
    // showScoreline(score_PlayerTwo, score_PlayerOne, digits_PlayerTwo);
    showTemp();

    if (millis() - prevTime >= 10)
    {
      prevTime = millis();

      updateDisplayOne();
      updateDisplayTwo();
    }
  }
}

void updateDisplayOne()
{
  uart_putc(0x42);
  uart_putc(digits_PlayerOne[0]);
  uart_putc(digits_PlayerOne[1]);
  uart_putc(digits_PlayerOne[2]);
  uart_putc(digits_PlayerOne[3]);
  uart_putc(digits_PlayerOne[4]);
}

void updateDisplayTwo()
{
  uart1_putc(0x42);
  uart1_putc(digits_PlayerTwo[0]);
  uart1_putc(digits_PlayerTwo[1]);
  uart1_putc(digits_PlayerTwo[2]);
  uart1_putc(digits_PlayerTwo[3]);
  uart1_putc(digits_PlayerTwo[4]);
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

void showServes(uint8_t serves, bool myself, uint8_t (&digits)[5])
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
    digits_PlayerOne[0] = digitToSegment[value / 10];
    digits_PlayerTwo[0] = digits_PlayerOne[0]; 

    digits_PlayerOne[1] = digitToSegment[value % 10];
    digits_PlayerTwo[1] = digits_PlayerOne[1];

    digits_PlayerOne[2] = 99;
    digits_PlayerTwo[2] = 99;

    digits_PlayerOne[3] = digitToSegment[0xC];
    digits_PlayerTwo[3] = digitToSegment[0xC];

    digits_PlayerOne[4] = false;
    digits_PlayerTwo[4] = false;
  }
  else if (value <= 9)
  {
    digits_PlayerOne[0] = 0;
    digits_PlayerTwo[0] = 0;

    digits_PlayerOne[1] = digitToSegment[value];
    digits_PlayerTwo[1] = digits_PlayerOne[1];

    digits_PlayerOne[2] = 99;
    digits_PlayerTwo[2] = 99;

    digits_PlayerOne[3] = digitToSegment[0xC];
    digits_PlayerTwo[3] = digitToSegment[0xC];

    digits_PlayerOne[4] = false;
    digits_PlayerTwo[4] = false;
  }
}

void playerOne_btn1_click()
{
  // TODO: add debounce 2 sec
  if (score_PlayerOne < 21)
    score_PlayerOne++;
}

void playerOne_btn1_longPressStart()
{

  /*
 if (score_PlayerOne > 0)
   score_PlayerOne--;
       */
}

void playerOne_btn2_click()
{
}

void playerOne_btn2_longPressStart()
{
}

void playerTwo_btn1_click()
{
  // TODO: add debounce 2 sec
  if (score_PlayerTwo < 21)
    score_PlayerTwo++;
}

void playerTwo_btn1_longPressStart()
{
  if (score_PlayerTwo > 0)
    score_PlayerTwo--;
}

void playerTwo_btn2_click()
{
}

void playerTwo_btn2_longPressStart()
{
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
