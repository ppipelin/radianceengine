#include "king.h"

void King::canMove(const Board &b, std::vector<UInt> &v) const
{
	v.reserve(8);
	const bool left = Board::leftCol(m_tile);
	const bool right = Board::rightCol(m_tile);
	// Go top
	UInt forward = m_tile + BOARD_SIZE;
	if (forward < BOARD_SIZE2 && !Board::topRow(m_tile))
	{
		if (!left && (!b.board()[forward - 1]->exists() || b.board()[forward - 1]->isWhite() != m_isWhite))
		{
			v.push_back(forward - 1);
		}
		else	if (!right && (!b.board()[forward + 1]->exists() || b.board()[forward + 1]->isWhite() != m_isWhite))
		{
			v.push_back(forward + 1);
		}
		if (!b.board()[forward]->exists() || b.board()[forward]->isWhite() != m_isWhite)
		{
			v.push_back(forward);
		}
	}
	//	Go bottom
	UInt	backward = m_tile - BOARD_SIZE;
	if (backward >= 0 && !Board::botRow(m_tile))
	{
		if (!left && (!b.board()[backward - 1]->exists() || b.board()[backward - 1]->isWhite() != m_isWhite))
		{
			v.push_back(backward - 1);
		}
		else	if (!right && (!b.board()[backward + 1]->exists() || b.board()[backward + 1]->isWhite() != m_isWhite))
		{
			v.push_back(backward + 1);
		}
		if (!b.board()[backward]->exists() || b.board()[backward]->isWhite() != m_isWhite)
		{
			v.push_back(backward);
		}
	}
}

bool King::exists() const
{
	return true;
}

std::string King::str() const
{
	return m_isWhite ? "K" : "k";
}
