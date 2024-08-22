#pragma once

#include "piece.h"

class Bishop : public Piece
{
public:
	Bishop(UInt a, UInt b, bool isWhite = true, bool neverMoved = true) : Piece(a, b, isWhite, neverMoved) {}
	Bishop(UInt a, bool isWhite = true, bool neverMoved = true) : Piece(a, isWhite, neverMoved) {}
	~Bishop() override = default;
	Bishop(const Piece &p)
	{
		m_tile = p.tile();
		m_isWhite = p.isWhite();
		m_neverMoved = p.neverMoved();
	}

	PieceType value() const override { return PieceType::BISHOP; }

	static constexpr Bitboard filterMoves(UInt tile, bool mask = false)
	{
		Bitboard b = 0ULL;
		const Bitboard tileBB = Bitboards::tileToBB(tile);

		// Surely not the fastest: finds the diagonals that collides with tile
		for (UInt i = 0; i < BOARD_SIZE - 1; ++i)
		{
			// Shifts is bounded by overflow
			// Diagonals go up
			const Bitboard computedClockwiseUp = Bitboards::diagonalClockwise << (i * BOARD_SIZE);
			if (computedClockwiseUp & tileBB)
				b |= computedClockwiseUp;

			const Bitboard computedCounterClockwiseUp = Bitboards::diagonalCounterClockwise << (i * BOARD_SIZE);
			if (computedCounterClockwiseUp & tileBB)
				b |= computedCounterClockwiseUp;

			// Diagonals go down
			const Bitboard computedClockwiseDown = Bitboards::diagonalClockwise >> (i * BOARD_SIZE);
			if (computedClockwiseDown & tileBB)
				b |= computedClockwiseDown;

			const Bitboard computedCounterClockwiseDown = Bitboards::diagonalCounterClockwise >> (i * BOARD_SIZE);
			if (computedCounterClockwiseDown & tileBB)
				b |= computedCounterClockwiseDown;
		}

		b &= ~Bitboards::tileToBB(tile);

		if (mask)
		{
			b &= ~Bitboards::column;
			b &= ~(Bitboards::column << (BOARD_SIZE - 1));
			b &= ~Bitboards::row;
			b &= ~(Bitboards::row << (BOARD_SIZE - 1) * BOARD_SIZE);
		}
		return b;
	}

	void canMove(const Board &b, std::vector<cMove> &v) const;

	bool exists() const override;

	std::string str() const override;
};
