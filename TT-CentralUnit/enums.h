#pragma once

enum class IndividualStates
{
  SCORE,
  SERVES,
  TEMP
};

enum class CommonlyStates
{
  STARTUP,
  WAITING,
  RANKING,
  ERROR
};

enum class Mode
{
  INDIVIDUAL,
  COMMONLY
};

enum class Ranks
{
  NOBODY,
  PLAYER_ONE,
  PLAYER_TWO
};

enum class ShowMode
{
  MODE1,
  MODE2
};

enum class Errors
{
  NOTHING,
  NO_CONN_DISP_P1,
  NO_CONN_DISP_P2,
  ACCU_WARN,
  ACCU_CRITCIAL
};

enum class ServesPlayer
{
  NOBODY,
  PLAYERONE,
  PLAYERTWO
};
