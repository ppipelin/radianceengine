#include <string>
#include <vector>

#include "board.h"
#include "cMove.h"
#include "include.h"
#include "knight.h"

void Knight::canAttack(Bitboard &bb) const
{
	bool lc = Board::leftCol(m_tile);
	bool llc = lc || Board::leftCol(m_tile - 1);
	bool rc = Board::rightCol(m_tile);
	bool rrc = rc || Board::rightCol(m_tile + 1);

	//	2 bottom
	if (Int(m_tile) - Int(BOARD_SIZE) * 2 >= 0)
	{
		// Left
		if (!lc)
		{
			UInt tile = m_tile - BOARD_SIZE * 2 - 1;
			bb |= Bitboards::tileToBB(tile);
		}
		// Right
		if (!rc)
		{
			UInt tile = m_tile - BOARD_SIZE * 2 + 1;
			bb |= Bitboards::tileToBB(tile);
		}
	}

	// 2 mid-bottom
	if (Int(m_tile) - Int(BOARD_SIZE) >= 0)
	{
		// Left
		if (!llc)
		{
			UInt tile = m_tile - BOARD_SIZE - 2;
			bb |= Bitboards::tileToBB(tile);
		}
		// Right
		if (!rrc)
		{
			UInt tile = m_tile - BOARD_SIZE + 2;
			bb |= Bitboards::tileToBB(tile);
		}
	}

	// 2 mid-top
	if (m_tile + BOARD_SIZE < BOARD_SIZE2)
	{
		// Left
		if (!llc)
		{
			UInt tile = m_tile + BOARD_SIZE - 2;
			bb |= Bitboards::tileToBB(tile);
		}
		// Right
		if (!rrc)
		{
			UInt tile = m_tile + BOARD_SIZE + 2;
			bb |= Bitboards::tileToBB(tile);
		}
	}

	// 2 top
	if (m_tile + BOARD_SIZE * 2 < BOARD_SIZE2)
	{
		// Left
		if (!lc)
		{
			UInt tile = m_tile + BOARD_SIZE * 2 - 1;
			bb |= Bitboards::tileToBB(tile);
		}
		// Right
		if (!rc)
		{
			UInt tile = m_tile + BOARD_SIZE * 2 + 1;
			bb |= Bitboards::tileToBB(tile);
		}
	}
}

void Knight::canMove(const Board &, std::vector<cMove> &v, bool legal, bool capture) const
{
	v.reserve(v.size() + 8);

	// Generate possible moves for the knight
	Bitboard pseudoLegalMovesBB = Bitboards::movesKnight[m_tile];

	// Mask out our own pieces
	Bitboard pseudoLegalQuietBB = pseudoLegalMovesBB & ~Bitboards::bbPieces[PieceType::ALL];

	// Iterate over possible quiet moves
	while (pseudoLegalQuietBB)
	{
		v.push_back(cMove(m_tile, Bitboards::popLeastBit(pseudoLegalQuietBB), 0));
	}

	// Iterate over possible capture moves
	Bitboard pseudoLegalCapturesBB = pseudoLegalMovesBB & Bitboards::bbColors[Color(!isWhite())];
	while (pseudoLegalCapturesBB)
	{
		v.push_back(cMove(m_tile, Bitboards::popLeastBit(pseudoLegalCapturesBB), 4));
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
