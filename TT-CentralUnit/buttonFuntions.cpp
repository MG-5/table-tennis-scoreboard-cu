#include "buttonFunctions.h"

OneButton button_p1_1(&PINA, 1);
OneButton button_p1_2(&PINA, 2);

OneButton button_p2_1(&PINC, 0);
OneButton button_p2_2(&PINB, 3);

OneButton button_cu_1(&PINA, 5);

extern Player playerOne;
extern Player playerTwo;
extern CommonlyStates currentState_common;
extern Mode currentMode;
extern ShowMode currentShowMode;
extern ServesPlayer currentPlayer;

extern bool playATone;
extern Tone currentTone;

uint32_t prevTimeBtn_p1 = 0;
uint32_t prevTimeBtn_p2 = 0;

bool alreadyDecremented_p1 = false;
bool alreadyDecremented_p2 = false;

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

  button_cu_1.attachClick(centralUnit_btn1_click);
  button_cu_1.attachLongPressStart(centralUnit_btn1_longPressStart);

  button_cu_1.setPressTicks(2000); // 2sec
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

  // check central unit first button
  if (button_cu_1.state != 0)
    button_cu_1.tick();

  if (check_bit(interruptFlags, 4))
  {
    clear_bit(interruptFlags, 4);
    button_cu_1.tick();
  }
}

void playerOne_btn1_click()
{
  // 1 sec debounce
  if (millis() - prevTimeBtn_p1 >= 1000 && currentMode == Mode::INDIVIDUAL &&
      playerOne.state == IndividualStates::SCORE)
  {
    prevTimeBtn_p1 = millis();
    alreadyDecremented_p1 = false;
    playerOne.incrementScore();

    playATone = true;
    currentTone = Tone::BUTTON_ACK;

    updateServes(true);
    compareScores();
  }
}

void playerOne_btn1_longPressStart()
{
  if (currentMode == Mode::COMMONLY && currentState_common == CommonlyStates::WAITING)
  {
    playerOne.refillServes();
    currentMode = Mode::INDIVIDUAL;
    playerOne.state = IndividualStates::SERVES;
    playerTwo.state = IndividualStates::SERVES;
    currentPlayer = ServesPlayer::PLAYERONE;
    clearTimeVariables();

    playATone = true;
    currentTone = Tone::SERVES_CHANGE;
  }
  else if (currentMode == Mode::INDIVIDUAL && playerOne.state == IndividualStates::SCORE)
  {
    // only one decrement is prohibited
    if (!alreadyDecremented_p1 && playerOne.getScore() != 0)
    {
      alreadyDecremented_p1 = true;
      playerOne.decrementScore();

      playATone = true;
      currentTone = Tone::BUTTON_ACK;

      updateServes(false);
    }
  }
}

void playerOne_btn2_click()
{
  clearTimeVariables();
  playerOne.state = IndividualStates::SERVES;
  playerTwo.state = IndividualStates::SERVES;
}

void playerOne_btn2_longPressStart()
{
  if (currentMode == Mode::COMMONLY && currentState_common == CommonlyStates::WAITING)
  {
    playerOne.refillServes();
    currentMode = Mode::INDIVIDUAL;
    playerOne.state = IndividualStates::SERVES;
    playerTwo.state = IndividualStates::SERVES;
    currentPlayer = ServesPlayer::PLAYERONE;
    clearTimeVariables();

    playATone = true;
    currentTone = Tone::SERVES_CHANGE;
  }
  else if (currentMode == Mode::INDIVIDUAL)
  {
    clearTimeVariables_p1();
    playerOne.state = IndividualStates::TEMP;
  }
}

void playerTwo_btn1_click()
{
  // 1 sec debounce
  if (millis() - prevTimeBtn_p2 >= 1000 && currentMode == Mode::INDIVIDUAL &&
      playerTwo.state == IndividualStates::SCORE)
  {
    prevTimeBtn_p2 = millis();
    alreadyDecremented_p2 = false;
    playerTwo.incrementScore();

    playATone = true;
    currentTone = Tone::BUTTON_ACK;

    updateServes(true);
    compareScores();
  }
}

void playerTwo_btn1_longPressStart()
{
  if (currentMode == Mode::COMMONLY && currentState_common == CommonlyStates::WAITING)
  {
    playerTwo.refillServes();
    currentMode = Mode::INDIVIDUAL;
    playerOne.state = IndividualStates::SERVES;
    playerTwo.state = IndividualStates::SERVES;
    currentPlayer = ServesPlayer::PLAYERTWO;
    clearTimeVariables();

    playATone = true;
    currentTone = Tone::SERVES_CHANGE;
  }
  else if (currentMode == Mode::INDIVIDUAL && playerTwo.state == IndividualStates::SCORE)
  {
    // only one decrement is prohibited
    if (!alreadyDecremented_p2 && playerTwo.getScore() != 0)
    {
      alreadyDecremented_p2 = true;
      playerTwo.decrementScore();

      playATone = true;
      currentTone = Tone::BUTTON_ACK;
	  
      updateServes(false);
    }
  }
}

void playerTwo_btn2_click()
{
  clearTimeVariables();
  playerOne.state = IndividualStates::SERVES;
  playerTwo.state = IndividualStates::SERVES;
}

void playerTwo_btn2_longPressStart()
{
  if (currentMode == Mode::COMMONLY && currentState_common == CommonlyStates::WAITING)
  {
    playerTwo.refillServes();
    currentMode = Mode::INDIVIDUAL;
    playerOne.state = IndividualStates::SERVES;
    playerTwo.state = IndividualStates::SERVES;
    currentPlayer = ServesPlayer::PLAYERTWO;
    clearTimeVariables();

    playATone = true;
    currentTone = Tone::SERVES_CHANGE;
  }
  else if (currentMode == Mode::INDIVIDUAL)
  {
    clearTimeVariables_p2();
    playerTwo.state = IndividualStates::TEMP;
  }
}

void centralUnit_btn1_click()
{
  // TODO: show accu voltage
}

void centralUnit_btn1_longPressStart()
{
  playerOne.clearScore();
  playerOne.clearServes();

  playerTwo.clearScore();
  playerTwo.clearServes();

  currentMode = Mode::COMMONLY;
  currentState_common = CommonlyStates::WAITING;
  currentShowMode = ShowMode::MODE1;

  playATone = true;
  currentTone = Tone::GAME_START;
}
