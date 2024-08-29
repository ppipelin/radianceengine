#include <string>
#include <vector>

#include "board.h"
#include "cMove.h"
#include "include.h"
#include "knight.h"

void Knight::canMove(const Board &b, std::vector<cMove> &v) const
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
