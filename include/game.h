#pragma once

#include "include.h"
#include "boardParser.h"

/**
 * @brief Game	class
 * @details	This class handles game event.
 * It contains the BoardParser and handles the communication with the UI.
 * It has to communicate with a Search and an Evaluate class.
 * @todo
 * - Support 1v1
 * - Support 1vAI
 */
class Game
{
private:
 BoardParser *m_boardParser;

public:
 Game();
 Game(const Game &g);
 ~Game();

 void makeMove(UInt from, UInt to);
 void unmakeMove(UInt from, UInt to);
};
