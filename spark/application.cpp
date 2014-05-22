#include "application.h"

const char PONG_DEBUG = 1;

static char score[4] = "0-0";
static char paddle1 = 0;
static char paddle2 = 0;

bool increment_score(char player)
{
  if ('9' == score[0] || '9' == score[2])
    return false;
  if (1 == player) {
    score[0]++;
  } else if (2 == player) {
    score[2]++;
  }
  return true;
}

int control_player1(String cmd)
{
  if (PONG_DEBUG) Serial.print("Player 1 ");
  if ('u' == cmd[0]) {
    paddle1 = 1;
    if (PONG_DEBUG) Serial.println("up!");
  } else if ('d' == cmd[0]) {
    paddle1 = -1;
    if (PONG_DEBUG) Serial.println("down!");
  }
  return paddle1;
}

int control_player2(String cmd)
{
  if (PONG_DEBUG) Serial.print("Player 2 ");
  if ('u' == cmd[0]) {
    paddle2 = 1;
    if (PONG_DEBUG) Serial.println("up!");
  } else if ('d' == cmd[0]) {
    paddle2 = -1;
    if (PONG_DEBUG) Serial.println("down!");
  }
  return paddle2;
}

int reset_game(String)
{
  strncpy(score, "0-0", 4);
  if (PONG_DEBUG) Serial.println("Score reset!");
  return 0;
}

void setup()
{
  Spark.function("p1", control_player1);
  Spark.function("p2", control_player2);
  Spark.function("reset", reset_game);
  Spark.variable("score", score, STRING);
  if (PONG_DEBUG) Serial.begin(115200);
}

void updateDisplay()
{

}

void loop()
{
  static system_tick_t last = millis();
  if (millis() - last > 3000)
  {
    char player = 1 + (rand() & 1);
    increment_score(player);

    if (PONG_DEBUG)
    {
      Serial.print("Incrementing player ");
      Serial.print((int)player);
      Serial.print("'s score. New score: ");
      Serial.println(score);
    }

    last = millis();
  }

  updateDisplay();
}
