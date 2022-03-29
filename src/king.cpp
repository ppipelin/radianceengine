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
		if (!left && (b[forward - 1] == nullptr || b[forward - 1]->isWhite() != m_isWhite))
		{
			v.push_back(forward - 1);
		}
		if (!right && (b[forward + 1] == nullptr || b[forward + 1]->isWhite() != m_isWhite))
		{
			v.push_back(forward + 1);
		}
		if (b[forward] == nullptr || b[forward]->isWhite() != m_isWhite)
		{
			v.push_back(forward);
		}
	}
	//	Go bottom
	UInt	backward = m_tile - BOARD_SIZE;
	if (backward >= 0 && !Board::botRow(m_tile))
	{
		if (!left && (b[backward - 1] == nullptr || b[backward - 1]->isWhite() != m_isWhite))
		{
			v.push_back(backward - 1);
		}
		if (!right && (b[backward + 1] == nullptr || b[backward + 1]->isWhite() != m_isWhite))
		{
			v.push_back(backward + 1);
		}
		if (b[backward] == nullptr || b[backward]->isWhite() != m_isWhite)
		{
			v.push_back(backward);
		}
	}
	//	Go left
	if (!left && (b[m_tile - 1] == nullptr || b[m_tile - 1]->isWhite() != m_isWhite))
	{
		v.push_back(m_tile - 1);
	}
	//	Go right
	if (!right && (b[m_tile + 1] == nullptr || b[m_tile + 1]->isWhite() != m_isWhite))
	{
		v.push_back(m_tile + 1);
	}

	// Castling
	// Queen	side
	if (m_isWhite && b.m_castleAvailableQueenWhite || !m_isWhite && b.m_castleAvailableQueenBlack)
	{
		if (b[m_tile - 1] == nullptr && b[m_tile - 2] == nullptr && b[m_tile - 3] == nullptr)
		{
			v.push_back(m_tile - 2);
		}
	}

	// King	side
	if (m_isWhite && b.m_castleAvailableKingWhite || !m_isWhite && b.m_castleAvailableKingBlack)
	{
		if (b[m_tile + 1] == nullptr && b[m_tile + 2] == nullptr)
		{
			v.push_back(m_tile + 2);
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
