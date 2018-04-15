#include "buttonFunctions.h"

OneButton button_p1_1(&PINA, 1);
OneButton button_p1_2(&PINA, 2);

OneButton button_p2_1(&PINC, 0);
OneButton button_p2_2(&PINB, 3);

extern Player playerOne;
extern Player playerTwo;

void buttons_init()
{
  button_p1_1.attachClick(playerOne_btn1_click);
  button_p1_1.attachLongPressStart(playerOne_btn1_longPressStart);

  button_p1_2.attachClick(playerOne_btn2_click);
  button_p1_2.attachLongPressStart(playerOne_btn2_longPressStart);

  button_p2_1.attachClick(playerTwo_btn1_click);
  button_p2_1.attachLongPressStart(playerTwo_btn1_longPressStart);

  button_p2_2.attachClick(playerTwo_btn2_click);
  button_p2_2.attachLongPressStart(playerTwo_btn2_longPressStart);
}

void checkForButtonUpdates(uint8_t interruptFlags)
{
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
}

void playerOne_btn1_click()
{
  // TODO: add debounce 2 sec
  playerOne.incrementScore();
}

void playerOne_btn1_longPressStart()
{

  playerOne.decrementScore();
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
  playerTwo.incrementScore();
}

void playerTwo_btn1_longPressStart()
{
  playerTwo.decrementScore();
}

void playerTwo_btn2_click()
{
}

void playerTwo_btn2_longPressStart()
{
}
