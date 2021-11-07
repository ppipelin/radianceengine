#include "knight.h"

void Knight::canMove(const Board &b, std::vector<UInt> &v) const
{
	v.reserve(8);
	bool lc = Board::leftCol(m_tile) || Board::leftCol(m_tile - 1);
	bool llc = lc || Board::leftCol(m_tile - 2);
	bool rc = Board::rightCol(m_tile) || Board::rightCol(m_tile + 1);
	bool rrc = rc || Board::rightCol(m_tile + 2);

	// 2 top
	if (m_tile + BOARD_SIZE * 2 < BOARD_SIZE2)
	{
		// Left
		if (!lc)
		{
			v.push_back(m_tile + BOARD_SIZE * 2 - 1);
		}
		// Right
		if (!rc)
		{
			v.push_back(m_tile + BOARD_SIZE * 2 + 1);
		}
	}

	// 2 mid-top
	if (m_tile + BOARD_SIZE < BOARD_SIZE2)
	{
		// Left
		if (!llc)
		{
			v.push_back(m_tile + BOARD_SIZE - 2);
		}
		// Right
		if (!rrc)
		{
			v.push_back(m_tile + BOARD_SIZE + 2);
		}

		// 2 mid-bottom
		if (Int(m_tile) - BOARD_SIZE >= 0)
		{
			// Left
			if (!llc)
			{
				v.push_back(m_tile - BOARD_SIZE - 2);
			}
			// Right
			if (!rc)
			{
				v.push_back(m_tile - BOARD_SIZE + 2);
			}
		}

		//	2 bottom
		if (Int(m_tile) - BOARD_SIZE * 2 >= 0)
		{
			// Left
			if (!lc)
			{
				v.push_back(m_tile - BOARD_SIZE * 2 - 1);
			}
			// Right
			if (!rc)
			{
				v.push_back(m_tile - BOARD_SIZE * 2 + 1);
			}
		}
	}
}

bool Knight::exists() const
{
	return true;
}

std::string Knight::str() const
{
	return m_isWhite ? "N" : "n";
}
