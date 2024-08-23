#include <array>
#include <string>
#include <vector>

#include "board.h"
#include "cMove.h"
#include "include.h"
#include "rook.h"

void Rook::canAttack(Bitboard &bb) const
{
	const Bitboard blockers = Bitboards::movesRookMask[m_tile] & Bitboards::bbPieces[PieceType::ALL];
	Bitboard pseudoLegalMovesBBPartial = 0;
	if (blockers == 0)
		pseudoLegalMovesBBPartial = Bitboards::movesRook[m_tile];
	else
		pseudoLegalMovesBBPartial = Bitboards::movesRookLegal[m_tile][blockers];

	bb = pseudoLegalMovesBBPartial & Bitboards::allPiecesColor(Color(!isWhite()));
}

void Rook::canMove(const Board &, std::vector<cMove> &v, bool legal, bool capture) const
{
	v.reserve(v.size() + 14);
	const Bitboard blockers = Bitboards::movesRookMask[m_tile] & Bitboards::bbPieces[PieceType::ALL];
	Bitboard pseudoLegalMovesBBPartial = 0;
	if (blockers == 0)
		pseudoLegalMovesBBPartial = Bitboards::movesRook[m_tile];
	else
		pseudoLegalMovesBBPartial = Bitboards::movesRookLegal[m_tile][blockers];
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

bool Rook::exists() const
{
	return true;
}

std::string Rook::str() const
{
	return m_isWhite ? "R" : "r";
}
