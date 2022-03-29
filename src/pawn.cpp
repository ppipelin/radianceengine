#include "pawn.h"

void Pawn::canMove(const Board &b, std::vector<UInt> &v) const
{
	v.reserve(4);
	if (m_isWhite)
	{
		// Can move forward
		if (Board::row(m_tile) < BOARD_SIZE - 1)
		{
			UInt forward = m_tile + BOARD_SIZE;
			// Forward and double forward
			if (b[forward] == nullptr)
			{
				v.push_back(forward);
				if (Board::row(m_tile) == 1 && b[forward + BOARD_SIZE] == nullptr)
					v.push_back(forward + BOARD_SIZE);
			}
			// Forward left (checks white + not on first column)
			if (!Board::leftCol(m_tile) && b[forward - 1] != nullptr && !b[forward - 1]->isWhite())
				v.push_back(forward - 1);
			// Forward right (checks white + not on last column)
			if (!Board::rightCol(m_tile) && b[forward + 1] != nullptr && !b[forward + 1]->isWhite())
				v.push_back(forward + 1);
		}
	}
	else
	{
		// Can move forward
		if (Board::row(m_tile) > 0)
		{
			UInt forward = m_tile - BOARD_SIZE;
			// Forward and double forward
			if (b[forward] == nullptr)
			{
				v.push_back(forward);
				if (Board::row(m_tile) == BOARD_SIZE - 2 && b[forward - BOARD_SIZE] == nullptr)
					v.push_back(forward - BOARD_SIZE);
			}
			// Forward left (checks black + not on first column)
			if (!Board::leftCol(m_tile) && b[forward - 1] != nullptr && b[forward - 1]->isWhite())
				v.push_back(forward - 1);
			// One black forward right (checks black + not on last column)
			if (!Board::rightCol(m_tile) && b[forward + 1] != nullptr && b[forward + 1]->isWhite())
				v.push_back(forward + 1);
		}
	}
}

bool Pawn::exists() const
{
	return true;
}

std::string Pawn::str() const
{
	return m_isWhite ? "P" : "p";
}
