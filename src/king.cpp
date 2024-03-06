#include "king.h"

void King::canMove(const Board &b, std::vector<cMove> &v) const
{
	v.reserve(v.size() + 8);
	const bool left = Board::leftCol(m_tile);
	const bool right = Board::rightCol(m_tile);
	// Go top
	UInt forward = m_tile + BOARD_SIZE;
	if (forward < BOARD_SIZE2 && !Board::topRow(m_tile))
	{
		if (!left && (b[forward - 1] == nullptr || b[forward - 1]->isWhite() != m_isWhite))
		{
			v.push_back(cMove(m_tile, forward - 1, b[forward - 1] == nullptr ? 0 : 4));
		}
		if (!right && (b[forward + 1] == nullptr || b[forward + 1]->isWhite() != m_isWhite))
		{
			v.push_back(cMove(m_tile, forward + 1, b[forward + 1] == nullptr ? 0 : 4));
		}
		if (b[forward] == nullptr || b[forward]->isWhite() != m_isWhite)
		{
			v.push_back(cMove(m_tile, forward, b[forward] == nullptr ? 0 : 4));
		}
	}
	//	Go bottom
	Int	backward = m_tile - BOARD_SIZE;
	if (backward >= 0 && !Board::botRow(m_tile))
	{
		if (!left && (b[backward - 1] == nullptr || b[backward - 1]->isWhite() != m_isWhite))
		{
			v.push_back(cMove(m_tile, backward - 1, b[backward - 1] == nullptr ? 0 : 4));
		}
		if (!right && (b[backward + 1] == nullptr || b[backward + 1]->isWhite() != m_isWhite))
		{
			v.push_back(cMove(m_tile, backward + 1, b[backward + 1] == nullptr ? 0 : 4));
		}
		if (b[backward] == nullptr || b[backward]->isWhite() != m_isWhite)
		{
			v.push_back(cMove(m_tile, backward, b[backward] == nullptr ? 0 : 4));
		}
	}
	//	Go left
	if (!left && (b[m_tile - 1] == nullptr || b[m_tile - 1]->isWhite() != m_isWhite))
	{
		v.push_back(cMove(m_tile, m_tile - 1, b[m_tile - 1] == nullptr ? 0 : 4));
	}
	//	Go right
	if (!right && (b[m_tile + 1] == nullptr || b[m_tile + 1]->isWhite() != m_isWhite))
	{
		v.push_back(cMove(m_tile, m_tile + 1, b[m_tile + 1] == nullptr ? 0 : 4));
	}

	// Castling, checks move is semi-legal (no check verification).
	// Queen side
	if (m_isWhite && (*b.m_castleInfo & 0b1000) || !m_isWhite && (*b.m_castleInfo & 0b0010))
	{
		if (b[m_tile - 1] == nullptr && b[m_tile - 2] == nullptr && b[m_tile - 3] == nullptr)
		{
			v.push_back(cMove(m_tile, m_tile - 2, 3));
		}
	}

	// King	side
	if (m_isWhite && (*b.m_castleInfo & 0b0100) || !m_isWhite && (*b.m_castleInfo & 0b0001))
	{
		if (b[m_tile + 1] == nullptr && b[m_tile + 2] == nullptr)
		{
			v.push_back(cMove(m_tile, m_tile + 2, 2));
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
