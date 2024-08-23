#pragma once

#include "piece.h"

class Rook : public Piece
{
public:
	Rook(UInt a, UInt b, bool isWhite = true, bool neverMoved = true) : Piece(a, b, isWhite, neverMoved) {}
	Rook(UInt a, bool isWhite = true, bool neverMoved = true) : Piece(a, isWhite, neverMoved) {}
	~Rook() override = default;
	Rook(const Piece &p)
	{
		m_tile = p.tile();
		m_isWhite = p.isWhite();
		m_neverMoved = p.neverMoved();
	}

	PieceType value() const override { return PieceType::ROOK; }

	static constexpr Bitboard filterMoves(UInt tile, bool mask = false)
	{
		Bitboard b = 0ULL;
		const UInt currentCol = Board::column(tile);
		const UInt currentRow = Board::row(tile);

		b |= Bitboards::column << currentCol;
		b |= Bitboards::row << (currentRow * BOARD_SIZE);
		b &= ~Bitboards::tileToBB(tile);

		if (mask)
		{
			b &= ~Bitboards::tileToBB(0 + currentCol); // Bottom
			b &= ~Bitboards::tileToBB(BOARD_SIZE2 - BOARD_SIZE + currentCol); // Top
			b &= ~Bitboards::tileToBB(BOARD_SIZE - 1 + (currentRow * BOARD_SIZE)); // Right
			b &= ~Bitboards::tileToBB(currentRow * BOARD_SIZE); // Left
		}
		return b;
	}

	void canAttack(Bitboard &bb) const;

	void canMove(const Board &b, std::vector<cMove> &v, bool legal, bool capture) const;

	bool exists() const override;

	std::string str() const override;
};
