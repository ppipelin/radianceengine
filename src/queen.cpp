#include "queen.h"

void Queen::canMove(const Board &b, std::vector<cMove> &v) const
{
	v.reserve(v.size() + 27);
	const Bitboard blockersRook = Bitboards::movesRookMask[m_tile] & Bitboards::bbPieces[PieceType::ALL];
	Bitboard pseudoLegalMovesBBPartialRook = 0;
	if (blockersRook == 0)
		pseudoLegalMovesBBPartialRook = Bitboards::movesRook[m_tile];
	else
		pseudoLegalMovesBBPartialRook = Bitboards::movesRookLegal[m_tile][blockersRook];

	const Bitboard blockersBishop = Bitboards::movesBishopMask[m_tile] & Bitboards::bbPieces[PieceType::ALL];
	Bitboard pseudoLegalMovesBBPartialBishop = 0;
	if (blockersBishop == 0)
		pseudoLegalMovesBBPartialBishop = Bitboards::movesBishop[m_tile];
	else
		pseudoLegalMovesBBPartialBishop = Bitboards::movesBishopLegal[m_tile][blockersBishop];

	Bitboard pseudoLegalQuietBB = (pseudoLegalMovesBBPartialRook | pseudoLegalMovesBBPartialBishop) & ~Bitboards::bbPieces[PieceType::ALL];
	Bitboard pseudoLegalCapturesBB = (pseudoLegalMovesBBPartialRook | pseudoLegalMovesBBPartialBishop) & Bitboards::allPiecesColor(Color(!isWhite()));

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

bool Queen::exists() const
{
	return true;
}

std::string Queen::str() const
{
	return m_isWhite ? "Q" : "q";
}
