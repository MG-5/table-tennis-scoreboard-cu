#include "main.h"
#include <avr/io.h>

int main(void)
{
  DDRC |= (1 << PINC2);
  timer0_init();
  uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUD, F_CPU));

  sei();
  bool test = false;
  uint32_t prevTime = 0;

  while (true)
  {
    if (test)
      PORTC |= (1 << PINC2);
    else
      PORTC &= ~(1 << PINC2);

    if (millis() - prevTime >= 1000)
    {
      test = !test;
      prevTime = millis();
    }

    /*
      if (test)
      {
        uart_putc(0x42);
        uart_putc(digitToSegment[2]);
        uart_putc(digitToSegment[1]);
        uart_putc(digitToSegment[9]);
        uart_putc(0);
        uart_putc(1);

        PORTB |= (1 << PINB5);
      }
      else
      {
        uart_putc(0x42);
        uart_putc(digitToSegment[2]);
        uart_putc(digitToSegment[1]);
        uart_putc(0);
        uart_putc(digitToSegment[9]);
        uart_putc(1);

        
              uart_putc(0x42);
              uart_putc(digitToSegment[5]);
              uart_putc(digitToSegment[1]);
              uart_putc(digitToSegment[14]);
              uart_putc(digitToSegment[6]);
              uart_putc(0);
                      

        PORTB &= ~(1 << PINB5);
      }

      wait_ms(10);

      if (PINC & (1 << PINC0))
        test = true;
      else
        test = false;
            */
  }
}
