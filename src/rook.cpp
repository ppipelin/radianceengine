#include <array>
#include <string>
#include <vector>

#include "board.h"
#include "cMove.h"
#include "include.h"
#include "rook.h"

void Rook::canMove(const Board &, std::vector<cMove> &v) const
{
	v.reserve(v.size() + 14);
	const Bitboard blockers = Bitboards::movesRookMask[m_tile] & (Bitboards::bbColors[Color::BLACK] | Bitboards::bbColors[Color::WHITE]);
	Bitboard pseudoLegalMovesBBPartial = 0;
	if (blockers == 0)
		pseudoLegalMovesBBPartial = Bitboards::movesRook[m_tile];
	else
		pseudoLegalMovesBBPartial = Bitboards::movesRookLegal[m_tile][blockers];
	Bitboard pseudoLegalQuietBB = pseudoLegalMovesBBPartial & ~(Bitboards::bbColors[Color::BLACK] | Bitboards::bbColors[Color::WHITE]);
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

bool Rook::exists() const
{
	return true;
}

std::string Rook::str() const
{
	return m_isWhite ? "R" : "r";
}
