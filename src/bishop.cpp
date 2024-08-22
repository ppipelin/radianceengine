#include <array>
#include <string>
#include <vector>

#include "bishop.h"
#include "board.h"
#include "cMove.h"
#include "include.h"

void Bishop::canMove(const Board &, std::vector<cMove> &v) const
{
	v.reserve(v.size() + 13);
	const Bitboard blockers = Bitboards::movesBishopMask[m_tile] & Bitboards::bbPieces[PieceType::ALL];
	Bitboard pseudoLegalMovesBBPartial = 0;
	if (blockers == 0)
		pseudoLegalMovesBBPartial = Bitboards::movesBishop[m_tile];
	else
		pseudoLegalMovesBBPartial = Bitboards::movesBishopLegal[m_tile][blockers];
	Bitboard pseudoLegalQuietBB = pseudoLegalMovesBBPartial & ~Bitboards::bbPieces[PieceType::ALL];
	Bitboard pseudoLegalCapturesBB = pseudoLegalMovesBBPartial & Bitboards::allPiecesColor(Color(!isWhite()));

	// Quiet moves
	while (pseudoLegalQuietBB)
	{
		v.push_back(cMove(m_tile, Bitboards::popLeastBit(pseudoLegalQuietBB), 0));
	}

	// Capture moves
	while (pseudoLegalCapturesBB)
	{
		v.push_back(cMove(m_tile, Bitboards::popLeastBit(pseudoLegalCapturesBB), 4));
	}
}

bool Bishop::exists() const
{
	return true;
}

std::string Bishop::str() const
{
	return m_isWhite ? "B" : "b";
}
