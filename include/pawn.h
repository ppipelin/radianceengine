#pragma once

#include "piece.h"

constexpr Bitboard filterCaptures(UInt tile, Color col)
{
	Bitboard filterAdjacent = Bitboards::filterAdjacent(tile);
	Bitboard filterTopBot = Bitboards::row << (BOARD_SIZE * (Board::row(tile) + ((col == Color::WHITE) ? 1 : -1)));
	return filterAdjacent & filterTopBot;
}

constexpr Bitboard filterForward(UInt tile, Color col)
{
	Bitboard b = 0ULL;
	if (col == Color::WHITE)
	{
		if (Board::row(tile) == 1)
			b = Bitboards::tileToBB(tile) << BOARD_SIZE * 2;
		b |= Bitboards::tileToBB(tile) << BOARD_SIZE;
	}
	else
	{
		if (Board::row(tile) == BOARD_SIZE - 2)
			b = Bitboards::tileToBB(tile) >> BOARD_SIZE * 2;
		b |= Bitboards::tileToBB(tile) >> BOARD_SIZE;
	}
	return b;
}

class Pawn : public Piece
{
public:
	Pawn(UInt a, UInt b, bool isWhite = true, bool neverMoved = true) : Piece(a, b, isWhite, neverMoved) {}
	Pawn(UInt a, bool isWhite = true, bool neverMoved = true) : Piece(a, isWhite, neverMoved) {}
	~Pawn() override = default;
	Pawn(const Piece &p)
	{
		m_tile = p.tile();
		m_isWhite = p.isWhite();
		m_neverMoved = p.neverMoved();
	}

	PieceType value() const override { return PieceType::PAWN; }

	void canMove(const Board &b, std::vector<cMove> &v) const;

	bool exists() const override;

	std::string str() const override;
};
