#pragma once

enum class IndividualStates
{
  SCORE,
  SERVES,
  TEMP
};

enum class CommonlyStates
{
  STARTUP_SEQ,
  STARTUP_HELLO,
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
  ACCU_WARN
};

enum class ServesPlayer
{
  NOBODY,
  PLAYERONE,
  PLAYERTWO
};
