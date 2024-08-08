#include <string>
#include <vector>

#include "board.h"
#include "cMove.h"
#include "include.h"
#include "pawn.h"

void Pawn::canMove(const Board &b, std::vector<cMove> &v) const
{
	// Default behavior, we push_back() to v
	std::vector<cMove> tmp = std::vector<cMove>();
	std::vector<cMove> *vRef = &tmp;
	// Promotion cases uses temporary vector to duplicate moveset during promotion
	const bool promotion = (m_isWhite && Board::row(m_tile + BOARD_SIZE) == BOARD_SIZE - 1) || (!m_isWhite && Board::row(m_tile - BOARD_SIZE) == 0);
	if (promotion)
	{
		vRef->reserve(4);
		v.reserve(v.size() + 4 * 4);
	}
	else
	{
		vRef = &v;
		vRef->reserve(vRef->size() + 4);
	}

	Color col = Color(m_isWhite);
	Int enPassantCol = b.enPassant();
	if (enPassantCol != -1)
	{
		Bitboards::generateMoves(*vRef, Bitboards::filterEnPassantComputed[m_tile - BOARD_SIZE][col][enPassantCol], m_tile, 5);
	}
	Bitboard filterC = Bitboards::filterCapturesComputed[m_tile - BOARD_SIZE][col] & Bitboards::bbColors[Color(!m_isWhite)];
	Bitboards::generateMoves(*vRef, filterC, m_tile, 4);

	// A filter way to prevent going through a piece when moving by two tiles is to double the third row up and sixth row down
	Bitboard thirdOrSixthDoubled = 0ULL;
	// Remove m_tile in case pawn is on this row
	const Bitboard tileBB = Bitboards::tileToBB(m_tile);
	if (m_isWhite)
		thirdOrSixthDoubled = ((Bitboards::row << BOARD_SIZE * 2) & Bitboards::bbPieces[PieceType::ALL] & ~tileBB) << BOARD_SIZE;
	else
		thirdOrSixthDoubled = ((Bitboards::row << BOARD_SIZE * 5) & Bitboards::bbPieces[PieceType::ALL] & ~tileBB) >> BOARD_SIZE;
	Bitboard filterF = Bitboards::filterForwardComputed[m_tile - BOARD_SIZE][col] & ~Bitboards::bbPieces[PieceType::ALL] & ~thirdOrSixthDoubled;

#ifdef _MSC_VER
	Bitboard bb = Bitboards::filterForwardComputed[m_tile - BOARD_SIZE][col];
	UInt cnt = 0;
	while (bb)
	{
		if (Bitboards::tileToBB(Bitboards::popLeastBit(bb)) & filterF)
			vRef->push_back(Bitboards::filterForwardComputedMoves[m_tile - BOARD_SIZE][col][cnt]);
		++cnt;
	}
#else
	Bitboards::generateMoves(*vRef, filterF, m_tile, 0);
#endif // _MSC_VER

	// If we are going to the last rank, previous computed moves are promotions
	if ((m_isWhite && Board::row(m_tile + BOARD_SIZE) == BOARD_SIZE - 1) || (!m_isWhite && Board::row(m_tile - BOARD_SIZE) == 0))
	{
		for (auto &move : *vRef)
		{
			if (move.isCapture())
			{
				move.setFlags(12);
				v.push_back(move);
				move.setFlags(13);
				v.push_back(move);
				move.setFlags(14);
				v.push_back(move);
				move.setFlags(15);
				v.push_back(move);
			}
			else
			{
				move.setFlags(8);
				v.push_back(move);
				move.setFlags(9);
				v.push_back(move);
				move.setFlags(10);
				v.push_back(move);
				move.setFlags(11);
				v.push_back(move);
			}
		}
	}
}

bool Pawn::exists() const
{
	return true;
}

std::string Pawn::str() const
{
	return m_isWhite ? "P" : "p";
}
