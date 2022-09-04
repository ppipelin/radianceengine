#pragma once

#include "include.h"
#include "cMove.h"
#include "boardParser.h"
#include "search.h"
#include "searchRandom.h"
#include "evaluate.h"


/**
	* @brief Game	class
	* @details	This class handles game event.
	* It contains the BoardParser and is used by the UI.
	* It has to communicate with a Search and an Evaluate class.
	* @todo
	* - Support 1v1
	* - Support 1vAI
	*/
class Game
{
protected:
	Search *m_search;
	Evaluate *m_evaluate;

public:
	BoardParser *m_boardParser;

	Game()
	{
		m_boardParser = new BoardParser();
		m_search = new SearchRandom();
	}
	Game(const Game &) {}
	~Game() {}

	cMove nextMove();

	cMove makeNextMove()
	{
		cMove move = m_search->nextMove(m_boardParser);
		m_boardParser->movePiece(move);
		return move;
	}
};
