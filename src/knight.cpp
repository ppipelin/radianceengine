#include "knight.h"

void Knight::canMove(const Board &b, std::vector<UInt> &v) const
{
	v.reserve(8);
	bool lc = Board::leftCol(m_tile);
	bool llc = lc || Board::leftCol(m_tile - 1);
	bool rc = Board::rightCol(m_tile);
	bool rrc = rc || Board::rightCol(m_tile + 1);

	// 2 top
	if (m_tile + BOARD_SIZE * 2 < BOARD_SIZE2)
	{
		// Left
		if (!lc)
		{
			UInt tile = m_tile + BOARD_SIZE * 2 - 1;
			if (b[tile] == nullptr || b[tile]->isWhite() != isWhite())
				v.push_back(tile);
		}
		// Right
		if (!rc)
		{
			UInt tile = m_tile + BOARD_SIZE * 2 + 1;
			if (b[tile] == nullptr || b[tile]->isWhite() != isWhite())
				v.push_back(tile);
		}
	}

	// 2 mid-top
	if (m_tile + BOARD_SIZE < BOARD_SIZE2)
	{
		// Left
		if (!llc)
		{
			UInt tile = m_tile + BOARD_SIZE - 2;
			if (b[tile] == nullptr || b[tile]->isWhite() != isWhite())
				v.push_back(tile);
		}
		// Right
		if (!rrc)
		{
			UInt tile = m_tile + BOARD_SIZE + 2;
			if (b[tile] == nullptr || b[tile]->isWhite() != isWhite())
				v.push_back(tile);
		}
	}

	// 2 mid-bottom
	if (Int(m_tile) - Int(BOARD_SIZE) >= 0)
	{
		// Left
		if (!llc)
		{
			UInt tile = m_tile - BOARD_SIZE - 2;
			if (b[tile] == nullptr || b[tile]->isWhite() != isWhite())
				v.push_back(tile);
		}
		// Right
		if (!rrc)
		{
			UInt tile = m_tile - BOARD_SIZE + 2;
			if (b[tile] == nullptr || b[tile]->isWhite() != isWhite())
				v.push_back(tile);
		}
	}

	//	2 bottom
	if (Int(m_tile) - Int(BOARD_SIZE) * 2 >= 0)
	{
		// Left
		if (!lc)
		{
			UInt tile = m_tile - BOARD_SIZE * 2 - 1;
			if (b[tile] == nullptr || b[tile]->isWhite() != isWhite())
				v.push_back(tile);
		}
		// Right
		if (!rc)
		{
			UInt tile = m_tile - BOARD_SIZE * 2 + 1;
			if (b[tile] == nullptr || b[tile]->isWhite() != isWhite())
				v.push_back(tile);
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
