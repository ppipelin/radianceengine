#pragma once

#include "include.h"

class Piece;

class Board
{
private:
	std::vector<Piece *> *m_board;
	//  std::vector<bool> * m_empty;
public:
	Board()
	{
		// m_board.reserve( BOARD_SIZE * BOARD_SIZE);
		// for (Int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i)
		// {
		//  m_board.push_back(new Piece(false, false, i)); // TO CHECK if first is true 
		// }
		m_board = new std::vector<Piece *>(BOARD_SIZE * BOARD_SIZE);
		// m_empty = new std::vector<bool>( BOARD_SIZE * BOARD_SIZE , false); // init as if all cases where used
		// fillBoard(m_starting);
	}

	~Board()
	{
		delete m_board;
		// delete m_empty;
	}

	// Mutators
	std::vector<Piece *> &board()
	{
		return *m_board;
	}

	//  std::vector<bool> &empty() {
	//   return *m_empty;
	//  }

	// const std::vector<const Piece*> board() const {
	//  return m_board;
	// }

	//  bool isFree(Int tile) {
	//   // return true;//decltype(m_board[tile]) == Piece;
	//    return m_empty->at(tile);
	//  }

	bool sameLine(Int a, Int b) const
	{
		return floor(a / BOARD_SIZE) == floor(b / BOARD_SIZE);
	}

	bool sameColumn(Int a, Int b) const
	{
		return a % BOARD_SIZE == b % BOARD_SIZE;
	}

	static bool firstCol(Int tile)
	{
		return tile % BOARD_SIZE == 0; // is 0 % smth always true ?
	}

	static bool lastCol(Int tile)
	{
		return tile % (BOARD_SIZE - 1) == 0;
	}
};
