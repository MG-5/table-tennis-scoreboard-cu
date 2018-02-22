#ifndef F_CPU
#error F_CPU not defined
#endif

#include "timer0.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#if defined(__AVR_ATmega328P__)
#define TIMER0_CONTROLB TCCR0B
#define TIMER0_REGISTER TCNT0
#define TIMER0_IMR TIMSK0

#elif defined(__AVR_ATtiny4313__) || defined(__AVR_ATtiny1634__)
#define TIMER0_CONTROLB TCCR0B
#define TIMER0_REGISTER TCNT0
#define TIMER0_IMR TIMSK

#else
#error No device defined!
#endif

#define clockCyclesPerMicrosecond() (F_CPU / 1000000L)
#define clockCyclesToMicroseconds(a) (((a)*1000L) / (F_CPU / 1000L))
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile uint32_t timer0_millis = 0;
volatile uint32_t timer0_fract = 0;
volatile uint16_t timer0_overflows = 0;

void timer0_init()
{
  TIMER0_CONTROLB |= (1 << CS01) | (1 << CS00); // Prescaler 64
  TIMER0_IMR |= (1 << TOIE0);                   // enable overflow interrupt
}

uint32_t millis()
{
  unsigned long m;
  uint8_t oldSREG = SREG;

  // disable interrupts while we read timer0_millis or we might get an
  // inconsistent value (e.g. in the middle of a write to timer0_millis)
  cli();
  m = timer0_millis;
  SREG = oldSREG;

  return m;
}

uint32_t micros()
{
  unsigned long m;
  uint8_t oldSREG = SREG, t;

  cli();
  m = timer0_overflows;
  t = TIMER0_REGISTER;

#ifdef TIFR0
  if ((TIFR0 & (TOV0 << 1)) && (t < 255))
    m++;
#else
  if ((TIFR & (TOV0 << 1)) && (t < 255))
    m++;
#endif

  SREG = oldSREG;

  return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}

ISR(TIMER0_OVF_vect)
{
  uint32_t m = timer0_millis;
  uint32_t f = timer0_fract;

  m += MILLIS_INC;
  f += FRACT_INC;

  if (f >= FRACT_MAX)
  {
    f -= FRACT_MAX;
    m += 1;
  }

  timer0_fract = f;
  timer0_millis = m;

  timer0_overflows++;
}
