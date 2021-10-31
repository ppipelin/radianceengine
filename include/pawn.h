#pragma once

#include "piece.h"

class Pawn : public Piece
{
public:
	// a is letter, b is number. Both starts at 0
	// tile is computed by substracting to 64 the sum of the cases from the bottom right to left.
	Pawn(bool isWhite, bool neverMoved, Int a, Int b) : Piece(isWhite, neverMoved, 64 - (BOARD_SIZE * b + (BOARD_SIZE - a))) {}
	Pawn(bool isWhite, bool neverMoved, Int a) : Piece(isWhite, neverMoved, a) {}
	~Pawn() {}
	// Returns a vector of the cases where Piece can move.
	std::vector<Int> canMove(Board &b)
	{
		std::vector<Int> v;
		if (m_isWhite)
		{
			// Can move forward
			if (floor(m_tile / BOARD_SIZE) > 0)
			{
				// Forward and double forward
				if (!b.board().at(m_tile - BOARD_SIZE)->exists())
					v.push_back(m_tile - BOARD_SIZE);
				if (floor(m_tile / BOARD_SIZE) == BOARD_SIZE - 2 && !b.board().at(m_tile - BOARD_SIZE * 2)->exists())
					v.push_back(m_tile - BOARD_SIZE * 2);
				// Forward left (checks black + not on first column)
				if (!Board::firstCol(m_tile - BOARD_SIZE - 1) && b.board().at(m_tile - BOARD_SIZE - 1)->exists() && !b.board().at(m_tile - BOARD_SIZE - 1)->isWhite())
					v.push_back(m_tile - BOARD_SIZE - 1);
				// One black forward right (checks black + not on last column)
				if (!Board::lastCol(m_tile - BOARD_SIZE + 1) && b.board().at(m_tile - BOARD_SIZE + 1)->exists() && !b.board().at(m_tile - BOARD_SIZE + 1)->isWhite())
					v.push_back(m_tile - BOARD_SIZE + 1);
			}
		}
		else
		{
			// Can move forward
			if (floor(m_tile / BOARD_SIZE) < BOARD_SIZE - 1)
			{
				// Forward and double forward
				if (!b.board().at(m_tile + BOARD_SIZE)->exists())
					v.push_back(m_tile + BOARD_SIZE);
				if (floor(m_tile / BOARD_SIZE) == 1 && !b.board().at(m_tile + BOARD_SIZE * 2)->exists())
					v.push_back(m_tile + BOARD_SIZE * 2);
				// Forward left (checks white + not on first column)
				if (!Board::firstCol(m_tile + BOARD_SIZE - 1) && b.board().at(m_tile + BOARD_SIZE - 1)->exists() && b.board().at(m_tile + BOARD_SIZE - 1)->isWhite())
					v.push_back(m_tile + BOARD_SIZE - 1);
				// Forward right (checks white + not on last column)
				if (!Board::lastCol(m_tile + BOARD_SIZE + 1) && b.board().at(m_tile + BOARD_SIZE + 1)->exists() && b.board().at(m_tile + BOARD_SIZE + 1)->isWhite())
					v.push_back(m_tile + BOARD_SIZE + 1);
			}
		}
		return v;
	}

	bool exists() override
	{
		return true;
	}

	const std::string str() override
	{
		return m_isWhite ? "P" : "p";
	}
};
